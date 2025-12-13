#pragma once
#include "CollisionStructs.h"

#include <memory>
#include <vector>
#include <SFML/Graphics/Rect.hpp>

namespace diji
{
    class Collider;
    class QuadTree final
    {
    public:
        explicit QuadTree(const sf::FloatRect& bounds, int capacity = 25, int maxDepth = 12, int depth = 0);
        ~QuadTree() = default;

        QuadTree(const QuadTree& other) = delete;
        QuadTree(QuadTree&& other) = delete;
        QuadTree& operator=(const QuadTree& other) = delete;
        QuadTree& operator=(QuadTree&& other) = delete;

        struct QuadTreeNodeCluster
        {
            std::vector<Collider*> dynamic;
            std::vector<StaticColliderInfo> statics;
        };
        
        void Insert(Collider* collider, bool isStatic, const sf::FloatRect& aabb = sf::FloatRect());
        std::vector<QuadTreeNodeCluster> Query(const std::vector<Collider*>& dynamicColliders, const std::vector<StaticColliderInfo>& staticColliders);
        void SetWorldBounds(const sf::FloatRect& bounds) { m_WorldBounds = bounds; }

    private:
        enum class Quadrant : std::uint8_t
        {
            INVALID = 255,
            NW = 0,
            NE = 1,
            SW = 2,
            SE = 3
        };
        
        sf::FloatRect m_WorldBounds;
        int m_Capacity;
        int m_MaxDepth;
        int m_Depth;
        
        std::vector<Collider*> m_Dynamic;
        std::vector<StaticColliderInfo> m_Static;

        std::unique_ptr<QuadTree> m_Children[4];   // NW, NE, SW, SE
        void Clear();



        // Subdivide node into 4 children
        void Subdivide();

        // Determine which child fully contains the collider's AABB
        [[nodiscard]] Quadrant GetChildQuadrant(const sf::FloatRect& aabb) const;
        void GetOverlappingChildren(const sf::FloatRect& aabb, std::array<bool, 4>& overlaps) const;

        // Recursive query helper
        void QueryNode(std::vector<QuadTreeNodeCluster>& clusters) const;
    };
}
