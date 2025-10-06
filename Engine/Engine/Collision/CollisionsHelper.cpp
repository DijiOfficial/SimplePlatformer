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

void diji::CollisionsHelper::ProcessCircleToBoxCollision(
    const sf::CircleShape& circleA,
    const sf::RectangleShape& rect,
    std::vector<PhysicsWorld::CollisionInfo>& collisionInfoVecA,
    std::vector<PhysicsWorld::CollisionInfo>& collisionInfoVecB)
{
    const auto cornersA = GetBoxCorners(rect);
    const sf::Vector2f closestPoint = FindClosestPointToCircle(circleA, cornersA);
    const auto axes = [&]
    {
        auto result = GetBoxAxes(cornersA);
        result.emplace_back(closestPoint - circleA.getPosition());
        return result;
    }();
    
    float minOverlap = std::numeric_limits<float>::max();
    sf::Vector2f smallestAxis;
    
    for (const auto& axisRaw : axes)
    {
        const sf::Vector2f axis = Helpers::Normalize(axisRaw);
        float minA, maxA, minB, maxB;
        ProjectOntoAxis(cornersA, axis, minA, maxA);
        ProjectCircleOntoAxis(circleA, axis, minB, maxB);

        if (maxA <= minB || maxB <= minA)
            return; // Separation axis found, no collision

        const float overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < minOverlap)
        {
            minOverlap = overlap;
            smallestAxis = axis;
        }
    }

    // Determine direction to push (centerB - centerA along the axis)
    const sf::Vector2f centerDelta = rect.getPosition() - circleA.getPosition();
    if (Helpers::DotProduct(centerDelta, smallestAxis) >= 0.f)
        smallestAxis = -smallestAxis;

    PhysicsWorld::CollisionInfo collision;
    collision.hasCollision = true;
    collision.normal = smallestAxis;
    collision.penetration = minOverlap;
    collision.tangent = sf::Vector2f(-smallestAxis.y, smallestAxis.x);

    collisionInfoVecA.push_back(collision);

    // Flip normal/tangent for B
    collision.normal *= -1.0f;
    collision.tangent *= -1.0f;
    collisionInfoVecB.push_back(collision);
}

void diji::CollisionsHelper::ProcessBoxToBoxCollision(
    const sf::RectangleShape& rectA, const sf::RectangleShape& rectB,
    std::vector<PhysicsWorld::CollisionInfo>& collisionInfoVecA,
    std::vector<PhysicsWorld::CollisionInfo>& collisionInfoVecB)
{
    const auto cornersA = GetBoxCorners(rectA);
    const auto cornersB = GetBoxCorners(rectB);
    const auto axesA = GetBoxAxes(cornersA);
    const auto axesB = GetBoxAxes(cornersB);

    const sf::Vector2f centerA = rectA.getPosition();
    const sf::Vector2f centerB = rectB.getPosition();

    float minOverlap = std::numeric_limits<float>::max();
    sf::Vector2f smallestAxis;

    auto testAxes = [&](const std::vector<sf::Vector2f>& axes)
    {
        for (const auto& axisRaw : axes)
        {
            const sf::Vector2f axis = Helpers::Normalize(axisRaw); // Not ideal but I couldn't get it to work properly without normalizing
            float minA, maxA, minB, maxB;
            ProjectOntoAxis(cornersA, axis, minA, maxA);
            ProjectOntoAxis(cornersB, axis, minB, maxB);

            if (maxA <= minB || maxB <= minA)
                return false; // Separation axis found, no collision

            const float overlap = std::min(maxA, maxB) - std::max(minA, minB);
            if (overlap < minOverlap)
            {
                minOverlap = overlap;
                smallestAxis = axis;
            }
        }
        return true;
    };

    if (!testAxes(axesA)) return;
    if (!testAxes(axesB)) return;

    // Determine direction to push (centerB - centerA along the axis)
    const sf::Vector2f centerDelta = centerB - centerA;
    if (Helpers::DotProduct(centerDelta, smallestAxis) >= 0.f)
        smallestAxis = -smallestAxis;

    PhysicsWorld::CollisionInfo collision;
    collision.hasCollision = true;
    collision.normal = smallestAxis;
    collision.penetration = minOverlap;
    collision.tangent = sf::Vector2f(-smallestAxis.y, smallestAxis.x);

    collisionInfoVecA.push_back(collision);

    // Flip normal/tangent for B
    collision.normal *= -1.0f;
    collision.tangent *= -1.0f;
    collisionInfoVecB.push_back(collision);
}

std::vector<sf::Vector2f> diji::CollisionsHelper::GetBoxCorners(const sf::RectangleShape& rect)
{
    std::vector<sf::Vector2f> corners;
    const sf::Transform& transform = rect.getTransform();

    corners.reserve(rect.getPointCount());
    for (size_t i = 0; i < rect.getPointCount(); ++i)
    {
        corners.emplace_back(transform.transformPoint(rect.getPoint(i)));
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

sf::Vector2f diji::CollisionsHelper::FindClosestPointToCircle(const sf::CircleShape& circle, const std::vector<sf::Vector2f>& corners)
{
    float minDistSquared = std::numeric_limits<float>::max();
    const sf::Vector2f circleCenter = circle.getPosition(); // Again, assuming origin is centered
    sf::Vector2f closestPoint;
    
    for (const auto& corner : corners)
    {
        const float distSquared = Helpers::DistanceSquared(circleCenter, corner);
        if (distSquared < minDistSquared)
        {
            minDistSquared = distSquared;
            closestPoint = corner;
        }
    }

    return closestPoint;
}

void diji::CollisionsHelper::ProjectOntoAxis(const std::vector<sf::Vector2f>& points, const sf::Vector2f& axis, float& min, float& max)
{
    min = max = Helpers::DotProduct(points[0], axis);
    
    for (auto point : points)
    {
        const float projection = Helpers::DotProduct(point, axis);
        min = std::min(projection, min);
        max = std::max(projection, max);
    }
}

void diji::CollisionsHelper::ProjectCircleOntoAxis(const sf::CircleShape& circle, const sf::Vector2f& axis, float& min, float& max)
{
    const sf::Vector2f distanceToCircleEdge = axis * circle.getRadius();

    const sf::Vector2f p1 = circle.getPosition() + distanceToCircleEdge; // origin? I think it's alread centered
    const sf::Vector2f p2 = circle.getPosition() - distanceToCircleEdge;

    min = Helpers::DotProduct(p1, axis);
    max = Helpers::DotProduct(p2, axis);

    if (min > max)
        std::swap(min, max);
}

sf::Vector2f diji::CollisionsHelper::GetCenterOfMass(const std::vector<sf::Vector2f>& points)
{
    float sumX = 0.f;
    float sumY = 0.f;

    for (const auto& p : points)
    {
        sumX += p.x;
        sumY += p.y;
    }

    return { sumX / static_cast<float>(points.size()), sumY / static_cast<float>(points.size()) };
}
