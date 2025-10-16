#include "PlayerCharacter.h"

#include "Engine/Singleton/SceneManager.h"
#include "Engine/Collision/Collider.h"
#include "../Singletons/GameManager.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Components/Transform.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/RandNumber.h"
#include "Engine/Singleton/TimerManager.h"


const std::vector<int> thomasWasLate::PlayerCharacter::s_StompPointsTable =
{
    100,   // 1st stomp
    200,   // 2nd stomp  
    400,   // 3rd stomp
    500,   // 4th stomp
    800,   // 5th stomp
    1000,  // 6th stomp
    2000,  // 7th stomp
    4000,  // 8th stomp
    5000,  // 9th stomp
    8000   // 10th stomp
    // 11th+ stomps give 1-Up (handled separately)
};

thomasWasLate::PlayerCharacter::PlayerCharacter(diji::GameObject* ownerPtr, const float jumpTime)
    : Component{ ownerPtr }
    , m_MaxJumpTime{ jumpTime }
{
}

void thomasWasLate::PlayerCharacter::Init()
{
    m_CurrentStateUPtr = std::make_unique<IdleState>();
    m_CurrentStateUPtr->OnEnter(GetOwner());

    m_TransformCompPtr = GetOwner()->GetComponent<diji::Transform>();
    m_ColliderCompPtr = GetOwner()->GetComponent<diji::Collider>();
    m_SpriteRenderCompPtr = GetOwner()->GetComponent<diji::SpriteRenderComponent>();

    GameManager::GetInstance().OnNewLevelLoadedEvent.AddListener(this, &PlayerCharacter::OnNewLevelLoaded);

    diji::SceneManager::GetInstance().GetGameObject("A_Camera")->GetComponent<diji::Camera>()->SetFollow(GetOwner());
}

void thomasWasLate::PlayerCharacter::Update()
{
    if (m_IsDead || m_IsPaused) return;

    if (m_IsInvincible)
    {
        InvisibilityFlash();
        CheckEnemyStomp();
    }
    
    if (m_TransformCompPtr->GetPosition().y > 600.f)
    {
        OnFallingInHoleEvent.Broadcast();
        HandleDeathSequence();
    }
    
    CheckIfPlayerIsGrounded();

    if (diji::Helpers::isZero(m_CurrSpeed.y) && m_IsJumping)
    {
        m_IsJumping = false;
        m_JumpTime = m_MaxJumpTime;
        m_MinJumpTime = 0.f;
    }

    // If player stopped sprinting, interpolate back to base speed over 1 second
    if (m_StoppedSprinting)
        DecelerateAfterSprint();
}

void thomasWasLate::PlayerCharacter::FixedUpdate()
{
    if (m_IsDead || m_IsPaused) return;

    if (m_MovementDirection != MovementDirection::None)
    {
        const float multiplier = m_IsOnGround ? 1.f : 0.75f;
        const sf::Vector2f direction = m_MovementDirection == MovementDirection::Right ? sf::Vector2f{ 1, 0 } : sf::Vector2f{ -1, 0 };
        // const float acceleration = diji::Helpers::isZero(m_CurrSpeed.x) ? m_Acceleration * 5.f : m_Acceleration;
        m_ColliderCompPtr->ApplyForce(direction * m_Acceleration * multiplier);
    }

    // speed is currently way to strong of an influence
    // const float multiplier = std::abs(m_CurrSpeed.x) * 0.005f <= 1 ? 1.f : 1 + (std::abs(m_CurrSpeed.x) / m_SprintMaxVelocity.x);
    const float multiplier = 1 + (std::abs(m_CurrSpeed.x) / m_SprintMaxVelocity.x) * 0.75f;
    if (m_IsJumping)
    {
        m_JumpTime += diji::TimeSingleton::GetInstance().GetFixedUpdateDeltaTime();

        if (m_JumpTime < m_MaxJumpTime)
            m_ColliderCompPtr->ApplyForce({ 0.f, -m_JumpForce * 0.5f * multiplier });
    }

    // If player jumped for one frame, ensure they get a consistent minimum jump
    if (m_MinJumpTime > 0.f)
    {
        m_MinJumpTime -= m_TimeSingletonInstance.GetFixedUpdateDeltaTime();
        m_ColliderCompPtr->ApplyForce({ 0.f, -m_JumpForce * 0.5f * multiplier });
    }
}

