#include "CollisionDispatcher.h"
#include "CollisionShape.h"
#include "Collider.h"
#include "CollisionsHelper.h"

diji::CollisionDispatcher::CollisionDispatcher()
{
    // Initialize lookup table
    collisionTable_[static_cast<int>(CollisionShape::ShapeType::CIRCLE)][static_cast<int>(CollisionShape::ShapeType::CIRCLE)] = HandleCircleCircle;
    collisionTable_[static_cast<int>(CollisionShape::ShapeType::CIRCLE)][static_cast<int>(CollisionShape::ShapeType::RECT)] = HandleCircleRect;
    collisionTable_[static_cast<int>(CollisionShape::ShapeType::RECT)][static_cast<int>(CollisionShape::ShapeType::CIRCLE)] = HandleCircleRect;
    collisionTable_[static_cast<int>(CollisionShape::ShapeType::RECT)][static_cast<int>(CollisionShape::ShapeType::RECT)] = HandleRectRect;
    // ... initialize remaining combinations
}

diji::PhysicsWorld::CollisionDetectionResult diji::CollisionDispatcher::Dispatch(PhysicsWorld::Prediction& predictionA, PhysicsWorld::Prediction& predictionB, const Collider* colliderA, const Collider* colliderB) const
{
    const auto dynamicAType = static_cast<int>(colliderA->GetShapeType());
    const auto dynamicBType = static_cast<int>(colliderB->GetShapeType());

    if (const auto handler = collisionTable_[dynamicAType][dynamicBType])
        return handler(predictionA, predictionB, colliderA, colliderB);

    throw std::runtime_error("No collision handler for given shape types.");
}

diji::PhysicsWorld::CollisionDetectionResult diji::CollisionDispatcher::HandleCircleCircle(PhysicsWorld::Prediction& predA, PhysicsWorld::Prediction& predB, const Collider* colliderA, const Collider* colliderB)
{
    return DispatchCollision<sf::CircleShape, sf::CircleShape>(predA, predB, colliderA, colliderB, CollisionsHelper::ProcessCircleToCircleCollision);
}

diji::PhysicsWorld::CollisionDetectionResult diji::CollisionDispatcher::HandleCircleRect(PhysicsWorld::Prediction& predA, PhysicsWorld::Prediction& predB, const Collider* colliderA, const Collider* colliderB)
{
    return DispatchCollision<sf::CircleShape, sf::RectangleShape>(predA, predB, colliderA, colliderB, CollisionsHelper::ProcessCircleToBoxCollision);
}

diji::PhysicsWorld::CollisionDetectionResult diji::CollisionDispatcher::HandleRectRect(PhysicsWorld::Prediction& predA, PhysicsWorld::Prediction& predB, const Collider* colliderA, const Collider* colliderB)
{
    return DispatchCollision<sf::RectangleShape, sf::RectangleShape>(predA, predB, colliderA, colliderB, CollisionsHelper::ProcessBoxToBoxCollision);
}