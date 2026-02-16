#include "BVHTree.h"
#include <algorithm>
#include <limits>
#include <cmath>
#include <functional>

using Node = diji::BVHTree::Node;

sf::FloatRect diji::BVHTree::MergeAABB(const sf::FloatRect& a, const sf::FloatRect& b)
{
    const float left = std::min(a.position.x, b.position.x);
    const float top = std::min(a.position.y, b.position.y);
    const float right = std::max(a.position.x + a.size.x, b.position.x + b.size.x);
    const float bottom = std::max(a.position.y + a.size.y, b.position.y + b.size.y);
    return sf::FloatRect{ sf::Vector2f{ left, top }, sf::Vector2f{ right - left, bottom - top } };
}

float diji::BVHTree::Area(const sf::FloatRect& r)
{
    return r.size.x * r.size.y;
}

bool diji::BVHTree::Overlap(const sf::FloatRect& a, const sf::FloatRect& b)
{
    return !(a.position.x + a.size.x < b.position.x || b.position.x + b.size.x < a.position.x ||
             a.position.y + a.size.y < b.position.y || b.position.y + b.size.y < a.position.y);
}

void diji::BVHTree::BuildFromAABBs(const std::vector<sf::FloatRect>& aabbs)
{
    m_nodes.clear();
    const int n = static_cast<int>(aabbs.size());
    if (n == 0) return;

    // Prepare array of leaf indices
    struct Item { int index; sf::FloatRect aabb; sf::Vector2f center; };
    std::vector<Item> items; items.reserve(n);
    for (int i = 0; i < n; ++i)
    {
        Item it;
        it.index = i;
        it.aabb = aabbs[i];
        it.center = { aabbs[i].position.x + aabbs[i].size.x * 0.5f, aabbs[i].position.y + aabbs[i].size.y * 0.5f };
        items.push_back(it);
    }

    // Recursively build with stack to avoid recursion depth issues
    struct BuildTask { int start, end, parentIndex; bool isLeft; };
    std::vector<BuildTask> tasks;
    tasks.push_back({ 0, static_cast<int>(items.size()), -1, false });

    // We'll maintain a parallel storage for temporary ordering of items
    std::vector<Item> localItems = std::move(items);

    // We will create nodes and assign leaves as we go
    // To make life easy, use a lambda that builds a subtree for [start,end) and returns node index
    std::function<int(int,int)> buildSubtree;
    buildSubtree = [&](const int start, const int end) -> int
    {
        const int count = end - start;
        if (count <= 0) return -1;

        Node node;
        if (count == 1)
        {
            node.aabb = localItems[start].aabb;
            node.isLeaf = true;
            node.userIndex = localItems[start].index;
            m_nodes.push_back(node);
            return static_cast<int>(m_nodes.size()) - 1;
        }

        // Compute bounding box and extents
        sf::FloatRect bounds = localItems[start].aabb;
        for (int i = start + 1; i < end; ++i)
            bounds = MergeAABB(bounds, localItems[i].aabb);

        const float width = bounds.size.x;
        const float height = bounds.size.y;
        const int axis = (width >= height) ? 0 : 1; // 0 = x, 1 = y

        // Partition by median of centers
        const int mid = start + count / 2;
        if (axis == 0)
        {
            std::nth_element(localItems.begin() + start, localItems.begin() + mid, localItems.begin() + end,
                [](const Item& a, const Item& b){ return a.center.x < b.center.x; });
        }
        else
        {
            std::nth_element(localItems.begin() + start, localItems.begin() + mid, localItems.begin() + end,
                [](const Item& a, const Item& b){ return a.center.y < b.center.y; });
        }

        // Create internal node
        Node internal;
        internal.isLeaf = false;
        // left and right will be filled after recursive calls
        m_nodes.push_back(internal);
        const int nodeIndex = static_cast<int>(m_nodes.size()) - 1;

        const int left = buildSubtree(start, mid);
        const int right = buildSubtree(mid, end);

        // compute AABB
        const sf::FloatRect leftAABB = (left >= 0) ? m_nodes[left].aabb : sf::FloatRect();
        const sf::FloatRect rightAABB = (right >= 0) ? m_nodes[right].aabb : sf::FloatRect();
        m_nodes[nodeIndex].left = left;
        m_nodes[nodeIndex].right = right;
        m_nodes[nodeIndex].aabb = MergeAABB(leftAABB, rightAABB);

        return nodeIndex;
    };

    // Build entire tree
    const int root = buildSubtree(0, static_cast<int>(localItems.size()));
    (void)root;
}

void diji::BVHTree::QueryOverlap(const sf::FloatRect& query, std::vector<int>& outIndices) const
{
    outIndices.clear();
    if (m_nodes.empty()) return;

    std::stack<int> stack;
    stack.push(static_cast<int>(m_nodes.size()) - 1); // root node

    while (!stack.empty())
    {
        const int ni = stack.top(); stack.pop();
        if (ni < 0) continue;
        const Node& n = m_nodes[ni];
        if (!Overlap(n.aabb, query)) continue;

        if (n.isLeaf)
        {
            outIndices.push_back(n.userIndex);
        }
        else
        {
            if (n.left >= 0) stack.push(n.left);
            if (n.right >= 0) stack.push(n.right);
        }
    }
}

int diji::BVHTree::RaycastFirst(const sf::Vector2f& origin, const sf::Vector2f& dir, float maxDist) const
{
    // Very simple slab test over nodes, returns first leaf hit (no sorting, may not be the closest)
    if (m_nodes.empty()) return -1;
    std::stack<int> stack;
    stack.push(static_cast<int>(m_nodes.size()) - 1);

    const sf::Vector2f invDir{ (dir.x == 0.0f) ? std::numeric_limits<float>::max() : 1.0f/dir.x,
                               (dir.y == 0.0f) ? std::numeric_limits<float>::max() : 1.0f/dir.y };

    auto slabTest = [&](const sf::FloatRect& r) -> bool
    {
        const float minX = r.position.x;
        const float maxX = r.position.x + r.size.x;
        const float t1 = (minX - origin.x) * invDir.x;
        const float t2 = (maxX - origin.x) * invDir.x;
        float tEnter = std::min(t1, t2);
        float tExit  = std::max(t1, t2);

        const float minY = r.position.y;
        const float maxY = r.position.y + r.size.y;
        const float t3 = (minY - origin.y) * invDir.y;
        const float t4 = (maxY - origin.y) * invDir.y;
        tEnter = std::max(tEnter, std::min(t3, t4));
        tExit  = std::min(tExit,  std::max(t3, t4));

        if (tExit < 0.0f || tEnter > tExit) return false;
        const float h = (tEnter >= 0.0f ? tEnter : tExit);
        return (h >= 0.0f && h <= maxDist);
    };

    while (!stack.empty())
    {
        const int ni = stack.top(); stack.pop();
        if (ni < 0) continue;
        const Node& n = m_nodes[ni];
        if (!slabTest(n.aabb)) continue;

        if (n.isLeaf)
            return n.userIndex;
        if (n.left >= 0) stack.push(n.left);
        if (n.right >= 0) stack.push(n.right);
    }
    return -1;
}
