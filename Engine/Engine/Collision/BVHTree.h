#pragma once
#include <vector>
#include <SFML/Graphics/Rect.hpp>

#include "CollisionStructs.h"

namespace diji
{
    class Collider;

    // todo: this tree doesn't support adding new colliders after the initial build.
    class BVHTree final
    {
    public:
        struct Node
        {
            sf::FloatRect aabb;
            uint32_t left = UINT16_MAX;
            uint32_t right = UINT16_MAX;

            Collider* collider = nullptr;
        };

        BVHTree() = default;
        ~BVHTree() = default;

        BVHTree(const BVHTree& other) = delete;
        BVHTree(BVHTree&& other) = delete;
        BVHTree& operator=(const BVHTree& other) = delete;
        BVHTree& operator=(BVHTree&& other) = delete;

        void Build(const std::vector<StaticColliderInfo>& staticColliders);
        [[nodiscard]] const std::vector<StaticColliderInfo>& Query(const sf::FloatRect& queryAABB);
        
    private:
        std::vector<StaticColliderInfo> m_CollidersInfo;
        std::vector<StaticColliderInfo> m_CollidersResults;
        std::vector<Node> m_Nodes;
        
        void QueryNode(uint32_t nodeIdx, const sf::FloatRect& queryAABB);
        uint32_t BuildNode(uint32_t begin, uint32_t end);
        sf::FloatRect ComputeBounds(uint32_t begin, uint32_t end) const;
    };
}
