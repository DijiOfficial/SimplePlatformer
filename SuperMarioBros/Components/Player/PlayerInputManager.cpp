#include "PlayerInputManager.h"

#include "PlayerCharacter.h"
#include "PlayerInformation.h"
#include "PlayerStates.h"
#include "../../Singletons/GameManager.h"
#include "../PowerUps/FireBall.h"

#include "Engine/Collision/Collider.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Interfaces/ISoundSystem.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Singleton/TimerManager.h"

superMarioBros::PlayerInputManager::PlayerInputManager(diji::GameObject* ownerPtr, const float jumpTime)
    : Component{ ownerPtr }
    , m_MaxJumpTime{ jumpTime }
{
}

void superMarioBros::PlayerInputManager::Init()
{
    m_SpriteRenderCompPtr = GetOwner()->GetComponent<diji::SpriteRenderComponent>();
    m_PlayerCharacterCompPtr = GetOwner()->GetComponent<PlayerCharacter>();
    m_ColliderCompPtr = GetOwner()->GetComponent<diji::Collider>();
    m_TransformCompPtr = GetOwner()->GetRootComponent();
    
    m_ColliderCompPtr->SetMaxVelocity(m_BaseMaxVelocity);
}

void superMarioBros::PlayerInputManager::Update()
{
    CheckIfPlayerIsGrounded();

    if (m_StoppedSprinting)
        DecelerateAfterSprint();
}

