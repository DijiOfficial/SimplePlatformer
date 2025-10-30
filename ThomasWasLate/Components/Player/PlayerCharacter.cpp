#include "PlayerCharacter.h"

#include <array>

#include "../../Helpers/MarioHelpers.h"
#include "../../Interfaces/IKillable.h"
#include "../../Interfaces/IShoveable.h"
#include "../PowerUps/FireBall.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Collision/Collider.h"
#include "../../Singletons/GameManager.h"
#include "../Other/Flag.h"
#include "../Other/PointsBehaviour.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Components/TextComp.h"
#include "Engine/Components/Render.h"
#include "Engine/Components/Transform.h"
#include "Engine/Interfaces/IInterface.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/RandNumber.h"
#include "Engine/Singleton/ResourceManager.h"
#include "Engine/Singleton/TimerManager.h"

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

    m_ColliderCompPtr->SetMaxVelocity(m_BaseMaxVelocity);

    GameManager::GetInstance().OnNewLevelLoadedEvent.AddListener(this, &PlayerCharacter::OnNewLevelLoaded);

    diji::SceneManager::GetInstance().GetMainCamera()->GetComponent<diji::Camera>()->SetFollow(GetOwner());
}

void thomasWasLate::PlayerCharacter::Start()
{
    sf::Shader& starShader = diji::ResourceManager::GetInstance().LoadShader("", "shaders/star.frag");
    m_SpriteRenderCompPtr->SetShader(&starShader);

    if (const auto flag = diji::SceneManager::GetInstance().GetGameObject("E_flag"))
        flag->GetComponent<Flag>()->OnFlagAnimationFinishedEvent.AddListener(this, &PlayerCharacter::StopFlagAnimAndMoveToCastle);

    m_PowerUpState = static_cast<PowerUpState>(GameManager::GetInstance().GetLastPlayerState());
}

#include <SFML/Window/Keyboard.hpp>
void thomasWasLate::PlayerCharacter::Update()
{
    // temp
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::T))
        m_TransformCompPtr->SetPosition(9000, 200);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::R))
    {
        GameManager::GetInstance().AddLife();
        PlayDeathSequence();
    }
    
    
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
        m_ColliderCompPtr->ApplyImpulse(sf::Vector2f{ 0, m_JumpForce * 0.25f });
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

    const float multiplier = 1 + (std::abs(m_CurrSpeed.x) / m_SprintMaxVelocity.x) * 0.25f;
    if (m_IsJumping)
    {
        m_JumpTime += diji::TimeSingleton::GetInstance().GetFixedUpdateDeltaTime();

        if (m_JumpTime < m_MaxJumpTime)
            m_ColliderCompPtr->ApplyForce({ 0.f, -m_JumpForce * 1.5f * multiplier });
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
            if (diji::Helpers::isZero(m_CurrSpeed.x) && !m_IsAgainstCameraEdge)
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

        if (m_PowerUpState == PowerUpState::Fire)
        {
            if (m_IsOnGround)
            {
                if (diji::Helpers::isZero(m_CurrSpeed.x))
                {
                    if (currentState != PlayerStates::PlayerState::BigIdle)
                        newState = std::make_unique<FireIdleState>();
                }
                else
                {
                    if (m_CurrSpeed.x < 0 && m_MovementDirection == MovementDirection::Right)
                    {
                        if (currentState != PlayerStates::PlayerState::BigDrifting)
                            newState = std::make_unique<FireDriftingState>();
                    }
                    else if (m_CurrSpeed.x > 0 && m_MovementDirection == MovementDirection::Left)
                    {
                        if (currentState != PlayerStates::PlayerState::BigDrifting)
                            newState = std::make_unique<FireDriftingState>();
                    }
                    else
                    {
                        if (std::abs(m_CurrSpeed.x) > m_BaseMaxVelocity.x)
                        {                               
                            if (currentState != PlayerStates::PlayerState::BigRunning)
                                newState = std::make_unique<FireRunningState>();
                        }
                        else
                        {
                            if (currentState != PlayerStates::PlayerState::BigWalking)
                                newState = std::make_unique<FireWalkingState>();
                        }
                    }
                }
            }
            else
            {
                if (currentState != PlayerStates::PlayerState::BigJumping)
                    newState = std::make_unique<FireJumpingState>();
            }
        }
        else
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
    }

    if (newState)
    {
        m_CurrentStateUPtr = std::move(newState);
        m_CurrentStateUPtr->OnEnter(GetOwner());
    }
    
    if (!m_IsStartPoweredUp) return;
    UpdateStarPowerShader();
}

