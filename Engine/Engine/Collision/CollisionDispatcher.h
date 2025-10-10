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
        
        PhysicsWorld::CollisionDetectionResult Dispatch(PhysicsWorld::Prediction& predictionA, PhysicsWorld::Prediction& predictionB, const Collider* colliderA, const Collider* colliderB) const;

    private:
        using CollisionFunc = std::function<PhysicsWorld::CollisionDetectionResult(PhysicsWorld::Prediction&, PhysicsWorld::Prediction&, const Collider*, const Collider*)>;
            
        // 2D lookup table indexed by shape types
        std::array<std::array<CollisionFunc, 3>, 3> collisionTable_;
            
        static PhysicsWorld::CollisionDetectionResult HandleCircleCircle(   PhysicsWorld::Prediction& predA, PhysicsWorld::Prediction& predB, const Collider* colliderA, const Collider* colliderB);
        static PhysicsWorld::CollisionDetectionResult HandleCircleRect(     PhysicsWorld::Prediction& predA, PhysicsWorld::Prediction& predB, const Collider* colliderA, const Collider* colliderB);
        static PhysicsWorld::CollisionDetectionResult HandleRectRect(       PhysicsWorld::Prediction& predA, PhysicsWorld::Prediction& predB, const Collider* colliderA, const Collider* colliderB);

        template<typename ShapeA, typename ShapeB>
        static PhysicsWorld::CollisionDetectionResult DispatchCollision
        (
            PhysicsWorld::Prediction& predA,
            PhysicsWorld::Prediction& predB,
            const Collider* colliderA,
            const Collider* colliderB,
            PhysicsWorld::CollisionDetectionResult (*collisionFunc)
                (
                    const ShapeA&, const ShapeB&, 
                    std::vector<PhysicsWorld::CollisionInfo>&, 
                    std::vector<PhysicsWorld::CollisionInfo>&,
                    bool
                )
        )
        {
            const bool isOverlap = colliderA->GetCollisionResponse() == Collider::CollisionResponse::Overlap ||
                                   colliderB->GetCollisionResponse() == Collider::CollisionResponse::Overlap;

            auto shapeA = *dynamic_cast<const ShapeA*>(&colliderA->GetShape()->GetShape());
            shapeA.setPosition(predA.pos);

            auto shapeB = *dynamic_cast<const ShapeB*>(&colliderB->GetShape()->GetShape());
            if (!colliderB->IsStatic())
                shapeB.setPosition(predB.pos);

            return collisionFunc(shapeA, shapeB, predA.collisionInfoVec, predB.collisionInfoVec, isOverlap);
        }
    };
}