void superMarioBros::PlayerInputManager::FixedUpdate()
{
    if (m_PlayerCharacterCompPtr->IsDeadOrPaused()) return;

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

void superMarioBros::PlayerInputManager::LateUpdate()
{
    if (m_PlayerCharacterCompPtr->IsDeadOrPaused()) return;
    
    const PlayerStates::PlayerState currentState = m_PlayerCharacterCompPtr->GetCurrentState();
    // animator controller code
    // todo: EWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
    if (m_PlayerCharacterCompPtr->IsSmallMario())
    {
        if (m_IsOnGround)
        {
            if (diji::Helpers::isZero(m_CurrSpeed.x) && !m_IsAgainstCameraEdge)
            {
                if (currentState != PlayerStates::PlayerState::Idle)
                    m_PlayerCharacterCompPtr->ChangeState<IdleState>();
            }
            else
            {
                if (m_CurrSpeed.x < 0 && m_MovementDirection == MovementDirection::Right)
                {
                    if (currentState != PlayerStates::PlayerState::Drifting)
                        m_PlayerCharacterCompPtr->ChangeState<DriftingState>();
                }
                else if (m_CurrSpeed.x > 0 && m_MovementDirection == MovementDirection::Left)
                {
                    if (currentState != PlayerStates::PlayerState::Drifting)
                        m_PlayerCharacterCompPtr->ChangeState<DriftingState>();
                }
                else
                {
                    if (std::abs(m_CurrSpeed.x) > m_BaseMaxVelocity.x)
                    {                               
                        if (currentState != PlayerStates::PlayerState::Running)
                            m_PlayerCharacterCompPtr->ChangeState<RunningState>();
                    }
                    else
                    {
                        if (currentState != PlayerStates::PlayerState::Walking)
                            m_PlayerCharacterCompPtr->ChangeState<WalkingState>();
                    }
                }
            }
        }
        else
        {
            if (currentState != PlayerStates::PlayerState::Jumping)
                m_PlayerCharacterCompPtr->ChangeState<JumpingState>();
        }
    }
    else if (currentState != PlayerStates::PlayerState::Growing)
    {

        if (m_PlayerCharacterCompPtr->IsFireMario())
        {
            if (m_IsCrouched)
            {
                m_PlayerCharacterCompPtr->ChangeState<FireCrouchingState>();
            }
            else if (m_IsOnGround)
            {
                if (diji::Helpers::isZero(m_CurrSpeed.x))
                {
                    if (currentState != PlayerStates::PlayerState::BigIdle)
                        m_PlayerCharacterCompPtr->ChangeState<FireIdleState>();
                }
                else
                {
                    if (m_CurrSpeed.x < 0 && m_MovementDirection == MovementDirection::Right)
                    {
                        if (currentState != PlayerStates::PlayerState::BigDrifting)
                            m_PlayerCharacterCompPtr->ChangeState<FireDriftingState>();
                    }
                    else if (m_CurrSpeed.x > 0 && m_MovementDirection == MovementDirection::Left)
                    {
                        if (currentState != PlayerStates::PlayerState::BigDrifting)
                            m_PlayerCharacterCompPtr->ChangeState<FireDriftingState>();
                    }
                    else
                    {
                        if (std::abs(m_CurrSpeed.x) > m_BaseMaxVelocity.x)
                        {                               
                            if (currentState != PlayerStates::PlayerState::BigRunning)
                                m_PlayerCharacterCompPtr->ChangeState<FireRunningState>();
                        }
                        else
                        {
                            if (currentState != PlayerStates::PlayerState::BigWalking)
                                m_PlayerCharacterCompPtr->ChangeState<FireWalkingState>();
                        }
                    }
                }
            }
            else
            {
                if (currentState != PlayerStates::PlayerState::BigJumping)
                    m_PlayerCharacterCompPtr->ChangeState<FireJumpingState>();
            }
        }
        else
        {
            if (m_IsCrouched)
            {
                m_PlayerCharacterCompPtr->ChangeState<CrouchingState>();
            }
            else if (m_IsOnGround)
            {
                if (diji::Helpers::isZero(m_CurrSpeed.x))
                {
                    if (currentState != PlayerStates::PlayerState::BigIdle)
                        m_PlayerCharacterCompPtr->ChangeState<BigIdleState>();
                }
                else
                {
                    if (m_CurrSpeed.x < 0 && m_MovementDirection == MovementDirection::Right)
                    {
                        if (currentState != PlayerStates::PlayerState::BigDrifting)
                            m_PlayerCharacterCompPtr->ChangeState<BigDriftingState>();
                    }
                    else if (m_CurrSpeed.x > 0 && m_MovementDirection == MovementDirection::Left)
                    {
                        if (currentState != PlayerStates::PlayerState::BigDrifting)
                            m_PlayerCharacterCompPtr->ChangeState<BigDriftingState>();
                    }
                    else
                    {
                        if (std::abs(m_CurrSpeed.x) > m_BaseMaxVelocity.x)
                        {                               
                            if (currentState != PlayerStates::PlayerState::BigRunning)
                                m_PlayerCharacterCompPtr->ChangeState<BigRunningState>();
                        }
                        else
                        {
                            if (currentState != PlayerStates::PlayerState::BigWalking)
                                m_PlayerCharacterCompPtr->ChangeState<BigWalkingState>();
                        }
                    }
                }
            }
            else
            {
                if (currentState != PlayerStates::PlayerState::BigJumping)
                    m_PlayerCharacterCompPtr->ChangeState<BigJumpingState>();
            }
        }
    }
}

void superMarioBros::PlayerInputManager::Move(const sf::Vector2f& direction)
{
    if (m_PlayerCharacterCompPtr->IsDeadOrPaused() || m_IsCrouched) return;

    m_MovementDirection = direction.x > 0.f ? MovementDirection::Right : (direction.x < 0.f ? MovementDirection::Left : MovementDirection::None);

    if (m_MovementDirection != m_LookDirection)
    {
        m_LookDirection = m_MovementDirection;
        m_SpriteRenderCompPtr->InvertSprite();
    }
}

void superMarioBros::PlayerInputManager::StopMove()
{
    m_MovementDirection = MovementDirection::None;
}

void superMarioBros::PlayerInputManager::Jump()
{
    if (m_PlayerCharacterCompPtr->IsDeadOrPaused()) return;
    if (!m_IsOnGround || m_IsJumping) return;
    if (!m_CanJump) return;
    if (m_IsCrouched) return;

    m_ColliderCompPtr->SetVelocity(sf::Vector2f{ m_ColliderCompPtr->GetVelocity().x, 0.f });
    m_ColliderCompPtr->ApplyImpulse({ 0.f, -m_JumpForce * 0.9f});
    m_IsOnGround = false;
    m_IsJumping = true;
    m_CanJump = false;
    m_MinJumpTime = m_MaxJumpTime * 0.15f;
    
    const std::string sound = m_PlayerCharacterCompPtr->IsSmallMario() ? "sound/smb_jump-small.wav" : "sound/smb_jump-super.wav";
    diji::ServiceLocator::GetSoundSystem().AddSoundRequest(sound, false);
}

void superMarioBros::PlayerInputManager::ClearJump()
{
    m_JumpTime = 0;
    m_CanJump = true;
    m_IsJumping = false;
}

void superMarioBros::PlayerInputManager::Sprint()
{
    if (!m_IsOnGround || m_PlayerCharacterCompPtr->IsDeadOrPaused()) return;
    
    m_Acceleration = m_SprintAcceleration;
    m_ColliderCompPtr->SetMaxVelocity(m_SprintMaxVelocity);
    m_StoppedSprinting = false;
}

void superMarioBros::PlayerInputManager::StopSprint()
{
    if (m_PlayerCharacterCompPtr->IsDead()) return;
    
    m_Acceleration = m_BaseAcceleration;
    m_StoppedSprinting = true;
    m_SprintDecelerationTimer = 1.f;
}

void superMarioBros::PlayerInputManager::Attack()
{
    if (!m_CanAttack) return;
    if (m_PlayerCharacterCompPtr->IsDeadOrPaused()) return;
    if (!m_PlayerCharacterCompPtr->IsFireMario()) return;
    if (!GameManager::GetInstance().CanSpawnFireball()) return;

    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_fireball.wav", false);

    // todo: use template instead
    auto fireBall = std::make_unique<diji::GameObject>();
    fireBall->SetObjectPosition({300, 500 });
    fireBall->AddComponent<diji::SpriteRenderComponent>("graphics/fireBall.png", sf::Vector2i{ 24,24 }, 4, 0.065f);
    fireBall->AddComponent<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 24, 24 });
    fireBall->AddComponent<FireBall>(m_ColliderCompPtr, m_LookDirection != MovementDirection::Left);

    diji::SceneManager::GetInstance().SpawnGameObject("Y_fireBall", std::move(fireBall), m_TransformCompPtr->GetWorldPosition() + sf::Vector2f{ m_LookDirection == MovementDirection::Left ? -30.f : 30.f, -10.f });
    m_PlayerCharacterCompPtr->ChangeState<ThrowingFireballState>();

    (void)diji::TimerManager::GetInstance().SetTimer([&]()
    {
        m_PlayerCharacterCompPtr->ChangeState<FireIdleState>();
    }, 0.14f, false);

    (void)diji::TimerManager::GetInstance().SetTimer([&]()
    {
        m_CanAttack = true;
    }, m_AttackFireballCooldownTimer, false);

    GameManager::GetInstance().FireballAdded();
    m_CanAttack = false;
}

