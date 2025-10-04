#include "PhysicsWorld.h"

#include <map>
#include <ranges>
#include <stdexcept>

#include "Collider.h"
#include "../Singleton/Helpers.h"

void diji::PhysicsWorld::Reset()
{
    m_DynamicColliders = std::vector<Collider*>();
    m_StaticInfos = std::vector<StaticColliderInfo>();
}

void diji::PhysicsWorld::AddCollider(Collider* collider)
{
    if (!collider)
        throw std::runtime_error("Ya done did fucked up");

    if (collider->IsStatic())
    {
        // Create lightweight static entry
        StaticColliderInfo info;
        info.aabb = collider->GetAABB();
        info.collider = collider;

        // Check for existing static collider?
        
        m_StaticInfos.emplace_back(info);
    }
    else
    {
        if (std::ranges::find(m_DynamicColliders, collider) == m_DynamicColliders.end())
            m_DynamicColliders.emplace_back(collider);
    }
}

void diji::PhysicsWorld::RemoveCollider(Collider* collider)
{
    std::erase(m_DynamicColliders, collider); // yoo can we get big up for c+++ 20 fr?

    // remove any static info with matching owner
    const auto it = std::ranges::find_if(m_StaticInfos,[collider](const StaticColliderInfo& sColInfo){ return sColInfo.collider == collider; });

    if (it != m_StaticInfos.end())
        m_StaticInfos.erase(it);
}

void diji::PhysicsWorld::FixedUpdate()
{
    // Phase 1: Predict movement
    std::vector<Prediction> predictionsVec;
    PredictMovement(predictionsVec);

    // Phase 2: Detect collisions using predicted positions
    DetectCollisions(predictionsVec);

    // Phase 3: Resolve collisions and apply friction
    for (Prediction& prediction : predictionsVec)
    {
        for (const CollisionInfo& collision : prediction.collisionInfoVec)
        {
            if (!collision.hasCollision)
                continue;

            // Apply resolution logic here
            ResolveCollision(prediction, collision);
            ApplyFriction(prediction, collision);
        }
    }

    // Phase 4: Update final state
    for (Prediction& prediction : predictionsVec)
    {
        UpdateFinalPosition(prediction);
    }
}

void diji::PhysicsWorld::PredictMovement(std::vector<Prediction>& predictionsVec) const
{
    const float dt = m_TimeSingletonInstance.GetFixedUpdateDeltaTime();
    predictionsVec.reserve(m_DynamicColliders.size());

    for (auto* collider : m_DynamicColliders)
    {
        if (!collider) continue;
        
        sf::Vector2f forcesApplied = collider->GetNetForce() / collider->GetMass();
        sf::Vector2f vel = collider->GetVelocity();
        if (collider->IsAffectedByGravity())
            forcesApplied += m_Gravity;

        vel += forcesApplied * dt;

        sf::Vector2f proposedPos = collider->GetNewPosition() + vel * dt;
        const auto predictedAabb = collider->GetAABBAt(proposedPos);

        predictionsVec.push_back({ .collider= collider, .AABB= predictedAabb, .pos= proposedPos, .vel= vel, .collisionInfoVec={} });
    }
}