void thomasWasLate::PlayerCharacter::LateUpdate()
{
    if (m_IsDead || m_IsPaused) return;
    
    const PlayerStates::PlayerState currentState = m_CurrentStateUPtr->GetState();
    // animator controller code
    // todo: EWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
    std::unique_ptr<PlayerStates> newState = nullptr;
    if (m_PowerUpState == PowerUpState::Small)
    {
        if (m_IsOnGround)
        {
            if (diji::Helpers::isZero(m_CurrSpeed.x))
            {
                if (currentState != PlayerStates::PlayerState::Idle)
                    newState = std::make_unique<IdleState>();
            }
            else
            {
                if (m_CurrSpeed.x < 0 && m_MovementDirection == MovementDirection::Right)
                {
                    if (currentState != PlayerStates::PlayerState::Drifting)
                        newState = std::make_unique<DriftingState>();
                }
                else if (m_CurrSpeed.x > 0 && m_MovementDirection == MovementDirection::Left)
                {
                    if (currentState != PlayerStates::PlayerState::Drifting)
                        newState = std::make_unique<DriftingState>();
                }
                else
                {
                    if (std::abs(m_CurrSpeed.x) > m_BaseMaxVelocity.x)
                    {                               
                        if (currentState != PlayerStates::PlayerState::Running)
                            newState = std::make_unique<RunningState>();
                    }
                    else
                    {
                        if (currentState != PlayerStates::PlayerState::Walking)
                            newState = std::make_unique<WalkingState>();
                    }
                }
            }
        }
        else
        {
            if (currentState != PlayerStates::PlayerState::Jumping)
                newState = std::make_unique<JumpingState>();
        }
    }
    else if (currentState != PlayerStates::PlayerState::Growing)
    {
        if (m_IsOnGround)
        {
            if (diji::Helpers::isZero(m_CurrSpeed.x))
            {
                if (currentState != PlayerStates::PlayerState::BigIdle)
                    newState = std::make_unique<BigIdleState>();
            }
            else
            {
                if (m_CurrSpeed.x < 0 && m_MovementDirection == MovementDirection::Right)
                {
                    if (currentState != PlayerStates::PlayerState::BigDrifting)
                        newState = std::make_unique<BigDriftingState>();
                }
                else if (m_CurrSpeed.x > 0 && m_MovementDirection == MovementDirection::Left)
                {
                    if (currentState != PlayerStates::PlayerState::BigDrifting)
                        newState = std::make_unique<BigDriftingState>();
                }
                else
                {
                    if (std::abs(m_CurrSpeed.x) > m_BaseMaxVelocity.x)
                    {                               
                        if (currentState != PlayerStates::PlayerState::BigRunning)
                            newState = std::make_unique<BigRunningState>();
                    }
                    else
                    {
                        if (currentState != PlayerStates::PlayerState::BigWalking)
                            newState = std::make_unique<BigWalkingState>();
                    }
                }
            }
        }
        else
        {
            if (currentState != PlayerStates::PlayerState::BigJumping)
                newState = std::make_unique<BigJumpingState>();
        }
    }

    if (newState)
    {
        m_CurrentStateUPtr = std::move(newState);
        m_CurrentStateUPtr->OnEnter(GetOwner());
    }

    // other
    const bool currLookDirection = m_IsLookingLeft;
    m_IsLookingLeft = m_CurrSpeed.x < 0.f;

    if (m_IsLookingLeft != currLookDirection)
        m_SpriteRenderCompPtr->InvertSprite();
}

void thomasWasLate::PlayerCharacter::OnTriggerEnter(const diji::Collider* other)
{
    if (m_IsDead || m_IsPaused) return;

    if (other->GetTag() == "powerUp")
        HandlePowerUpCollision();
}