void superMarioBros::PlayerInputManager::Crouch(const bool isStart)
{
    if (!isStart)
    {
        m_IsCrouched = false;
        return;
    }
    m_IsCrouched = true;

    StopMove();
    
    if (!m_StoppedSprinting)
        StopSprint();
}

void superMarioBros::PlayerInputManager::Bump()
{
    m_IsJumping = false;
    m_JumpTime = m_MaxJumpTime;
    m_MinJumpTime = 0.f;
    m_ColliderCompPtr->SetVelocity(sf::Vector2f{ m_ColliderCompPtr->GetVelocity().x, 0.f });
    m_ColliderCompPtr->ApplyImpulse(sf::Vector2f{ 0, m_JumpForce * 0.10f });
}

void superMarioBros::PlayerInputManager::DecelerateAfterSprint()
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

void superMarioBros::PlayerInputManager::CheckIfPlayerIsGrounded()
{
    m_CurrSpeed = m_ColliderCompPtr->GetVelocity();
    
    // if(!diji::Helpers::isZero(m_CurrSpeed.y)) return;

    const float offset = m_PlayerCharacterCompPtr->IsSmallMario() ? 22.f : 44.f;
    const sf::Vector2f origin = m_TransformCompPtr->GetWorldPosition();
    constexpr sf::Vector2f dir = { 0, 1 };
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