void thomasWasLate::PlayerCharacter::OnTriggerEnter(const diji::Collider* other, const diji::CollisionInfo&)
{
    if (m_IsDead || m_IsPaused) return;
    const std::string& otherTag = other->GetTag();
    
    if (otherTag == "powerUp")
        HandlePowerUpCollision();

    if (otherTag == "star")
        HandleStarPickup();

    if (otherTag == "flagPole")
        HandleLevelCompletion(other->GetPosition());

    if (otherTag != "enemy" && otherTag != "koopa") return;
    if (m_IsStartPoweredUp)
    {
        const auto enemyInterface = diji::InterfaceRegistry::GetInterface<IKillable>(other->GetParent());
        enemyInterface->Kill(m_TransformCompPtr->GetPosition().x > other->GetPosition().x);
        return;
    }

    if (otherTag == "koopa")
    {
        const auto enemyInterface = diji::InterfaceRegistry::GetInterface<IShoveable>(other->GetParent());
        enemyInterface->Shove(m_TransformCompPtr->GetPosition().x > other->GetPosition().x);
        
        const std::string& pointsString = mario::MarioHelpers::GetStompPointsAsString(m_BounceScoreMultiplier + 3);
        OnEnemyStompedEvent.Broadcast(other, pointsString);
    }
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

    if (GROUND_TAGS.contains(otherTag))
    {
        m_KoopaStompToggle = false;
        m_BounceScoreMultiplier = 0;
        return;
    }
    
    if (otherTag != "enemy" && otherTag != "koopa") return;
    
    const sf::Vector2f playerCenter = m_TransformCompPtr->GetPosition();
    const sf::Vector2f enemyCenter = other->GetPosition();
    
    // Calculate vector from enemy to player
    sf::Vector2f enemyToPlayer = playerCenter - enemyCenter;
    enemyToPlayer = diji::Helpers::Normalize(enemyToPlayer);
    const float dotProduct =  diji::Helpers::DotProduct(enemyToPlayer, UP_VECTOR);
    if (dotProduct > STOMP_THRESHOLD)
    {
        if (otherTag == "koopa")
            StompKoopa(other);
        else
            StompEnemy(other);
    }
    else
    {
        OnHitByEnemyEvent.Broadcast();
        if (m_PowerUpState == PowerUpState::Big || m_PowerUpState == PowerUpState::Fire)
        {
            m_PowerUpState = PowerUpState::Small;
            PlayShrinkAnimation();
        }
        else
        {
            HandleDeathSequence();
        }
    }
}

void thomasWasLate::PlayerCharacter::Move(const sf::Vector2f& direction)
{
    if (m_IsDead || m_IsPaused) return;

    m_MovementDirection = direction.x > 0.f ? MovementDirection::Right : (direction.x < 0.f ? MovementDirection::Left : MovementDirection::None);

    if (m_MovementDirection != m_LookDirection)
    {
        m_LookDirection = m_MovementDirection;
        m_SpriteRenderCompPtr->InvertSprite();
    }
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

    m_ColliderCompPtr->SetVelocity(sf::Vector2f{ m_ColliderCompPtr->GetVelocity().x, 0.f });
    m_ColliderCompPtr->ApplyImpulse({ 0.f, -m_JumpForce * 0.9f});
    m_IsOnGround = false;
    m_IsJumping = true;
    m_CanJump = false;
    m_MinJumpTime = m_MaxJumpTime * 0.15f;
}

