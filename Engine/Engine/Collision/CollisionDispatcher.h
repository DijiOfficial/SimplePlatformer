#pragma once
#include "PhysicsWorld.h"
#include "Collider.h" // not ideal, when template in cpp?

#include <array>
#include <functional>

namespace diji
{
    class CollisionDispatcher final
    {
    public:
        CollisionDispatcher();
        ~CollisionDispatcher() noexcept = default;

        CollisionDispatcher(const CollisionDispatcher&) = delete;
        CollisionDispatcher& operator=(const CollisionDispatcher&) = delete;
        CollisionDispatcher(CollisionDispatcher&&) noexcept = default;
        CollisionDispatcher& operator=(CollisionDispatcher&&) noexcept = default;
        
        bool Dispatch(PhysicsWorld::Prediction& prediction, const Collider* dynamic, const Collider* staticCol) const;

    private:
        using CollisionFunc = std::function<bool(PhysicsWorld::Prediction&, const Collider*, const Collider*)>;
            
        // 2D lookup table indexed by shape types
        std::array<std::array<CollisionFunc, 3>, 3> collisionTable_;
            
        static bool HandleCircleCircle(PhysicsWorld::Prediction& pred, const Collider* dynamic, const Collider* staticCol);
        static bool HandleCircleRect(PhysicsWorld::Prediction& pred, const Collider* dynamic, const Collider* staticCol);
        static bool HandleRectRect(PhysicsWorld::Prediction& pred, const Collider* dynamic, const Collider* staticCol);

        template<typename DynamicShapeT, typename StaticShapeT>
        static bool DispatchCollision
        (
            PhysicsWorld::Prediction& pred,
            const Collider* dynamic,
            const Collider* staticCol,
            bool (*collisionFunc)
                (
                    const DynamicShapeT&, const StaticShapeT&, 
                    std::vector<PhysicsWorld::CollisionInfo>&, 
                    std::vector<PhysicsWorld::CollisionInfo>&,
                    bool
                )
        )
        {
            const bool isOverlap = dynamic->GetCollisionResponse() == Collider::CollisionResponse::Overlap ||
                                   staticCol->GetCollisionResponse() == Collider::CollisionResponse::Overlap;

            auto dynamicShape = *dynamic_cast<const DynamicShapeT*>(&dynamic->GetShape()->GetShape());
            dynamicShape.setPosition(pred.pos);

            const auto staticShape = dynamic_cast<const StaticShapeT*>(&staticCol->GetShape()->GetShape());

            std::vector<PhysicsWorld::CollisionInfo> emptyVec;
            return collisionFunc(dynamicShape, *staticShape, pred.collisionInfoVec, emptyVec, isOverlap);
        }
    };
}
