#pragma once
#include <vector>
#include <stack>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

// A small, generic BVH that operates on axis-aligned FloatRects.
// Leaves store an integer "userIndex" which you decide (e.g. index into predictions or static array).
namespace diji
{
    class BVHTree
    {
    public:
        struct Node
        {
            sf::FloatRect aabb;
            int left = -1;         // index of left child (if internal)
            int right = -1;        // index of right child (if internal)
            int userIndex = -1;    // if leaf: index into user array
            bool isLeaf = false;
        };

        BVHTree() = default;

        // Build tree from provided list of AABBs. userIndices are implicit as 0..N-1.
        void BuildFromAABBs(const std::vector<sf::FloatRect>& aabbs);

        // Query all leaf userIndices whose AABB overlaps the query AABB
        void QueryOverlap(const sf::FloatRect& query, std::vector<int>& outIndices) const;

        // Raycast: optional, returns first hit userIndex (not required in this PR, but useful)
        int RaycastFirst(const sf::Vector2f& origin, const sf::Vector2f& dir, float maxDist) const;

        void Clear() { m_nodes.clear(); }

        bool Empty() const { return m_nodes.empty(); }

    private:
        std::vector<Node> m_nodes;

        static sf::FloatRect MergeAABB(const sf::FloatRect& a, const sf::FloatRect& b);
        static float Area(const sf::FloatRect& r);
        static bool Overlap(const sf::FloatRect& a, const sf::FloatRect& b);
    };
}