void thomasWasLate::PlayerCharacter::ClearJump()
{
    m_JumpTime = 0;
    m_CanJump = true;
    m_IsJumping = false;
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

void thomasWasLate::PlayerCharacter::Attack()
{
    if (!m_CanAttack) return;
    if (m_IsDead || m_IsPaused) return;
    if (m_PowerUpState != PowerUpState::Fire) return;
    if (!GameManager::GetInstance().CanSpawnFireball()) return;

    // todo: use template instead
    auto fireBall = std::make_unique<diji::GameObject>();
    fireBall->AddComponents<diji::Transform>(300, 500);
    fireBall->AddComponents<diji::SpriteRenderComponent>("graphics/fireBall.png", sf::Vector2i{ 24,24 }, 4, 0.065f);
    fireBall->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 24, 24 });
    fireBall->AddComponents<FireBall>(m_ColliderCompPtr, m_LookDirection != MovementDirection::Left);

    diji::SceneManager::GetInstance().SpawnGameObject("Y_fireBall", std::move(fireBall), m_TransformCompPtr->GetPosition() + sf::Vector2f{ m_LookDirection == MovementDirection::Left ? -30.f : 30.f, -10.f });

    // play animation
    std::unique_ptr<PlayerStates> newState = std::make_unique<ThrowingFireballState>();
    m_CurrentStateUPtr = std::move(newState);
    m_CurrentStateUPtr->OnEnter(GetOwner());

    (void)diji::TimerManager::GetInstance().SetTimer([&]()
    {
        std::unique_ptr<PlayerStates> newBigState = std::make_unique<FireIdleState>();
        m_CurrentStateUPtr = std::move(newBigState);
        m_CurrentStateUPtr->OnEnter(GetOwner());
    }, 0.14f, false);

    (void)diji::TimerManager::GetInstance().SetTimer([&]()
    {
        m_CanAttack = true;
    }, m_AttackFireballCooldownTimer, false);

    GameManager::GetInstance().FireballAdded();
    m_CanAttack = false;
}

void thomasWasLate::PlayerCharacter::SetTransitionState()
{
    m_PowerUpState = static_cast<PowerUpState>(GameManager::GetInstance().GetLastPlayerState());
    std::unique_ptr<PlayerStates> newState;
    if (m_PowerUpState == PowerUpState::Small)
        newState = std::make_unique<RunningState>();
    else if (m_PowerUpState == PowerUpState::Fire)
        newState = std::make_unique<FireRunningState>();
    else
        newState = std::make_unique<BigRunningState>();
    m_CurrentStateUPtr = std::move(newState);
    m_CurrentStateUPtr->OnEnter(GetOwner());
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
        GameManager::GetInstance().LoseLife();
        GameManager::GetInstance().ResetLevel();
        
    }, 3.41f, false);
}

void thomasWasLate::PlayerCharacter::OnNewLevelLoaded()
{
    m_TransformCompPtr->SetPosition(static_cast<sf::Vector2f>(GameManager::GetInstance().GetStartPosition()));

    m_SpawnPoint = m_TransformCompPtr->GetPosition();
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
        m_ColliderCompPtr->SetAffectedByGravity(true);
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

        for(const auto enemyCollider : GameManager::GetInstance().GetEnemyColliders())
            m_ColliderCompPtr->OverlapCollider(enemyCollider);
        
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
    bool skip = false;
    m_ColliderCompPtr->SetAffectedByGravity(false);
    if (m_PowerUpState == PowerUpState::Small)
    {
        m_ColliderCompPtr->SetVelocity(sf::Vector2f{ 0.f, 0.f });
        GameManager::GetInstance().SwitchCurrentPlayerState();
        m_PowerUpState = PowerUpState::Big;
        PlayGrowthAnimation();
    }
    else if (m_PowerUpState == PowerUpState::Big)
    {
        m_ColliderCompPtr->SetVelocity(sf::Vector2f{ 0.f, 0.f });
        m_PowerUpState = PowerUpState::Fire;
        PlayFireTransitionAnimation();
    }
    else
    {
        skip = true;
        m_ColliderCompPtr->SetAffectedByGravity(true);
    }

    GameManager::GetInstance().OnScoreAddedEvent.Broadcast(1000);
    if (skip) return;
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
        m_ColliderCompPtr->ClearAllOverlappedCollider();
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

        if (other->GetTag() == "koopa")
            StompKoopa(other);
        else
            StompEnemy(other);
        // StompEnemy(other);
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
    m_ColliderCompPtr->ApplyImpulse(sf::Vector2f(0, -m_JumpForce));
    
    ++m_BounceScoreMultiplier;
    const std::string& pointsString = mario::MarioHelpers::GetStompPointsAsString(m_BounceScoreMultiplier);
    OnEnemyStompedEvent.Broadcast(other, pointsString);
    
    m_ColliderCompPtr->IgnoreCollider(other);
}

