#include "CustomCommands.h"
#include "Engine/Singleton/SceneManager.h"
#include "../Components/PlayerCharacter.h"
#include "../Components/tempTest.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Singleton/RandNumber.h"

thomasWasLate::MoveCharacter::MoveCharacter(diji::GameObject* actor, const sf::Vector2f& direction, const bool isMoving)
    : GameActorCommands{ actor }
    , m_Direction{ direction }
    , m_IsMoving{ isMoving }
{
    m_Character = actor->GetComponent<PlayerCharacter>();
}

void thomasWasLate::MoveCharacter::Execute()
{
    if (m_IsMoving)
        m_Character->Move(m_Direction);
    else
        m_Character->StopMove();
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

thomasWasLate::Sprint::Sprint(diji::GameObject* actor, bool isSprinting)
    : GameActorCommands{ actor }
    , m_IsSprinting{ isSprinting }
{
    m_Character = actor->GetComponent<PlayerCharacter>();
}

void thomasWasLate::Sprint::Execute()
{
    m_IsSprinting ? m_Character->Sprint() : m_Character->StopSprint();
}

thomasWasLate::SpawnBox::SpawnBox(diji::GameObject* actor)
    : GameActorCommands{ actor }
{
    huhBoxThing = actor->GetComponent<tempTest>();
}

void thomasWasLate::SpawnBox::Execute()
{
    huhBoxThing->SpawnRandomCollider();
}

thomasWasLate::TrackMouse::TrackMouse(diji::GameObject* actor)
: GameActorCommands{ actor }
{
    huhBoxThing = actor->GetComponent<tempTest>();
}

void thomasWasLate::TrackMouse::Execute(const sf::Vector2i& pos)
{
    huhBoxThing->SetMousePos(pos);
}
