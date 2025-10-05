#include "CollisionsHelper.h"
#include "../Singleton/Helpers.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <algorithm>

#include "PhysicsWorld.h"

void diji::CollisionsHelper::ProcessCircleToCircleCollision(
    const sf::CircleShape& circleA,
    const sf::CircleShape& circleB,
    std::vector<PhysicsWorld::CollisionInfo>& collisionInfoVecA,
    std::vector<PhysicsWorld::CollisionInfo>& collisionInfoVecB)
{
    const sf::Vector2f centerA = circleA.getPosition() + circleA.getOrigin();
    const sf::Vector2f centerB = circleB.getPosition() + circleB.getOrigin();
    const float radiusA = circleA.getRadius();
    const float radiusB = circleB.getRadius();

    const float distanceSquared = Helpers::DistanceSquared(centerA, centerB);
    const float radiiSum = radiusA + radiusB;
    const float radiiSumSquared = radiiSum * radiiSum;

    if (distanceSquared >= radiiSumSquared) return;

    PhysicsWorld::CollisionInfo collision;
    collision.hasCollision = true;

    // Calculate actual distance and penetration depth
    const float distance = std::sqrt(distanceSquared);
    collision.normal = Helpers::Normalize(centerA - centerB);
    collision.penetration = radiiSum - distance;

    collisionInfoVecA.push_back(collision);
    collision.normal *= -1.f;
    collisionInfoVecB.push_back(collision);
}

// void diji::CollisionsHelper::ProcessCircleToBoxCollision(
//     const sf::CircleShape& circleA,
//     const sf::RectangleShape& rect,
//     std::vector<PhysicsWorld::CollisionInfo>& collisionInfoVecA,
//     std::vector<PhysicsWorld::CollisionInfo>& collisionInfoVecB)
// {
//     const sf::Vector2f circleCenter = circleA.getPosition() + circleA.getOrigin();
//     const float radius = circleA.getRadius();
//
//     // Get rectangle corners in world space (handles origin/scale even though not rotated)
//     const sf::Vector2f rectLocalTopLeft  = { 0.f, 0.f };
//     const sf::Vector2f rectLocalBottomRight = rect.getSize();
//
//     const sf::Transform rectTransform = rect.getTransform();
//     const sf::Vector2f worldA = rectTransform.transformPoint(rectLocalTopLeft);
//     const sf::Vector2f worldB = rectTransform.transformPoint(rectLocalBottomRight);
//
//     // Make sure we have proper min/max in case of flipped scales
//     const float minX = std::min(worldA.x, worldB.x);
//     const float maxX = std::max(worldA.x, worldB.x);
//     const float minY = std::min(worldA.y, worldB.y);
//     const float maxY = std::max(worldA.y, worldB.y);
//
//     auto clamp = [](float v, float lo, float hi) {
//         return std::max(lo, std::min(v, hi));
//     };
//
//     // Closest point on AABB to circle center
//     const sf::Vector2f closest{
//         clamp(circleCenter.x, minX, maxX),
//         clamp(circleCenter.y, minY, maxY)
//     };
//
//     const float distSq = Helpers::DistanceSquared(closest, circleCenter);
//     const float radiusSq = radius * radius;
//
//     if (distSq > radiusSq)
//         return; // no collision
//
//     PhysicsWorld::CollisionInfo collA;
//     collA.hasCollision = true;
//
//     // If the closest point is not the center -> circle center outside rect
//     if (distSq > 0.f)
//     {
//         const float distance = std::sqrt(distSq);
//         collA.normal = Helpers::Normalize(closest - circleCenter); // from circle -> rect
//         collA.penetration = radius - distance;
//         collA.point = closest;
//     }
//     else
//     {
//         // circle center is inside rectangle -> find nearest edge and build normal accordingly
//         const float dLeft   = circleCenter.x - minX;
//         const float dRight  = maxX - circleCenter.x;
//         const float dTop    = circleCenter.y - minY;
//         const float dBottom = maxY - circleCenter.y;
//
//         float minDist = dLeft;
//         enum Side { Left, Right, Top, Bottom } side = Left;
//
//         if (dRight < minDist) { minDist = dRight; side = Right; }
//         if (dTop   < minDist) { minDist = dTop;   side = Top;   }
//         if (dBottom< minDist) { minDist = dBottom;side = Bottom;}
//
//         // contact point on the rectangle edge (world space)
//         sf::Vector2f edgePoint = circleCenter;
//         switch (side)
//         {
//         case Left:
//             edgePoint.x = minX;
//             // clamp Y to rect vertical span
//             edgePoint.y = clamp(circleCenter.y, minY, maxY);
//             collA.normal = sf::Vector2f(-1.f, 0.f);
//             break;
//         case Right:
//             edgePoint.x = maxX;
//             edgePoint.y = clamp(circleCenter.y, minY, maxY);
//             collA.normal = sf::Vector2f(1.f, 0.f);
//             break;
//         case Top:
//             edgePoint.y = minY;
//             edgePoint.x = clamp(circleCenter.x, minX, maxX);
//             collA.normal = sf::Vector2f(0.f, -1.f);
//             break;
//         case Bottom:
//             edgePoint.y = maxY;
//             edgePoint.x = clamp(circleCenter.x, minX, maxX);
//             collA.normal = sf::Vector2f(0.f, 1.f);
//             break;
//         }
//
//         // distance from center to that nearest edge (in world space)
//         const float outDist = std::sqrt(Helpers::DistanceSquared(edgePoint, circleCenter));
//         // penetration chosen so moving circle along normal by `penetration` will separate it:
//         collA.penetration = radius + outDist;
//         collA.point = edgePoint;
//     }
//
//     // push circle collision
//     collisionInfoVecA.push_back(collA);
//
//     // mirrored collision for the rectangle (invert normal)
//     PhysicsWorld::CollisionInfo collB = collA;
//     collB.normal *= -1.f;
//     collisionInfoVecB.push_back(collB);
// }

