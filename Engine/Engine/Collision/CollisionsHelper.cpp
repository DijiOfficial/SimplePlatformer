#include "CollisionsHelper.h"
#include "../Singleton/Helpers.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <algorithm>

diji::PhysicsWorld::CollisionDetectionResult diji::CollisionsHelper::ProcessCircleToCircleCollision(
    const sf::CircleShape& circleA,
    const sf::CircleShape& circleB,
    std::vector<CollisionInfo>& collisionInfoVecA,
    std::vector<CollisionInfo>& collisionInfoVecB,
    const bool isCheckingOverlap)
{
    PhysicsWorld::CollisionDetectionResult result = { .Overlap = false, .Hit = false };
        
    const sf::Vector2f centerA = circleA.getPosition() + circleA.getOrigin();
    const sf::Vector2f centerB = circleB.getPosition() + circleB.getOrigin();
    const float radiusA = circleA.getRadius();
    const float radiusB = circleB.getRadius();

    const float distanceSquared = Helpers::DistanceSquared(centerA, centerB);
    const float radiiSum = radiusA + radiusB;
    const float radiiSumSquared = radiiSum * radiiSum;

    if (distanceSquared >= radiiSumSquared)
        return result;
    
    CollisionInfo collision;
    collision.hasCollision = !isCheckingOverlap;

    // Calculate actual distance and penetration depth
    const float distance = std::sqrt(distanceSquared);
    collision.normal = Helpers::Normalize(centerA - centerB);
    collision.penetration = radiiSum - distance;

    collisionInfoVecA.push_back(collision);
    collision.normal *= -1.f;
    collisionInfoVecB.push_back(collision);

    result.Overlap = isCheckingOverlap;
    result.Hit = !isCheckingOverlap;
    
    return result;
}

diji::PhysicsWorld::CollisionDetectionResult diji::CollisionsHelper::ProcessCircleToBoxCollision(
    const sf::CircleShape& circleA,
    const sf::RectangleShape& rect,
    std::vector<CollisionInfo>& collisionInfoVecA,
    std::vector<CollisionInfo>& collisionInfoVecB,
    const bool isCheckingOverlap)
{
    PhysicsWorld::CollisionDetectionResult collisionResult = { .Overlap = false, .Hit = false };

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
             return collisionResult; // Separation axis found, no collision

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

    CollisionInfo collision;
    collision.hasCollision = !isCheckingOverlap;
    collision.normal = smallestAxis;
    collision.penetration = minOverlap;
    collision.tangent = sf::Vector2f(-smallestAxis.y, smallestAxis.x);

    collisionInfoVecA.push_back(collision);

    // Flip normal/tangent for B
    collision.normal *= -1.0f;
    collision.tangent *= -1.0f;
    collisionInfoVecB.push_back(collision);

    collisionResult.Overlap = isCheckingOverlap;
    collisionResult.Hit = !isCheckingOverlap;
    
    return collisionResult;
}

diji::PhysicsWorld::CollisionDetectionResult diji::CollisionsHelper::ProcessBoxToBoxCollision(
    const sf::RectangleShape& rectA, const sf::RectangleShape& rectB,
    std::vector<CollisionInfo>& collisionInfoVecA,
    std::vector<CollisionInfo>& collisionInfoVecB,
    const bool isCheckingOverlap)
{
    PhysicsWorld::CollisionDetectionResult collisionResult = { .Overlap = false, .Hit = false };

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

    if (!testAxes(axesA))
        return collisionResult;
    if (!testAxes(axesB))
        return collisionResult;

    // Determine direction to push (centerB - centerA along the axis)
    const sf::Vector2f centerDelta = centerB - centerA;
    if (Helpers::DotProduct(centerDelta, smallestAxis) >= 0.f)
        smallestAxis = -smallestAxis;
    
    CollisionInfo collision;
    collision.hasCollision = !isCheckingOverlap;
    collision.normal = smallestAxis;
    collision.penetration = minOverlap;
    collision.tangent = sf::Vector2f(-smallestAxis.y, smallestAxis.x);
    collision.otherAABB = rectB.getGlobalBounds();
    
    collisionInfoVecA.push_back(collision);

    // Flip normal/tangent for B
    collision.normal *= -1.0f;
    collision.tangent *= -1.0f;
    collisionInfoVecB.push_back(collision);
    
    collisionResult.Overlap = isCheckingOverlap;
    collisionResult.Hit = !isCheckingOverlap;
    return collisionResult;
}