void thomasWasLate::PlayerCharacter::OnHitEvent(const diji::Collider* other, const diji::CollisionInfo&)
{
    if (m_IsDead || m_IsPaused || m_IsInvincible) return;
    const std::string& otherTag = other->GetTag();

    if (otherTag == "powerUp")
    {
        HandlePowerUpCollision();
        return;
    }

    if (otherTag == "ground")
    {
        m_BounceScoreMultiplier = 0;
        return;
    }
    
    if (otherTag != "enemy") return;

    const sf::Vector2f playerCenter = m_TransformCompPtr->GetPosition();
    const sf::Vector2f enemyCenter = other->GetPosition();
    
    // Calculate vector from enemy to player
    sf::Vector2f enemyToPlayer = playerCenter - enemyCenter;
    enemyToPlayer = diji::Helpers::Normalize(enemyToPlayer);
    const float dotProduct =  diji::Helpers::DotProduct(enemyToPlayer, UP_VECTOR);
    if (dotProduct > STOMP_THRESHOLD)
    {
        StompEnemy(other);
    }
    else
    {
        OnHitByEnemyEvent.Broadcast();
        if (m_PowerUpState == PowerUpState::Big)
        {
            m_PowerUpState = PowerUpState::Small;
            PlayShrinkAnimation();
        }
        else if (m_PowerUpState == PowerUpState::Small)
            HandleDeathSequence();
    }
}

void thomasWasLate::PlayerCharacter::Move(const sf::Vector2f& direction)
{
    if (m_IsDead || m_IsPaused) return;

    m_MovementDirection = direction.x > 0.f ? MovementDirection::Right : (direction.x < 0.f ? MovementDirection::Left : MovementDirection::None);
}

void thomasWasLate::PlayerCharacter::StopMove()
{
    m_MovementDirection = MovementDirection::None;
}

void thomasWasLate::PlayerCharacter::Jump()
{
    if (m_IsDead || m_IsPaused) return;
    if (!m_IsOnGround || m_IsJumping) return;
    if (!m_CanJump) return;

    m_ColliderCompPtr->ApplyImpulse({ 0.f, -m_JumpForce });
    m_IsOnGround = false;
    m_IsJumping = true;
    m_CanJump = false;
    m_MinJumpTime = m_MaxJumpTime * 0.25f;
}

void thomasWasLate::PlayerCharacter::ClearJump()
{
    m_JumpTime = m_MaxJumpTime;
    m_CanJump = true;
    m_IsJumping = false;
    m_JumpTime = 0.0f;
}

void thomasWasLate::PlayerCharacter::Sprint()
{
    if (!m_IsOnGround || m_IsDead || m_IsPaused) return;
    
    m_Acceleration = m_SprintAcceleration;
    m_ColliderCompPtr->SetMaxVelocity(m_SprintMaxVelocity);
    m_StoppedSprinting = false;
}

void thomasWasLate::PlayerCharacter::StopSprint()
{
    if (m_IsDead) return;
    
    m_Acceleration = m_BaseAcceleration;
    m_StoppedSprinting = true;
    m_SprintDecelerationTimer = 1.f;
}

void thomasWasLate::PlayerCharacter::HandleDeathSequence()
{
    m_IsDead = true;

    auto newState = std::make_unique<DeathState>();
    m_CurrentStateUPtr = std::move(newState);
    m_CurrentStateUPtr->OnEnter(GetOwner());
    m_ColliderCompPtr->SetVelocity(sf::Vector2f{ 0, 0 });
    m_ColliderCompPtr->SetAffectedByGravity(false);

    (void)diji::TimerManager::GetInstance().SetTimer([&]()
    {
        PlayDeathSequence();
    }, 0.25f, false);
}

void thomasWasLate::PlayerCharacter::PlayDeathSequence() const
{
    m_ColliderCompPtr->SetAffectedByGravity(true);
    m_ColliderCompPtr->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);

    m_ColliderCompPtr->ApplyImpulse(sf::Vector2f{ 0, -m_JumpForce });

    (void)diji::TimerManager::GetInstance().SetTimer([&]()
    {
        GameManager::GetInstance().ResetLevel();
    }, 3.41f, false);
}

void thomasWasLate::PlayerCharacter::OnNewLevelLoaded()
{
    m_TransformCompPtr->SetPosition(static_cast<sf::Vector2f>(GameManager::GetInstance().GetStartPosition()));

    m_SpawnPoint = m_TransformCompPtr->GetPosition();
}

