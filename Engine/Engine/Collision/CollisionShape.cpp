#include "CollisionShape.h"

#include <algorithm>

#include "Collider.h"

std::vector<sf::Vector2f> diji::CollisionShape::GetCorners(const sf::RectangleShape& rect)
{
    std::vector<sf::Vector2f> corners;
    const sf::Transform& transform = rect.getTransform();

    for (size_t i = 0; i < rect.getPointCount(); ++i)
    {
        corners.push_back(transform.transformPoint(rect.getPoint(i)) + rect.getOrigin());
    }

    return corners;
}


std::vector<sf::Vector2f> diji::CollisionShape::GetAxes(const std::vector<sf::Vector2f>& corners)
{
    std::vector<sf::Vector2f> axes;
    for (size_t i = 0; i < corners.size(); ++i)
    {
        sf::Vector2f p1 = corners[i];
        sf::Vector2f p2 = corners[(i + 1) % corners.size()];
        const sf::Vector2f edge = p2 - p1;

        // todo: fast inverse sqrt?
        sf::Vector2f normal(-edge.y, edge.x);
        const float length = std::sqrt(normal.x * normal.x + normal.y * normal.y);
        if (length > 0.f)
            normal /= length;

        axes.push_back(normal);
    }
    return axes;
}

void diji::CollisionShape::ProjectOntoAxis(const std::vector<sf::Vector2f>& points, const sf::Vector2f& axis, float& min, float& max)
{
    min = max = points[0].x * axis.x + points[0].y * axis.y;
    for (size_t i = 1; i < points.size(); ++i)
    {
        const float projection = points[i].x * axis.x + points[i].y * axis.y;
        min = std::min(projection, min);
        max = std::max(projection, max);
    }
}

void diji::Rect::CollideWith(std::vector<PhysicsWorld::CollisionInfo>& collisionsVec, const PhysicsWorld::StaticColliderInfo& info, const sf::Vector2f& pos)
{
    const auto rectShape = dynamic_cast<const sf::RectangleShape*>(&GetShape());
    const auto other = dynamic_cast<const sf::RectangleShape*>(&info.collider->GetShape()->GetShape());
    if (!rectShape || !other)
    {
        throw std::runtime_error("Failed to get rectangle shape for collision detection.");
    }

    info.collider->GetShape()->HandleStaticCollisionWithRect(collisionsVec, *rectShape, pos, *other);
}

void diji::Rect::CollideWith(std::vector<PhysicsWorld::CollisionInfo>& collisionsVec, const sf::FloatRect& staticAABB, const sf::FloatRect& predictedAABB)
{
    // This only works for non-rotated rectangles (AABB)
    const float leftA   = predictedAABB.left;
    const float rightA  = PhysicsWorld::Right(predictedAABB);
    const float topA    = predictedAABB.top;
    const float bottomA = PhysicsWorld::Bottom(predictedAABB);

    const float leftB   = staticAABB.left;
    const float rightB  = PhysicsWorld::Right(staticAABB);
    const float topB    = staticAABB.top;
    const float bottomB = PhysicsWorld::Bottom(staticAABB);
    
    const float overlapX = std::min(rightA, rightB) - std::max(leftA, leftB);
    const float overlapY = std::min(bottomA, bottomB) - std::max(topA, topB);
    
    if (overlapX > 0.f && overlapY > 0.f)
    {
        PhysicsWorld::CollisionInfo collision;
        collision.hasCollision = true;
        
        // Choose smaller axis of penetration
        if (overlapX < overlapY)
        {
            // X-axis collision
            if (leftA < leftB)
            {
                // A is to the left of B, collision from left
                collision.normal = sf::Vector2f{-1.f, 0.f};
                collision.point  = sf::Vector2f{leftB, (topA + bottomA) * 0.5f};
            }
            else
            {
                // A is to the right of B, collision from right
                collision.normal = sf::Vector2f{1.f, 0.f};
                collision.point  = sf::Vector2f{rightB, (topA + bottomA) * 0.5f};
            }
    
            collision.tangent = sf::Vector2f{0.f, 1.f}; // Perpendicular to normal
            collision.penetration = overlapX;
        }
        else
        {
            // Y-axis collision
            if (topA < topB)
            {
                // A is above B, collision from top
                collision.normal = sf::Vector2f{0.f, -1.f};
                collision.point  = sf::Vector2f{(leftA + rightA) * 0.5f, topB};
            }
            else
            {
                // A is below B, collision from bottom
                collision.normal = sf::Vector2f{0.f, 1.f};
                collision.point  = sf::Vector2f{(leftA + rightA) * 0.5f, bottomB};
            }
    
            collision.tangent = sf::Vector2f{1.f, 0.f};
            collision.penetration = overlapY;
        }
    
        collisionsVec.push_back(collision);
    }
}

