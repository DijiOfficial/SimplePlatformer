#include "CustomCommands.h"
#include "Engine/Singleton/SceneManager.h"
#include "../Components/PlayerCharacter.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Singleton/RandNumber.h"

thomasWasLate::MoveCharacter::MoveCharacter(diji::GameObject* actor, const sf::Vector2f& direction)
    : GameActorCommands{ actor }
    , m_Direction{ direction }
{
    m_Character = actor->GetComponent<PlayerCharacter>();
}

void thomasWasLate::MoveCharacter::Execute()
{
    m_Character->Move(m_Direction);
}

thomasWasLate::CharacterJump::CharacterJump(diji::GameObject* actor, const bool isJumping)
    : GameActorCommands{ actor }
    , m_IsJumping{ isJumping }
{
    m_Character = actor->GetComponent<PlayerCharacter>();
}

void thomasWasLate::CharacterJump::Execute()
{
    if (m_IsJumping)
        m_Character->Jump();
    else
        m_Character->ClearJump();
}

thomasWasLate::TempAddImpulse::TempAddImpulse(diji::GameObject* actor)
    : GameActorCommands{ actor }
{
    m_Collider = actor->GetComponent<diji::Collider>();
}

void thomasWasLate::TempAddImpulse::Execute()
{
    m_Collider->ApplyImpulse({ diji::RandNumber::GetRandomRangeFloat(-m_RandomForce, m_RandomForce), diji::RandNumber::GetRandomRangeFloat(-m_RandomForce, m_RandomForce) });
}

