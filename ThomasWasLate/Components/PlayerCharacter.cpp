#include "PlayerCharacter.h"

#include "Engine/Singleton/SceneManager.h"
#include "Engine/Collision/Collider.h"
#include "../Singletons/GameManager.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/Transform.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/RandNumber.h"
#include "Engine/Singleton/TimerManager.h"

thomasWasLate::PlayerCharacter::PlayerCharacter(diji::GameObject* ownerPtr, const float jumpTime)
    : Component{ ownerPtr }
    , m_MaxJumpTime{ jumpTime }
{
}

void thomasWasLate::PlayerCharacter::Init()
{
    m_TransformCompPtr = GetOwner()->GetComponent<diji::Transform>();
    m_ColliderCompPtr = GetOwner()->GetComponent<diji::Collider>();

    GameManager::GetInstance().OnNewLevelLoadedEvent.AddListener(this, &PlayerCharacter::OnNewLevelLoaded);

    // acceleration due to gravity is independent of mass
    m_Acceleration = sf::Vector2f(0.f, GRAVITY);

    // Angle of impact normalized * mass. (for now it's just mass on Y axis)
    m_Normal = sf::Vector2f(0.f, m_Mass);
    
    (void)diji::TimerManager::GetInstance().SetTimer([&]()
    {
        m_TransformCompPtr->SetPosition(m_SpawnPoint);
        m_LastPosition = m_TransformCompPtr->GetPosition();
        m_NewPosition = m_TransformCompPtr->GetPosition();
        m_Velocity = sf::Vector2f(0.f, 0.f);
        constexpr float forceValue = 1000;  
        m_Velocity = sf::Vector2f(diji::RandNumber::GetRandomVector(-forceValue*2, forceValue*2, -forceValue*0.2, forceValue*0.2));
        // m_NetForce = sf::Vector2f(diji::RandNumber::GetRandomVector(-forceValue, forceValue, -forceValue, forceValue));
    }, 8.f, false);
}

void thomasWasLate::PlayerCharacter::Start()
{
    m_LastPosition = m_TransformCompPtr->GetPosition();
    m_NewPosition = m_TransformCompPtr->GetPosition();

    // temp
    m_Camera = diji::SceneManager::GetInstance().GetGameObject("A_Camera")->GetComponent<diji::Camera>();
}

void thomasWasLate::PlayerCharacter::Update()
{
    m_TransformCompPtr->SetPosition(diji::Helpers::lerp(m_LastPosition, m_NewPosition, m_TimeSingletonInstance.GetFixedTimeAlpha()));
}

void thomasWasLate::PlayerCharacter::FixedUpdate()
{
    // Save position for interpolation
    m_LastPosition = m_NewPosition;

    // Phase 1: Predict movement
    PredictMovement();

    // Phase 2: Detect collisions using predicted positions
    const std::vector<CollisionInfo>& collisionsVec = DetectCollision();
    
    // Phase 3: Resolve collisions and apply friction
    for (const auto& collision : collisionsVec)
    {
        if (!collision.hasCollision)
            continue;
        
        ResolveCollision(collision);
        ApplyFriction(collision);
    }

    // Phase 4: Update final state
    UpdateFinalPosition();
}

void thomasWasLate::PlayerCharacter::Move(const sf::Vector2f& direction) const
{
    (void)direction;
    // diji::Rectf newCollisionBox = m_ColliderCompPtr->GetCollisionBox();
    // newCollisionBox.left += direction.x * m_Speed * m_TimeSingletonInstance.GetDeltaTime();
    //
    // // check for collision with world
    // if (diji::CollisionSingleton::GetInstance().IsCollidingWithWorld(newCollisionBox))
    //     return;
    //
    // // Check collision with others
    // const auto& colliders = diji::CollisionSingleton::GetInstance().IsColliding(m_ColliderCompPtr);
    // for (const auto& collider : colliders)
    // {
    //     if (!collider->GetParent())
    //         continue;
    //     
    //     if (collider->GetParent()->HasComponent<PlayerCharacter>())
    //     {
    //         const auto otherBox = collider->GetCollisionBox();
    //
    //         if (direction.x > 0.f && otherBox.left > newCollisionBox.left)
    //             return;
    //         if (direction.x < 0.f && otherBox.left < newCollisionBox.left)
    //             return;
    //     }
    // }
    //
    // m_TransformCompPtr->AddOffset(direction * m_Speed * m_TimeSingletonInstance.GetDeltaTime());
}

