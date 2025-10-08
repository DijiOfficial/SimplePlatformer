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

    diji::SceneManager::GetInstance().GetGameObject("A_Camera")->GetComponent<diji::Camera>()->SetFollow(GetOwner());
}

void thomasWasLate::PlayerCharacter::Update()
{
    m_PreviousSpeed = m_CurrSpeed;
    m_CurrSpeed = m_ColliderCompPtr->GetVelocity();

    m_IsOnGround = diji::Helpers::isZero(m_CurrSpeed.y); // todo: if platforming feels unresponsive, detect grounding with raycasts instead

    if (m_IsJumping && m_IsOnGround)
    {
        m_IsJumping = false;
        m_JumpTime = 0.0f;
    }

    if (m_MinJumpTime > 0.f)
    {
        const float multiplier = std::clamp(std::abs(m_CurrSpeed.x) * 0.005f, 1.f, 1000.f);
        m_MinJumpTime -= m_TimeSingletonInstance.GetDeltaTime();
        m_ColliderCompPtr->ApplyForce({ 0.f, -m_JumpForce * 0.5f * multiplier });
    }

    if (m_StoppedSprinting)
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

}

void thomasWasLate::PlayerCharacter::Move(const sf::Vector2f& direction) const
{
    const float multiplier = m_IsOnGround ? 1.f : 0.75f;
    m_ColliderCompPtr->ApplyForce(direction * m_Acceleration * multiplier);
}

void thomasWasLate::PlayerCharacter::OnNewLevelLoaded()
{
    m_TransformCompPtr->SetPosition(static_cast<sf::Vector2f>(GameManager::GetInstance().GetStartPosition()));

    m_SpawnPoint = m_TransformCompPtr->GetPosition();
}

void thomasWasLate::PlayerCharacter::Jump()
{
    const float multiplier = m_CurrSpeed.x == 0.f ? 1.f : std::abs(m_CurrSpeed.x) * 0.005f;
    
    if (m_IsOnGround)
    {
        m_ColliderCompPtr->ApplyImpulse({ 0.f, -m_JumpForce });
        m_IsOnGround = false;
        m_IsJumping = true;
        m_MinJumpTime = m_MaxJumpTime * 0.25f;
        return;
    }

    if (!m_IsJumping) return;

    m_JumpTime += diji::TimeSingleton::GetInstance().GetDeltaTime();
    
    if (m_JumpTime < m_MaxJumpTime)
    {
        // (void)multiplier;
        m_ColliderCompPtr->ApplyForce({ 0.f, -m_JumpForce * 0.5f * multiplier });
    }
}

void thomasWasLate::PlayerCharacter::ClearJump()
{
    m_JumpTime = 5.0f;
}

void thomasWasLate::PlayerCharacter::Sprint()
{
    if (!m_IsOnGround) return;
    
    m_Acceleration = m_SprintAcceleration;
    m_ColliderCompPtr->SetMaxVelocity(m_SprintMaxVelocity);
    m_StoppedSprinting = false;
}

void thomasWasLate::PlayerCharacter::StopSprint()
{
    m_Acceleration = m_BaseAcceleration;
    m_StoppedSprinting = true;
    m_SprintDecelerationTimer = 1.f;
}