void diji::CollisionsHelper::ProcessCircleToBoxCollision(
    const sf::CircleShape& circleA,
    const sf::RectangleShape& rect,
    std::vector<PhysicsWorld::CollisionInfo>& collisionInfoVecA,
    std::vector<PhysicsWorld::CollisionInfo>& collisionInfoVecB)
{
    const float radius = circleA.getRadius();
    const sf::Vector2f circleCenter = circleA.getPosition() + circleA.getOrigin();
    const sf::Vector2f rectSize = rect.getSize();
    const sf::Transform rectTransform = rect.getTransform();
    const sf::Transform invRectTransform = rectTransform.getInverse();
    const sf::Vector2f localCenter = invRectTransform.transformPoint(circleCenter);
    
    // Closest point on AABB in rectangle local space (rect local coords are [0..w] x [0..h])
    auto clamp = [](float v, float lo, float hi) {
        return std::max(lo, std::min(v, hi));
    };
    
    const sf::Vector2f closestLocal{
        clamp(localCenter.x, 0.0f, rectSize.x),
        clamp(localCenter.y, 0.0f, rectSize.y)
    };
    
    // Convert closest local point back to world space
    const sf::Vector2f closestWorld = rectTransform.transformPoint(closestLocal);
    
    // Vector from circle center to the closest point on rectangle
    const sf::Vector2f delta = closestWorld - circleCenter;
    const float distSq = Helpers::DistanceSquared(closestWorld, circleCenter);
    
    // No collision if distance from closest point > radius
    if (distSq > (radius * radius))
        return;
    
    PhysicsWorld::CollisionInfo collisionA;
    collisionA.hasCollision = true;
    
    // If circle center is not inside rectangle (closest point is not the center)
    if (distSq > 0.0f)
    {
        const float distance = std::sqrt(distSq);
        // normal pointing from circle (A) to rectangle (B)
        collisionA.normal = Helpers::Normalize(closestWorld - circleCenter);
        collisionA.penetration = radius - distance;
        collisionA.point = closestWorld; // contact point approx
    }
    else
    {
        // center is inside rectangle (closestLocal == localCenter). Need special handling.
        // Compute distance to each side in local space
        const float dLeft   = localCenter.x - 0.0f;
        const float dRight  = rectSize.x - localCenter.x;
        const float dTop    = localCenter.y - 0.0f;
        const float dBottom = rectSize.y - localCenter.y;
    
        // Find minimum distance to an edge
        float minDist = dLeft;
        enum Side { Left, Right, Top, Bottom } side = Left;
    
        if (dRight < minDist) { minDist = dRight; side = Right; }
        if (dTop < minDist)   { minDist = dTop;   side = Top; }
        if (dBottom < minDist){ minDist = dBottom;side = Bottom; }
    
        // Build a local-space point that lies on the nearest edge (same Y for left/right, same X for top/bottom)
        sf::Vector2f edgeLocal = localCenter;
        switch (side)
        {
        case Left:   edgeLocal.x = 0.0f;                 break;
        case Right:  edgeLocal.x = rectSize.x;           break;
        case Top:    edgeLocal.y = 0.0f;                 break;
        case Bottom: edgeLocal.y = rectSize.y;           break;
        }
    
        const sf::Vector2f edgeWorld = rectTransform.transformPoint(edgeLocal);
        const sf::Vector2f outDelta = edgeWorld - circleCenter;
        const float outDist = std::sqrt(Helpers::DistanceSquared(edgeWorld, circleCenter));
    
        // If outDist == 0 (degenerate), fall back to rectangle axes:
        if (outDist > 0.0f)
        {
            collisionA.normal = Helpers::Normalize(outDelta); // points from circle center to rectangle edge
        }
        else
        {
            // Construct rectangle local axes in world space (xAxis = transform(1,0)-transform(0,0), yAxis likewise)
            const sf::Vector2f xAxis = rectTransform.transformPoint(sf::Vector2f(1.f, 0.f)) - rectTransform.transformPoint(sf::Vector2f(0.f, 0.f));
            const sf::Vector2f yAxis = rectTransform.transformPoint(sf::Vector2f(0.f, 1.f)) - rectTransform.transformPoint(sf::Vector2f(0.f, 0.f));
            // pick axis based on side
            switch (side)
            {
            case Left:  collisionA.normal = -Helpers::Normalize(xAxis); break;
            case Right: collisionA.normal =  Helpers::Normalize(xAxis); break;
            case Top:   collisionA.normal = -Helpers::Normalize(yAxis); break;
            default:    collisionA.normal =  Helpers::Normalize(yAxis); break;
            }
        }
    
        // Penetration: when center is inside rect we choose penetration as radius + distance-to-edge
        // (distance-to-edge = outDist). This yields a positive penetration that will move the circle out.
        collisionA.penetration = radius + outDist;
        collisionA.point = edgeWorld;
    }
    
    // push collision for circle (A)
    collisionInfoVecA.push_back(collisionA);
    
    // produce mirrored collision for rectangle (B) with inverted normal
    PhysicsWorld::CollisionInfo collisionB = collisionA;
    collisionB.normal *= -1.0f;
    collisionInfoVecB.push_back(collisionB);
}