void diji::CollisionsHelper::FilterAlignedBoxCollisions(Prediction& pred)
{
    constexpr float moveDeadzone = 0.05f;

    const float xAxis = std::abs(pred.vel.x);
    const float yAxis = std::abs(pred.vel.y);
    const bool preferHorizontal = xAxis > yAxis + moveDeadzone;
    const bool preferVertical = yAxis > xAxis + moveDeadzone;

    std::vector<char> keep(pred.collisionInfoVec.size(), 1);
    for (size_t i = 0; i < pred.collisionInfoVec.size(); ++i)
    {
        if (!pred.collisionInfoVec[i].hasCollision)
        {
            keep[i] = 0;
            continue;
        }

        const sf::FloatRect& rectA = pred.collisionInfoVec[i].otherAABB;
        for (size_t j = i + 1; j < pred.collisionInfoVec.size(); ++j)
        {
            if (!pred.collisionInfoVec[j].hasCollision)
            {
                keep[j] = 0;
                continue;
            }

            const sf::FloatRect& rectB = pred.collisionInfoVec[j].otherAABB;
            if (!BoxesAreAxisAligned(rectA, rectB, Helpers::EPSILON))
                continue;

            const auto& collisionInfoI = pred.collisionInfoVec[i];
            const auto& collisionInfoJ = pred.collisionInfoVec[j];

            const bool iHorizontal = std::abs(collisionInfoI.normal.x) > std::abs(collisionInfoI.normal.y);
            const bool jHorizontal = std::abs(collisionInfoJ.normal.x) > std::abs(collisionInfoJ.normal.y);

            // Prioritize floor collisions
            if (collisionInfoI.normal.y < 0.0f)
            {
                keep[j] = 0;
                continue;
            }
            if (collisionInfoJ.normal.y < 0.0f)
            {
                keep[i] = 0;
                continue;
            }

            if (preferHorizontal)
            {
                if (iHorizontal && !jHorizontal)
                    keep[j] = 0;
                else if (jHorizontal && !iHorizontal)
                    keep[i] = 0;
            }
            else if (preferVertical)
            {
                if (!iHorizontal && jHorizontal)
                    keep[j] = 0;
                else if (!jHorizontal && iHorizontal)
                    keep[i] = 0;
            }
            else
            {
                // Standing still
                if (collisionInfoI.penetration >= collisionInfoJ.penetration)
                    keep[j] = 0;
                else
                    keep[i] = 0;
            }
        }
    }

    std::vector<CollisionInfo> filtered;
    filtered.reserve(pred.collisionInfoVec.size());
    for (size_t k = 0; k < pred.collisionInfoVec.size(); ++k)
        if (keep[k])
            filtered.push_back(pred.collisionInfoVec[k]);

    pred.collisionInfoVec.swap(filtered);
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
        const sf::Vector2f p1 = corners[i];
        const sf::Vector2f p2 = corners[(i + 1) % corners.size()];
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

    const sf::Vector2f p1 = circle.getPosition() + distanceToCircleEdge; // origin? I think it's already centered
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

bool diji::CollisionsHelper::BoxesAreAxisAligned(const sf::FloatRect& rectA, const sf::FloatRect& rectB, const float minOverlap)
{
    const float xOverlap = std::min(rectA.position.x + rectA.size.x, rectB.position.x + rectB.size.x) - std::max(rectA.position.x, rectB.position.x);
    const float yOverlap = std::min(rectA.position.y + rectA.size.y, rectB.position.y + rectB.size.y) - std::max(rectA.position.y, rectB.position.y);

    if (xOverlap <= 0.f || yOverlap <= 0.f) return false;
    return (xOverlap > minOverlap && yOverlap > minOverlap);
}
