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
        sf::FloatRect m_WorldBounds;
        int m_Capacity;
        int m_MaxDepth;
        int m_Depth;
        
        std::vector<Collider*> m_Dynamic;
        std::vector<StaticColliderInfo> m_Static;

        std::unique_ptr<QuadTree> m_Children[4];   // NW, NE, SW, SE
        void Clear();
        void Subdivide();
        void GetOverlappingChildren(const sf::FloatRect& aabb, std::array<bool, 4>& overlaps) const;
        void QueryNode(std::vector<QuadTreeNodeCluster>& clusters) const;
    };
}