void diji::CollisionsHelper::ProcessBoxToBoxCollision(const sf::RectangleShape& rectA, const sf::RectangleShape& rectB,
    std::vector<PhysicsWorld::CollisionInfo>& collisionInfoVecA,
    std::vector<PhysicsWorld::CollisionInfo>& collisionInfoVecB)
{
    const auto cornersA = GetBoxCorners(rectA);
    const auto cornersB = GetBoxCorners(rectB);
    const auto axesA = GetBoxAxes(cornersA);
    const auto axesB = GetBoxAxes(cornersB);
    
    float minOverlap = std::numeric_limits<float>::max();
    sf::Vector2f smallestAxis;
    
    // Test all axes from both shapes
    for (const auto& axis : axesA)
    {
        float minA, maxA, minB, maxB;
        ProjectOntoAxis(cornersA, axis, minA, maxA);
        ProjectOntoAxis(cornersB, axis, minB, maxB);
    
        if (maxA <= minB || maxB <= minA) // Found separation
            return;

        const float overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < minOverlap)
        {
            minOverlap = overlap;
            smallestAxis = axis;
        }
    }
    
    for (const auto& axis : axesB)
    {
        float minA, maxA, minB, maxB;
        ProjectOntoAxis(cornersA, axis, minA, maxA);
        ProjectOntoAxis(cornersB, axis, minB, maxB);

        if (maxA <= minB || maxB <= minA)
            return;

        const float overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < minOverlap)
        {
            minOverlap = overlap;
            smallestAxis = axis;
        }
    }

    PhysicsWorld::CollisionInfo collision;
    collision.hasCollision = true;
    collision.penetration = minOverlap / Helpers::LengthFast(-smallestAxis);
    
    smallestAxis = Helpers::Normalize(smallestAxis) * -1.0f;
    collision.normal = smallestAxis;
    collision.tangent = sf::Vector2f(-smallestAxis.y, smallestAxis.x);

    collisionInfoVecA.push_back(collision);

    collision.normal *= -1.0f;
    collision.tangent *= -1.0f; // I have no idea if I should invert tangent too (I think so since the normal is opposite you'd think tangent rotates with it)
    collisionInfoVecB.push_back(collision);
}

std::vector<sf::Vector2f> diji::CollisionsHelper::GetBoxCorners(const sf::RectangleShape& rect)
{
    std::vector<sf::Vector2f> corners;
    const sf::Transform& transform = rect.getTransform();

    corners.reserve(rect.getPointCount());
    for (size_t i = 0; i < rect.getPointCount(); ++i)
    {
        corners.emplace_back(transform.transformPoint(rect.getPoint(i)) + rect.getOrigin());
    }

    return corners;
}

std::vector<sf::Vector2f> diji::CollisionsHelper::GetBoxAxes(const std::vector<sf::Vector2f>& corners)
{
    std::vector<sf::Vector2f> axes;
    for (size_t i = 0; i < corners.size(); ++i)
    {
        sf::Vector2f p1 = corners[i];
        sf::Vector2f p2 = corners[(i + 1) % corners.size()];
        const sf::Vector2f edge = p2 - p1;

        sf::Vector2f normal(-edge.y, edge.x);
        axes.push_back(normal);
    }
    return axes;
}

void diji::CollisionsHelper::ProjectOntoAxis(const std::vector<sf::Vector2f>& points, const sf::Vector2f& axis, float& min, float& max)
{
    min = std::numeric_limits<float>::max();
    max = std::numeric_limits<float>::min();
    
    for (size_t i = 1; i < points.size(); ++i)
    {
        const float projection = Helpers::DotProduct(points[i], axis);
        min = std::min(projection, min);
        max = std::max(projection, max);
    }
}