void thomasWasLate::PlayerCharacter::OnNewLevelLoaded()
{
    m_TransformCompPtr->SetPosition(static_cast<sf::Vector2f>(GameManager::GetInstance().GetStartPosition()));

    m_SpawnPoint = m_TransformCompPtr->GetPosition();
}

void thomasWasLate::PlayerCharacter::PredictMovement()
{
    // Apply physics
    const sf::Vector2f finalForce = (m_NetForce / m_Mass) + m_Acceleration;
    m_PredictedVelocity = m_Velocity + finalForce * m_TimeSingletonInstance.GetFixedUpdateDeltaTime();
    m_PredictedPosition = m_NewPosition + m_PredictedVelocity * m_TimeSingletonInstance.GetFixedUpdateDeltaTime();
}

std::vector<thomasWasLate::PlayerCharacter::CollisionInfo> thomasWasLate::PlayerCharacter::DetectCollision() const
{
    // Check for collision with world (temp code for when I actually want to check collisions)
    std::vector<CollisionInfo> collisionsVec;
    
    const auto center = m_Camera->GetCameraView().getCenter();
    const auto size = m_Camera->GetCameraView().getSize();
    const auto col = m_ColliderCompPtr->GetCollisionBox();
    const auto offset = m_ColliderCompPtr->GetOffset();
    // bool bounced = false;

    // Calculate bounds
    const float leftBound = center.x - size.x * 0.5f;
    const float rightBound = center.x + size.x * 0.5f - col.width;
    const float topBound = center.y - size.y * 0.5f;
    const float bottomBound = center.y + size.y * 0.5f - col.height - offset.y;

    CollisionInfo collision;
    collision.hasCollision = false;
  
    // Check X bounds
    if (m_PredictedPosition.x < leftBound)
    {
        collision.hasCollision = true;
        collision.point = sf::Vector2f(leftBound, m_PredictedPosition.y);
        collision.normal = sf::Vector2f(1.0f, 0.0f);  // Normal pointing right
        // collision.penetration = leftBound - m_PredictedPosition.x;
        collision.tangent = sf::Vector2f(0.0f, 1.0f); // Perpendicular to normal
        collisionsVec.push_back(collision);
    }
    else if (m_PredictedPosition.x > rightBound)
    {
        collision.hasCollision = true;
        collision.point = sf::Vector2f(rightBound, m_PredictedPosition.y);
        collision.normal = sf::Vector2f(-1.0f, 0.0f); // Normal pointing left
        // collision.penetration = m_PredictedPosition.x - rightBound;
        collision.tangent = sf::Vector2f(0.0f, 1.0f);
        collisionsVec.push_back(collision);
    }
    
    // Check Y bounds
    if (m_PredictedPosition.y < topBound)
    {
        collision.hasCollision = true;
        collision.point = sf::Vector2f(m_PredictedPosition.x, topBound + col.height + offset.y);
        collision.normal = sf::Vector2f(0.0f, 1.0f);  // Normal pointing down
        // collision.penetration = topBound - m_PredictedPosition.y;
        collision.tangent = sf::Vector2f(1.0f, 0.0f);
        collisionsVec.push_back(collision);
    }
    else if (m_PredictedPosition.y > bottomBound)
    {
        collision.hasCollision = true;
        collision.point = sf::Vector2f(m_PredictedPosition.x, bottomBound);
        collision.normal = sf::Vector2f(0.0f, -1.0f); // Normal pointing up
        // collision.penetration = m_PredictedPosition.y - bottomBound;
        collision.tangent = sf::Vector2f(1.0f, 0.0f);
        collisionsVec.push_back(collision);
    }

    return collisionsVec;
}

void thomasWasLate::PlayerCharacter::ResolveCollision(const CollisionInfo& collision)
{
    // Calculate relative velocity in collision normal direction
    const float velocityAlongNormal = diji::Helpers::DotProduct(m_PredictedVelocity, collision.normal);
    
    // Objects separating? No collision response needed
    if (velocityAlongNormal > 0) return;
    
    // Calculate impulse scalar using coefficient of restitution
    constexpr float restitution = 1.f; // Bouncy collision (0 = no bounce, 1 = perfect bounce) // todo: Handle restitution per object
    const float impulseScalar = -(1 + restitution) * velocityAlongNormal;
    
    // Apply impulse to velocity (since we have infinite mass walls, only affect player) // todo: Handle collision between two dynamic objects
    const sf::Vector2f impulse = impulseScalar * collision.normal;
    m_PredictedVelocity += impulse;
    
    // Store impulse magnitude for friction calculation
    collision.normalImpulse = impulseScalar;

    // Position correction to prevent sinking
    // constexpr float correctionSlop = 0.1f;    // Minimum penetration to correct
    //
    // if (collision.penetration > correctionSlop)
    // {
    //     constexpr float correctionPercent = 0.8f;
    //     
    //     const sf::Vector2f correction = (collision.penetration - correctionSlop) * correctionPercent * collision.normal;
    //     m_PredictedPosition += correction;
    // }
    
    // Instead, just set position to the collision boundary
    m_PredictedPosition = collision.point;
}

