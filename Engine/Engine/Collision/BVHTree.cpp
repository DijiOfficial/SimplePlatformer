#include "BVHTree.h"
#include "PhysicsWorld.h"
#include <algorithm>

void diji::BVHTree::Build(const std::vector<StaticColliderInfo>& staticColliders)
{
    m_Nodes.clear();

    if (staticColliders.empty())
        return;
    
    m_Nodes.reserve(staticColliders.size() * 2 - 1);
    m_CollidersInfo = staticColliders;

    BuildNode(0, static_cast<uint32_t>(staticColliders.size()));
}

const std::vector<diji::StaticColliderInfo>& diji::BVHTree::Query(const sf::FloatRect& queryAABB)
{
    m_CollidersResults.clear();
    QueryNode(0, queryAABB);
    
    return m_CollidersResults;
}

void diji::BVHTree::QueryNode(const uint32_t nodeIdx, const sf::FloatRect& queryAABB)
{
    const auto& [aabb, left, right, collider] = m_Nodes[nodeIdx];

    if (PhysicsWorld::AABBOverlap(aabb, queryAABB) == false)
        return;

    if (collider)
    {
        m_CollidersResults.push_back({ .aabb= aabb, .collider= collider });
        return;
    }

    QueryNode(left, queryAABB);
    QueryNode(right, queryAABB);
}

// Median split with max 1 collider per leaf node. Could extend to allow more colliders per leaf node depending on use case.
// todo: look into using SAH or Spatial Splitting
uint32_t diji::BVHTree::BuildNode(const uint32_t begin, const uint32_t end)
{
    const uint32_t nodeIndex = static_cast<uint32_t>(m_Nodes.size());
    m_Nodes.emplace_back();

    if (end - begin == 1)
    {
        m_Nodes[nodeIndex].aabb = m_CollidersInfo[begin].aabb;
        m_Nodes[nodeIndex].collider = m_CollidersInfo[begin].collider;
        return nodeIndex;
    }

    m_Nodes[nodeIndex].aabb = ComputeBounds(begin, end);

    bool splitX = m_Nodes[nodeIndex].aabb.size.x > m_Nodes[nodeIndex].aabb.size.y;
    const auto first = m_CollidersInfo.begin() + begin;
    const auto middle = first + (end - begin) / 2;
    const auto last = m_CollidersInfo.begin() + end;

    // std::sort(m_CollidersInfo.begin() + begin, m_CollidersInfo.begin() + end,
    std::nth_element(first, middle, last,
    [splitX](const auto& a, const auto& b)
    {
        const auto& aAABB = a.aabb;
        const auto& bAABB = b.aabb;

        const float aCenterAxisAligned = splitX ? aAABB.position.x + aAABB.size.x * 0.5f : aAABB.position.y + aAABB.size.y * 0.5f;
        const float bCenterAxisAligned = splitX ? bAABB.position.x + bAABB.size.x * 0.5f : bAABB.position.y + bAABB.size.y * 0.5f;

        return aCenterAxisAligned < bCenterAxisAligned;
    });

    const uint32_t mid = begin + (end - begin) / 2;

    m_Nodes[nodeIndex].left = BuildNode(begin, mid);
    m_Nodes[nodeIndex].right = BuildNode(mid, end);

    return nodeIndex;
}

sf::FloatRect diji::BVHTree::ComputeBounds(const uint32_t begin, const uint32_t end) const
{
    sf::FloatRect bounds = m_CollidersInfo[begin].aabb;

    for (uint32_t i = begin + 1; i < end; i++)
    {
        const sf::FloatRect& aabb = m_CollidersInfo[i].aabb;

        const float left = std::min(bounds.position.x, aabb.position.x);
        const float top = std::min(bounds.position.y, aabb.position.y);
        const float right = std::max(bounds.position.x + bounds.size.x, aabb.position.x + aabb.size.x);
        const float bottom = std::max(bounds.position.y + bounds.size.y, aabb.position.y + aabb.size.y);

        bounds = sf::FloatRect{ sf::Vector2f{ left, top }, sf::Vector2f{ right - left, bottom - top } };
    }

    return bounds;
}
