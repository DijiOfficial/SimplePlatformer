#include "QuadTree.h"
#include "Collider.h"

diji::QuadTree::QuadTree(const sf::FloatRect& bounds, const int capacity, const int maxDepth, const int depth)
    : m_WorldBounds{ bounds }
    , m_Capacity{ capacity }
    , m_MaxDepth{ maxDepth }
    , m_Depth{ depth }
{
}

void diji::QuadTree::Insert(Collider* collider, const bool isStatic, const sf::FloatRect& aabb)
{
    const sf::FloatRect& box = isStatic ? aabb : collider->GetAABB();

    // If we already have children, insert into all overlapping children
    if (m_Children[0])
    {
        std::array<bool, 4> overlaps;
        GetOverlappingChildren(box, overlaps);

        bool inserted = false;
        for (int i = 0; i < 4; ++i)
        {
            if (overlaps[i])
            {
                m_Children[i]->Insert(collider, isStatic, box);
                inserted = true;
            }
        }

        if (inserted)
            return; // do NOT store in parent
    }

    // Store in this node
    if (isStatic)
        m_Static.push_back({ box, collider });
    else
        m_Dynamic.push_back(collider);

    // Subdivide if capacity exceeded
    if (static_cast<int>(m_Dynamic.size() + m_Static.size()) <= m_Capacity || m_Depth >= m_MaxDepth)
        return;

    if (!m_Children[0])
        Subdivide();

    // Reinsert dynamic colliders into children where possible
    for (size_t i = 0; i < m_Dynamic.size(); )
    {
        std::array<bool, 4> overlaps;
        GetOverlappingChildren(m_Dynamic[i]->GetAABB(), overlaps);

        bool moved = false;
        for (int q = 0; q < 4; ++q)
        {
            if (overlaps[q])
            {
                m_Children[q]->Insert(m_Dynamic[i], false);
                moved = true;
            }
        }

        if (moved)
        {
            m_Dynamic[i] = m_Dynamic.back();
            m_Dynamic.pop_back();
        }
        else
        {
            ++i;
        }
    }

    // Reinsert static colliders into children where possible
    for (size_t i = 0; i < m_Static.size(); )
    {
        std::array<bool, 4> overlaps;
        GetOverlappingChildren(m_Static[i].aabb, overlaps);

        bool moved = false;
        for (int q = 0; q < 4; ++q)
        {
            if (overlaps[q])
            {
                m_Children[q]->Insert(m_Static[i].collider, true, m_Static[i].aabb);
                moved = true;
            }
        }

        if (moved)
        {
            m_Static[i] = m_Static.back();
            m_Static.pop_back();
        }
        else
        {
            ++i;
        }
    }
}

void diji::QuadTree::Subdivide()
{
    const float halfWidth = m_WorldBounds.size.x * 0.5f;
    const float halfHeight = m_WorldBounds.size.y * 0.5f;

    m_Children[0] = std::make_unique<QuadTree>(
        sf::FloatRect{sf::Vector2f{ m_WorldBounds.position.x, m_WorldBounds.position.y }, sf::Vector2f{ halfWidth, halfHeight } },
        m_Capacity, m_MaxDepth, m_Depth + 1);

    m_Children[1] = std::make_unique<QuadTree>(
        sf::FloatRect{sf::Vector2f{ m_WorldBounds.position.x + halfWidth, m_WorldBounds.position.y }, sf::Vector2f{ halfWidth, halfHeight } },
        m_Capacity, m_MaxDepth, m_Depth + 1);

    m_Children[2] = std::make_unique<QuadTree>(
        sf::FloatRect{sf::Vector2f{ m_WorldBounds.position.x, m_WorldBounds.position.y + halfHeight }, sf::Vector2f{ halfWidth, halfHeight } },
        m_Capacity, m_MaxDepth, m_Depth + 1);

    m_Children[3] = std::make_unique<QuadTree>(
        sf::FloatRect{sf::Vector2f{ m_WorldBounds.position.x + halfWidth, m_WorldBounds.position.y + halfHeight }, sf::Vector2f{ halfWidth, halfHeight } },
        m_Capacity, m_MaxDepth, m_Depth + 1);
}