void thomasWasLate::PlayerCharacter::StompKoopa(const diji::Collider* other)
{
    m_ColliderCompPtr->ApplyImpulse(sf::Vector2f(0, -m_JumpForce));

    m_KoopaStompToggle = !m_KoopaStompToggle;

    if (m_KoopaStompToggle)
        ++m_BounceScoreMultiplier;

    const std::string& pointsString = m_KoopaStompToggle ? mario::MarioHelpers::GetStompPointsAsString(m_BounceScoreMultiplier) : "";
    OnEnemyStompedEvent.Broadcast(other, pointsString);
}

void thomasWasLate::PlayerCharacter::PlayFireTransitionAnimation()
{
    m_IsPaused = true;
    
    // play animation
    std::unique_ptr<PlayerStates> newState = std::make_unique<FireAnimationState>();
    m_CurrentStateUPtr = std::move(newState);
    m_CurrentStateUPtr->OnEnter(GetOwner());

    (void)diji::TimerManager::GetInstance().SetTimer([&]()
    {
        std::unique_ptr<PlayerStates> newBigState = std::make_unique<FireIdleState>();
        m_CurrentStateUPtr = std::move(newBigState);
        m_CurrentStateUPtr->OnEnter(GetOwner());
    
        m_IsPaused = false;
        OnPoweringUpEvent.Broadcast(false);
        m_ColliderCompPtr->SetAffectedByGravity(true);
    }, 0.78f, false);
}

void thomasWasLate::PlayerCharacter::HandleStarPickup()
{
    m_IsStartPoweredUp = true;
    m_StarPowerTimer = 0;
    m_SpriteRenderCompPtr->SetRenderWithShader(true);

    for(const auto enemyCollider : GameManager::GetInstance().GetEnemyColliders())
        m_ColliderCompPtr->OverlapCollider(enemyCollider);
}

void thomasWasLate::PlayerCharacter::UpdateStarPowerShader()
{
    m_StarPowerTimer += m_TimeSingletonInstance.GetDeltaTime();
    const auto starShader = m_SpriteRenderCompPtr->GetShader();
    
    starShader->setUniform("texture", sf::Shader::CurrentTexture);
    starShader->setUniform("time", m_StarPowerTimer);
    constexpr float flashSpeed = 16.0f;  // 8 cycles per second
    const int paletteIndex = static_cast<int>(m_StarPowerTimer * flashSpeed) % 4;
    starShader->setUniform("paletteIndex", paletteIndex);

    if (m_StarPowerTimer >= 11.f)
    {
        m_IsStartPoweredUp = false;
        m_SpriteRenderCompPtr->SetRenderWithShader(false);
        m_ColliderCompPtr->ClearAllOverlappedCollider();
    }
}