void diji::Rect::HandleStaticCollisionWithRect(std::vector<PhysicsWorld::CollisionInfo>& collisionsVec, const sf::RectangleShape& movingShape, const sf::Vector2f& predictedPos, const sf::RectangleShape& staticShape)
{
    (void)collisionsVec;
    (void)movingShape;
    (void)predictedPos;
    (void)staticShape;

    // SAT for rotated rectangles not working quite right
    // // Apply predicted position to moving shape transform
    // sf::RectangleShape movedShape = movingShape;
    // movedShape.setPosition(predictedPos);
    //
    // auto cornersA = GetCorners(movedShape);
    // auto cornersB = GetCorners(staticShape);
    //
    // auto axesA = GetAxes(cornersA);
    // auto axesB = GetAxes(cornersB);
    //
    // float minOverlap = std::numeric_limits<float>::max();
    // sf::Vector2f smallestAxis;
    // bool foundSeparation = false;
    //
    // // Test all axes from both shapes
    // for (const auto& axis : axesA)
    // {
    //     float minA, maxA, minB, maxB;
    //     ProjectOntoAxis(cornersA, axis, minA, maxA);
    //     ProjectOntoAxis(cornersB, axis, minB, maxB);
    //
    //     if (maxA < minB || maxB < minA)
    //     {
    //         foundSeparation = true;
    //         break; // No collision
    //     }
    //
    //     float overlap = std::min(maxA, maxB) - std::max(minA, minB);
    //     if (overlap < minOverlap)
    //     {
    //         minOverlap = overlap;
    //         smallestAxis = axis;
    //     }
    // }
    //
    // if (!foundSeparation)
    // {
    //     for (const auto& axis : axesB)
    //     {
    //         float minA, maxA, minB, maxB;
    //         ProjectOntoAxis(cornersA, axis, minA, maxA);
    //         ProjectOntoAxis(cornersB, axis, minB, maxB);
    //
    //         if (maxA < minB || maxB < minA)
    //         {
    //             foundSeparation = true;
    //             break; // No collision
    //         }
    //
    //         float overlap = std::min(maxA, maxB) - std::max(minA, minB);
    //         if (overlap < minOverlap)
    //         {
    //             minOverlap = overlap;
    //             smallestAxis = axis;
    //         }
    //     }
    // }
    //
    // if (!foundSeparation)
    // {
    //     PhysicsWorld::CollisionInfo collision;
    //     collision.hasCollision = true;
    //     collision.normal = smallestAxis;
    //     collision.penetration = minOverlap;
    //     collision.tangent = sf::Vector2f(-smallestAxis.y, smallestAxis.x);
    //
    //     // Estimate point of contact as center of overlap (approximation)
    //     collision.point = 0.5f * (movedShape.getPosition() + staticShape.getPosition());
    //
    //     collisionsVec.push_back(collision);
    // }


}

void diji::Rect::HandleStaticCollisionWithCircle(Circle&, const PhysicsWorld::StaticColliderInfo&)
{
}

void diji::Rect::HandleStaticCollisionWithTriangle(Triangle&, const PhysicsWorld::StaticColliderInfo&)
{
}