diji::QuadTree::Quadrant diji::QuadTree::GetChildQuadrant(const sf::FloatRect& aabb) const
{
    const float centerX = m_WorldBounds.position.x + m_WorldBounds.size.x * 0.5f;
    const float centerY = m_WorldBounds.position.y + m_WorldBounds.size.y * 0.5f;

    const bool top = aabb.position.y + aabb.size.y <= centerY;
    const bool bottom = aabb.position.y >= centerY;
    const bool left = aabb.position.x + aabb.size.x <= centerX;
    const bool right = aabb.position.x >= centerX;

    if (top && left) return Quadrant::NW;    
    if (top && right) return Quadrant::NE;   
    if (bottom && left) return Quadrant::SW; 
    if (bottom && right) return Quadrant::SE; 

    return Quadrant::INVALID;
}

void diji::QuadTree::GetOverlappingChildren(const sf::FloatRect& aabb, std::array<bool, 4>& overlaps) const
{
    const float centerX = m_WorldBounds.position.x + m_WorldBounds.size.x * 0.5f;
    const float centerY = m_WorldBounds.position.y + m_WorldBounds.size.y * 0.5f;

    overlaps.fill(false);

    sf::FloatRect nw{ m_WorldBounds.position,
                      { centerX - m_WorldBounds.position.x,
                        centerY - m_WorldBounds.position.y } };

    sf::FloatRect ne{ { centerX, m_WorldBounds.position.y },
                      { m_WorldBounds.size.x * 0.5f,
                        m_WorldBounds.size.y * 0.5f } };

    sf::FloatRect sw{ { m_WorldBounds.position.x, centerY },
                      { m_WorldBounds.size.x * 0.5f,
                        m_WorldBounds.size.y * 0.5f } };

    sf::FloatRect se{ { centerX, centerY },
                      { m_WorldBounds.size.x * 0.5f,
                        m_WorldBounds.size.y * 0.5f } };

    overlaps[0] = nw.findIntersection(aabb).has_value();
    overlaps[1] = ne.findIntersection(aabb).has_value();
    overlaps[2] = sw.findIntersection(aabb).has_value();
    overlaps[3] = se.findIntersection(aabb).has_value();
}

std::vector<diji::QuadTree::QuadTreeNodeCluster> diji::QuadTree::Query(const std::vector<Collider*>& dynamicColliders, const std::vector<StaticColliderInfo>& staticColliders)
{
    Clear();
    
    // Insert colliders (both dynamic and static)
    for (auto* collider : dynamicColliders)
        Insert(collider, false);

    for (const auto [aabb, collider] : staticColliders)
        Insert(collider, true, aabb);

    std::vector<QuadTreeNodeCluster> clusters;
    clusters.reserve(64); // heuristic; tune as needed
    QueryNode(clusters);
    return clusters;
}

void diji::QuadTree::QueryNode(std::vector<QuadTreeNodeCluster>& clusters) const
{
    if (!m_Dynamic.empty() || !m_Static.empty())
    {
        QuadTreeNodeCluster cluster;
        cluster.dynamic = m_Dynamic;  
        cluster.statics = m_Static;
        clusters.push_back(std::move(cluster));
    }

    for (const auto& child : m_Children)
        if (child)
            child->QueryNode(clusters);
}

void diji::QuadTree::Clear()
{
    // Clear colliders in this node
    m_Dynamic.clear();
    m_Static.clear();

    // Recursively clear children
    for (auto& child : m_Children)
    {
        if (child)
        {
            child->Clear();
            child.reset(); // optional: free memory and reset pointer
        }
    }
}


