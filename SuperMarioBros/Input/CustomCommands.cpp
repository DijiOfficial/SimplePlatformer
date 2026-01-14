#include "CustomCommands.h"
#include "Engine/Singleton/SceneManager.h"
#include "../Components/Player/PlayerCharacter.h"
#include "../Core/GameState.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Interfaces/ISoundSystem.h"
#include "Engine/Singleton/GameStateManager.h"
#include "Engine/Singleton/PauseSingleton.h"
#include "Engine/Singleton/RandNumber.h"

superMarioBros::MoveCharacter::MoveCharacter(diji::GameObject* actor, const sf::Vector2f& direction, const bool isMoving)
    : GameActorCommands { actor }
    , m_Direction { direction }
    , m_IsMoving { isMoving }
{
    m_Character = actor->GetComponent<PlayerCharacter>();
}

void superMarioBros::MoveCharacter::Execute()
{
    if (m_IsMoving)
        m_Character->Move(m_Direction);
    else
        m_Character->StopMove();
}

superMarioBros::CharacterJump::CharacterJump(diji::GameObject* actor, const bool isJumping)
    : GameActorCommands { actor }
    , m_IsJumping { isJumping }
{
    m_Character = actor->GetComponent<PlayerCharacter>();
}

void superMarioBros::CharacterJump::Execute()
{
    if (m_IsJumping)
        m_Character->Jump();
    else
        m_Character->ClearJump();
}

superMarioBros::TempAddImpulse::TempAddImpulse(diji::GameObject* actor)
    : GameActorCommands { actor }
{
    m_Collider = actor->GetComponent<diji::Collider>();
}

void superMarioBros::TempAddImpulse::Execute()
{
    m_Collider->ApplyImpulse({ diji::RandNumber::GetRandomRangeFloat(-m_RandomForce, m_RandomForce), diji::RandNumber::GetRandomRangeFloat(-m_RandomForce, m_RandomForce) });
}

superMarioBros::Sprint::Sprint(diji::GameObject* actor, bool isSprinting)
    : GameActorCommands { actor }
    , m_IsSprinting { isSprinting }
{
    m_Character = actor->GetComponent<PlayerCharacter>();
}

void superMarioBros::Sprint::Execute()
{
    m_IsSprinting ? m_Character->Sprint() : m_Character->StopSprint();
}

superMarioBros::Attack::Attack(diji::GameObject* actor)
    : GameActorCommands { actor }
{
    m_Character = actor->GetComponent<PlayerCharacter>();
}

void superMarioBros::Attack::Execute()
{
    m_Character->Attack();
}

void superMarioBros::StartGame::Execute()
{
    diji::SceneManager::GetInstance().SetNextSceneToActivate(static_cast<int>(superMarioBrosState::LivesDisplayMenu));
}

void superMarioBros::Pause::Execute()
{
    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_pause.wav", false);

    diji::PauseSingleton::GetInstance().TogglePause();
}

superMarioBros::Crouch::Crouch(diji::GameObject* actor, const bool isStart)
    : GameActorCommands { actor }
    , m_IsStart { isStart }
{
    m_Character = actor->GetComponent<PlayerCharacter>();
}

void superMarioBros::Crouch::Execute()
{
    m_Character->Crouch(m_IsStart);
}