std::string thomasWasLate::PlayerCharacter::GetStompPointsAsString(const int bounceMultiplier)
{
    // Clamp to valid range (1-based index)
    const int index = bounceMultiplier - 1;
    
    if (index < 0 || index >= static_cast<int>(s_StompPointsTable.size()))
    {
        OnExtraLifeGainedEvent.Broadcast();
        return "1UP";
    }

    OnPointsScoredEvent.Broadcast(s_StompPointsTable[index]);
    return std::to_string(s_StompPointsTable[index]);
}

void thomasWasLate::PlayerCharacter::DecelerateAfterSprint()
{
    if (m_SprintDecelerationTimer > 0.f)
    {
        // Calculate interpolation factor (alpha) from 0 to 1
        constexpr float maxDecelTime = 1.0f;
        const float t = diji::Helpers::clamp01(m_SprintDecelerationTimer / maxDecelTime);

        // Interpolate from sprint to base velocity
        const sf::Vector2f newVel = diji::Helpers::lerp(m_SprintMaxVelocity, m_BaseMaxVelocity, 1.0f - t);

        m_ColliderCompPtr->SetMaxVelocity(newVel);
        m_SprintDecelerationTimer -= m_TimeSingletonInstance.GetDeltaTime();
    }
    else
    {
        m_StoppedSprinting = false;
        m_ColliderCompPtr->SetMaxVelocity(m_BaseMaxVelocity);
    }
}

void thomasWasLate::PlayerCharacter::CheckIfPlayerIsGrounded()
{
    m_CurrSpeed = m_ColliderCompPtr->GetVelocity();
    
    // if(!diji::Helpers::isZero(m_CurrSpeed.y)) return;

    // Inside your game loop or input handler:
    const float offset = m_PowerUpState == PowerUpState::Small ? 22.f : 44.f;
    const sf::Vector2f origin = m_TransformCompPtr->GetPosition();
    const sf::Vector2f dir = { 0, 1 };
    const sf::Vector2f bottomLeft = { origin.x - 23, origin.y + offset };
    const sf::Vector2f bottomRight = { origin.x + 23, origin.y + offset };
    
    if (const auto hit =  diji::SceneManager::GetInstance().GetPhysicsWorld()->Raycast(bottomLeft, dir, 10.f, m_ColliderCompPtr))
    {
        if (hit->info.hasCollision && GROUND_TAGS.contains(hit->collider->GetTag()))
        {
            m_IsOnGround = true;
            return;
        }
    }

    if (const auto hit =  diji::SceneManager::GetInstance().GetPhysicsWorld()->Raycast(bottomRight, dir, 10.f, m_ColliderCompPtr))
    {
        if (hit->info.hasCollision && GROUND_TAGS.contains(hit->collider->GetTag()))
        {
            m_IsOnGround = true;
            return;
        }
    }
        
    m_IsOnGround = false;
}

void thomasWasLate::PlayerCharacter::PlayGrowthAnimation()
{
    m_IsPaused = true;
    
    // play animation
    std::unique_ptr<PlayerStates> newState = std::make_unique<GrowthAnimationState>();
    m_CurrentStateUPtr = std::move(newState);
    m_CurrentStateUPtr->OnEnter(GetOwner());

    m_ColliderCompPtr->ResizeCollider(sf::Vector2f{ 48, 96 });
    m_TransformCompPtr->SetPosition(m_TransformCompPtr->GetPosition() + sf::Vector2f{ 0.f, -24.f });
    
    (void)diji::TimerManager::GetInstance().SetTimer([&]()
    {
        std::unique_ptr<PlayerStates> newBigState = std::make_unique<BigIdleState>();
        m_CurrentStateUPtr = std::move(newBigState);
        m_CurrentStateUPtr->OnEnter(GetOwner());
    
        m_IsPaused = false;
        OnPoweringUpEvent.Broadcast(false);
    }, 0.78f, false);
}

