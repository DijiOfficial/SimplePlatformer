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

diji::PhysicsWorld::CollisionDetectionResult diji::CollisionDispatcher::Dispatch(PhysicsWorld::Prediction& prediction, const Collider* dynamic, const Collider* staticCol) const
{
    const auto dynamicType = static_cast<int>(dynamic->GetShapeType());
    const auto staticType = static_cast<int>(staticCol->GetShapeType());

    if (const auto handler = collisionTable_[dynamicType][staticType])
        return handler(prediction, dynamic, staticCol);

    throw std::runtime_error("No collision handler for given shape types.");
}

diji::PhysicsWorld::CollisionDetectionResult diji::CollisionDispatcher::HandleCircleCircle(PhysicsWorld::Prediction& pred, const Collider* dynamic, const Collider* staticCol)
{
    return DispatchCollision<sf::CircleShape, sf::CircleShape>(pred, dynamic, staticCol, CollisionsHelper::ProcessCircleToCircleCollision);
}

diji::PhysicsWorld::CollisionDetectionResult diji::CollisionDispatcher::HandleCircleRect(PhysicsWorld::Prediction& pred, const Collider* dynamic, const Collider* staticCol)
{
    return DispatchCollision<sf::CircleShape, sf::RectangleShape>(pred, dynamic, staticCol, CollisionsHelper::ProcessCircleToBoxCollision);
}

diji::PhysicsWorld::CollisionDetectionResult diji::CollisionDispatcher::HandleRectRect(PhysicsWorld::Prediction& pred, const Collider* dynamic, const Collider* staticCol)
{
    return DispatchCollision<sf::RectangleShape, sf::RectangleShape>(pred, dynamic, staticCol, CollisionsHelper::ProcessBoxToBoxCollision);
}