void thomasWasLate::PlayerCharacter::HandleLevelCompletion(const sf::Vector2f& center)
{
    m_IsPaused = true;
    m_ColliderCompPtr->SetVelocity(sf::Vector2f{ 0, 0});
    m_ColliderCompPtr->SetAffectedByGravity(false);

    m_FlagCenter = center;
    m_TransformCompPtr->SetPosition(center.x - 20.f, m_TransformCompPtr->GetPosition().y);
    std::unique_ptr<PlayerStates> newState;
    if (m_PowerUpState == PowerUpState::Small)
        newState = std::make_unique<FlagPoleSlideState>();
    else if (m_PowerUpState == PowerUpState::Fire)
        newState = std::make_unique<FireFlagPoleSlideState>();
    else
        newState = std::make_unique<BigFlagPoleSlideState>();
    
    m_CurrentStateUPtr = std::move(newState);
    m_CurrentStateUPtr->OnEnter(GetOwner());

    // create timeline for moving down the pole
    m_FlagPoleTimelinePtr = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());
    sf::Vector2f originalPos = m_TransformCompPtr->GetPosition();

    const float groundDistance = m_PowerUpState == PowerUpState::Small ? 450.f : 400.f;
    const float distanceToMove = groundDistance - originalPos.y;
    constexpr float moveDuration = 2.f/15.f / 50.f; // 8frames to move 50 units
    auto &track = m_FlagPoleTimelinePtr->AddFloatTrack("MoveVertically");
    track.keys = { { .time= 0.f, .value= 0.f }, { .time= distanceToMove * moveDuration, .value= distanceToMove } };
    
    track.onValue = [&, originalPos](const float y)
    {
        m_TransformCompPtr->SetPosition(originalPos.x, originalPos.y + y);
    };

    // handle pole score
    const int flagPoleChunk = static_cast<int>(std::floorf(distanceToMove * 0.01f));
    constexpr std::array flagPointTable = { 100, 400, 800, 2000, 5000 };
    const int index = std::clamp(flagPoleChunk, 0, static_cast<int>(flagPointTable.size() - 1));
    const int flagPoints = flagPointTable[index];

    GameManager::GetInstance().OnScoreAddedEvent.Broadcast(flagPoints);
    const std::string pointsString = std::to_string(flagPoints);
    auto pointsText = std::make_unique<diji::GameObject>();
    pointsText->AddComponents<diji::Transform>(sf::Vector2f{ originalPos.x + 65.f, 425.f });
    pointsText->AddComponents<diji::TextComp>(pointsString, "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    pointsText->GetComponent<diji::TextComp>()->GetText().setCharacterSize(18);
    pointsText->AddComponents<diji::Render>();
    pointsText->AddComponents<PointsBehaviour>(true);
    pointsText->GetComponent<PointsBehaviour>()->SetSpeed(-375.f);
    pointsText->GetComponent<PointsBehaviour>()->SetMaxHeight(-75.f);

    diji::SceneManager::GetInstance().SpawnGameObject("ZZ_pointsText", std::move(pointsText), sf::Vector2f{ originalPos.x + 65.f, 425.f });
    
    OnLevelFinishedEvent.Broadcast();
}

void thomasWasLate::PlayerCharacter::StopFlagAnimAndMoveToCastle()
{
    if (m_IsFlagTriggered) return;

    m_IsFlagTriggered = true;
    m_FlagPoleTimelinePtr->Stop();
    m_SpriteRenderCompPtr->InvertSprite();
    m_SpriteRenderCompPtr->Pause();
    m_TransformCompPtr->SetPosition(m_FlagCenter.x + 40, m_TransformCompPtr->GetPosition().y);
    
    (void)diji::TimerManager::GetInstance().SetTimer([&]
    {
        m_SpriteRenderCompPtr->InvertSprite();
        m_ColliderCompPtr->SetAffectedByGravity(true);
        std::unique_ptr<PlayerStates> newState;
        if (m_PowerUpState == PowerUpState::Small)
            newState = std::make_unique<WalkingState>();
        else if (m_PowerUpState == PowerUpState::Fire)
            newState = std::make_unique<FireWalkingState>();
        else
            newState = std::make_unique<BigWalkingState>();
        m_CurrentStateUPtr = std::move(newState);
        m_CurrentStateUPtr->OnEnter(GetOwner());

        // create timeline for moving down the pole
        m_FlagPoleTimelinePtr = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());
        sf::Vector2f originalPos = m_TransformCompPtr->GetPosition();

        auto &track = m_FlagPoleTimelinePtr->AddFloatTrack("MoveVertically");
        track.keys = { { .time= 0.f, .value= 0.f }, { .time= 1.25f, .value= 700 } };
            
        track.onValue = [&, originalPos](const float x)
        {
            m_TransformCompPtr->SetPosition(originalPos.x + x, m_TransformCompPtr->GetPosition().y);
        };

        auto& [eventName, eventKeysVec] = m_FlagPoleTimelinePtr->GetEventTrack("OnAnimationEnd");
        eventKeysVec =
        {
            { .time= 1.25f, .callback= [&]()
                {
                    OnCastleReachedEvent.Broadcast();
                    SetActive(false);
                }
            }
        };
    }, 0.4f, false);
}