void thomasWasLate::PlayerCharacter::PlayShrinkAnimation()
{
    m_IsPaused = true;
    OnPoweringUpEvent.Broadcast(true);
    GameManager::GetInstance().SwitchCurrentPlayerState();

    // play animation
    std::unique_ptr<PlayerStates> newState = std::make_unique<ShrinkAnimationState>();
    m_CurrentStateUPtr = std::move(newState);
    m_CurrentStateUPtr->OnEnter(GetOwner());
    
    (void)diji::TimerManager::GetInstance().SetTimer([&]()
    {
        m_SpriteRenderCompPtr->SetStartingFrameX(2);
        m_SpriteRenderCompPtr->SetTotalAnimationFrames(8);
        m_SpriteRenderCompPtr->SetCurrentAnimationFrame(0);
        m_SpriteRenderCompPtr->UpdateFrame();
    }, 0.32f, false);

    (void)diji::TimerManager::GetInstance().SetTimer([&]()
    {
        m_ColliderCompPtr->ResizeCollider(sf::Vector2f{ 48, 48 });
        m_ColliderCompPtr->SetIgnoreAllDynamicColliders(true);
        std::unique_ptr<PlayerStates> newBigState = std::make_unique<IdleState>();
        m_CurrentStateUPtr = std::move(newBigState);
        m_CurrentStateUPtr->OnEnter(GetOwner());

        m_IsInvincible = true;
        m_InvincibilityTimer = 2.2f;
        m_IsPaused = false;
        OnPoweringUpEvent.Broadcast(false);
    }, 1.12f, false);
}

void thomasWasLate::PlayerCharacter::HandlePowerUpCollision()
{
    if (m_PowerUpState == PowerUpState::Small)
    {
        GameManager::GetInstance().SwitchCurrentPlayerState();
        m_PowerUpState = PowerUpState::Big;
        PlayGrowthAnimation();
    }

    GameManager::GetInstance().OnScoreAddedEvent.Broadcast(1000);
    OnPoweringUpEvent.Broadcast(true);
}

void thomasWasLate::PlayerCharacter::InvisibilityFlash()
{
    m_InvincibilityTimer -= m_TimeSingletonInstance.GetDeltaTime();
    m_InvincibilityRenderTimer -= m_TimeSingletonInstance.GetDeltaTime();

    if (m_InvincibilityRenderTimer <= 0.f)
    {
        m_InvincibilityRenderTimer += 0.05f;
        m_SpriteRenderCompPtr->ToggleRendering();
    }
    
    if (m_InvincibilityTimer <= 0.f)
    {
        m_InvincibilityRenderTimer = 0.f;
        m_ColliderCompPtr->SetIgnoreAllDynamicColliders(false);
        m_SpriteRenderCompPtr->EnableRender();
        m_IsInvincible = false;
    }
}

void thomasWasLate::PlayerCharacter::CheckEnemyStomp()
{
    if (m_CurrSpeed.y <= 0.f) return;
    
    const sf::Vector2f origin = m_ColliderCompPtr->GetPosition();
    const sf::Vector2f dir = { 0, 1 };
    constexpr float offset = 23.f;
    const sf::Vector2f TopLeft = { origin.x - offset, origin.y + offset };
    const sf::Vector2f TopRight = { origin.x + offset, origin.y + offset };
    
    auto ValidateEnemyStomp = [&](const diji::Collider* other) -> void
    {
        if (other->GetPosition().y <= m_ColliderCompPtr->GetPosition().y) return;
        
        StompEnemy(other);
    };
    
    if (const auto hit =  diji::SceneManager::GetInstance().GetPhysicsWorld()->Raycast(TopLeft, dir, 10.f, m_ColliderCompPtr))
    {
        if (hit->info.hasCollision && hit->collider->GetTag() == "enemy")
            ValidateEnemyStomp(hit->collider);
    }

    if (const auto hit =  diji::SceneManager::GetInstance().GetPhysicsWorld()->Raycast(TopRight, dir, 10.f, m_ColliderCompPtr))
    {
        if (hit->info.hasCollision && hit->collider->GetTag() == "enemy")
            ValidateEnemyStomp(hit->collider);
    }
}

void thomasWasLate::PlayerCharacter::StompEnemy(const diji::Collider* other)
{
    // I'm capping vertical velocity so max it out to ensure the bounce is same height as normal jump
    m_ColliderCompPtr->ApplyImpulse(sf::Vector2f(0, -m_JumpForce * 2.f));

    // Increment multiplier and get points string
    ++m_BounceScoreMultiplier;
    const std::string& pointsString = GetStompPointsAsString(m_BounceScoreMultiplier);
    OnEnemyStompedEvent.Broadcast(other, pointsString);
    m_ColliderCompPtr->IgnoreCollider(other);
}