// todo: fix vertical bounce friction? air friction?
void thomasWasLate::PlayerCharacter::ApplyFriction(const CollisionInfo& collision)
{
    // if (collision.normalImpulse <= 0) return;
    
    // Calculate tangential (sliding) velocity
    const float tangentialVelocity = diji::Helpers::DotProduct(m_PredictedVelocity, collision.tangent);
    
    // No sliding, no friction needed
    if (std::abs(tangentialVelocity) < 0.001f) return;
    
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
    const float gravityMagnitude = std::abs(GRAVITY);
    const float normalForce = m_Mass * gravityMagnitude;
    
    constexpr float kineticFriction = 0.5f;
    const float frictionMagnitude = kineticFriction * normalForce;
    sf::Vector2f frictionForce = -std::copysign(frictionMagnitude, tangentialVelocity) * collision.tangent;
    
    // Clamp friction so it doesn't reverse velocity
    const float maxFriction = std::abs(tangentialVelocity * m_Mass / m_TimeSingletonInstance.GetFixedUpdateDeltaTime());
    if (frictionMagnitude > maxFriction)
        frictionForce = -tangentialVelocity * collision.tangent * m_Mass / m_TimeSingletonInstance.GetFixedUpdateDeltaTime();
    
    // Apply friction to velocity (F = ma, so a = F/m, v += a*dt)
    m_PredictedVelocity += (frictionForce / m_Mass) * m_TimeSingletonInstance.GetFixedUpdateDeltaTime();
}

void thomasWasLate::PlayerCharacter::UpdateFinalPosition()
{
    // Update final velocity and position with collision-modified values
    m_Velocity = m_PredictedVelocity;
    m_NewPosition = m_PredictedPosition;
    
    // Clear net forces for next frame
    m_NetForce = sf::Vector2f(0.0f, 0.0f);
}

void thomasWasLate::PlayerCharacter::Jump(const bool ignoreTimer)
{
    (void)ignoreTimer;
    // if (!m_IsOnGround)
    //     return;
    //
    // m_IsJumping = true;
    //
    // // Update how long the jump has been going
    // m_JumpTime += m_TimeSingletonInstance.GetDeltaTime();
    //
    // if (m_JumpTime < m_MaxJumpTime || ignoreTimer)
    // {
    //     diji::Rectf newCollisionBox = m_ColliderCompPtr->GetCollisionBox();
    //     newCollisionBox.bottom -= GRAVITY * 3 * m_TimeSingletonInstance.GetDeltaTime();
    //
    //     // check for collision with world
    //     if (diji::CollisionSingleton::GetInstance().IsCollidingWithWorld(newCollisionBox))
    //         return;
    //     
    //     // handle collision with players
    //     const auto& colliders = diji::CollisionSingleton::GetInstance().IsColliding(m_ColliderCompPtr, newCollisionBox);
    //     for (const auto& collider : colliders)
    //     {
    //         if (ignoreTimer)
    //             break;
    //         
    //         // other collision
    //         if (!collider->GetParent())
    //             continue;
    //
    //         // player collision
    //         if (collider->GetParent()->HasComponent<PlayerCharacter>())
    //         {
    //             const auto otherBox = collider->GetCollisionBox();
    //             const float myTopEdge = newCollisionBox.bottom;
    //             const float otherBottomEdge = otherBox.bottom + otherBox.height;
    //             if (myTopEdge <= otherBottomEdge)
    //             {
    //                 m_IsBoosting = true;
    //                 collider->GetParent()->GetComponent<PlayerCharacter>()->Jump(true);
    //                 // return;
    //             }
    //         }
    //     }
    //     
    //     m_TransformCompPtr->AddOffset(0.f, -GRAVITY * 3 * m_TimeSingletonInstance.GetDeltaTime());
    //     return;
    // }
    //
    // CheckForBoosting();
    //
    // m_IsJumping = false;
}

void thomasWasLate::PlayerCharacter::ClearJump()
{
    // m_IsOnGround = false;
    // m_JumpTime = 5.0f;
    // m_IsJumping = false;
    
}