void diji::PhysicsWorld::DetectCollisions(std::vector<Prediction>& predictionsVec)
{
    for (size_t i = 0; i < predictionsVec.size(); ++i)
    {
        auto& [colliderPtr, predictedAABB, pos, vel, collisionsVec] = predictionsVec[i];
        
        // Handle dynamic vs static collisions
        for (const auto& info : m_StaticInfos)
        {
            if (!AABBOverlap(predictedAABB, info.aabb))
                continue;

            // todo: rework the collision handling system
            // colliderPtr->GetShape()->CollideWith(collisionsVec, info, pos);
            // this is not working?
            // colliderPtr->GetShape()->CollideWith(collisionsVec, predictedAABB, info.aabb);

            // This only works for non-rotated rectangles (AABB)
            const float leftA   = predictedAABB.left;
            const float rightA  = PhysicsWorld::Right(predictedAABB);
            const float topA    = predictedAABB.top;
            const float bottomA = PhysicsWorld::Bottom(predictedAABB);

            const float leftB   = info.aabb.left;
            const float rightB  = PhysicsWorld::Right(info.aabb);
            const float topB    = info.aabb.top;
            const float bottomB = PhysicsWorld::Bottom(info.aabb);
            
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

        for (size_t j = 0; j < predictionsVec.size(); ++j)
        {
            if (i == j) continue;
            const Prediction& other = predictionsVec[j];

            if (!AABBOverlap(predictedAABB, other.AABB))
                continue;
            
            // compute dynamic-dynamic collision, push to both dyn and otherDyn
            
            // // notify collision callback(s)
            // colliderPtr->OnCollision(other.collider);
            // other.collider->OnCollision(colliderPtr);
        }
    }
}

void diji::PhysicsWorld::ResolveCollision(Prediction& prediction, const CollisionInfo& collision)
{
    // Calculate relative velocity in collision normal direction
    const float velocityAlongNormal = Helpers::DotProduct(prediction.vel, collision.normal);
    
    // Objects separating? No collision response needed
    if (velocityAlongNormal > 0) return;
    
    // Calculate impulse scalar using coefficient of restitution
    const float restitution = prediction.collider->GetRestitution();
    const float impulseScalar = -(1 + restitution) * velocityAlongNormal;
    
    // Apply impulse to velocity (since we have infinite mass walls, only affect player)
    const sf::Vector2f impulse = impulseScalar * collision.normal;
    prediction.vel += impulse;
    
    // Store impulse magnitude for friction calculation
    collision.normalImpulse = impulseScalar;

    // Position correction to prevent sinking
    constexpr float correctionSlop = 0.1f;    // Minimum penetration to correct
    if (collision.penetration > correctionSlop)
    {
        constexpr float correctionPercent = 0.8f;
        
        const sf::Vector2f correction = (collision.penetration - correctionSlop) * correctionPercent * collision.normal;
        prediction.pos += correction;
    }
    
    // Instead, just set position to the collision boundary
    // prediction.pos = collision.point;
}

void diji::PhysicsWorld::ApplyFriction(Prediction& prediction, const CollisionInfo& collision) const
{
    // if (collision.normalImpulse <= 0) return;
    
    // Calculate tangential (sliding) velocity
    const float tangentialVelocity = Helpers::DotProduct(prediction.vel, collision.tangent);
    
    // No sliding, no friction needed
    if (std::abs(tangentialVelocity) < 0.001f) return;

    const float mass = prediction.collider->GetMass();
    
    // // Calculate friction coefficients
    // constexpr float staticFriction = 0.7f;   // Prevents sliding from starting
    // // Calculate maximum friction force (Coulomb friction model)
    // const float maxFrictionForce = staticFriction * collision.normalImpulse;
    // // Calculate required force to stop sliding completely
    // const float stoppingForce = std::abs(tangentialVelocity) * m_Mass / m_TimeSingletonInstance.GetFixedUpdateDeltaTime();
    // sf::Vector2f frictionForce;
    // if (stoppingForce <= maxFrictionForce)
    // {
    //     // Static friction: stop sliding completely
    //     frictionForce = -tangentialVelocity * collision.tangent * m_Mass / m_TimeSingletonInstance.GetFixedUpdateDeltaTime();
    // }
    // else
    // {
    //     constexpr float kineticFriction = 0.5f;
    //     // Kinetic friction: reduce sliding velocity
    //     const float kineticFrictionForce = kineticFriction * collision.normalImpulse;
    //     frictionForce = -std::copysign(kineticFrictionForce, tangentialVelocity) * collision.tangent;
    // }


    // Use normal force (mass * gravity) for friction
    const float gravityMagnitude = std::abs(m_Gravity.y);
    const float normalForce = mass * gravityMagnitude;
    
    constexpr float kineticFriction = 0.5f;
    const float frictionMagnitude = kineticFriction * normalForce;
    sf::Vector2f frictionForce = -std::copysign(frictionMagnitude, tangentialVelocity) * collision.tangent;
    
    // Clamp friction so it doesn't reverse velocity
    const float maxFriction = std::abs(tangentialVelocity * mass / m_TimeSingletonInstance.GetFixedUpdateDeltaTime());
    if (frictionMagnitude > maxFriction)
        frictionForce = -tangentialVelocity * collision.tangent * mass / m_TimeSingletonInstance.GetFixedUpdateDeltaTime();
    
    // Apply friction to velocity (F = ma, so a = F/m, v += a*dt)
    prediction.vel += (frictionForce / mass) * m_TimeSingletonInstance.GetFixedUpdateDeltaTime();
}

void diji::PhysicsWorld::UpdateFinalPosition(const Prediction& prediction)
{
    // Update final velocity and position with collision-modified values
    prediction.collider->SetVelocity(prediction.vel);
    prediction.collider->SetNewPosition(prediction.pos);

    // Clear net forces for next frame
    prediction.collider->ClearNetForce();
}
