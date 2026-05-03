#include "CustomCommands.h"
#include "Engine/Singleton/SceneManager.h"
#include "../Components/Player/PlayerInputManager.h"
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
    m_CharacterInput = actor->GetComponent<PlayerInputManager>();
}

void superMarioBros::MoveCharacter::Execute()
{
    if (m_IsMoving)
        m_CharacterInput->Move(m_Direction);
    else
        m_CharacterInput->StopMove();
}

superMarioBros::CharacterJump::CharacterJump(diji::GameObject* actor, const bool isJumping)
    : GameActorCommands { actor }
    , m_IsJumping { isJumping }
{
    m_CharacterInput = actor->GetComponent<PlayerInputManager>();
}

void superMarioBros::CharacterJump::Execute()
{
    if (m_IsJumping)
        m_CharacterInput->Jump();
    else
        m_CharacterInput->ClearJump();
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

superMarioBros::Sprint::Sprint(diji::GameObject* actor, const bool isSprinting)
    : GameActorCommands { actor }
    , m_IsSprinting { isSprinting }
{
    m_CharacterInput = actor->GetComponent<PlayerInputManager>();
}

void superMarioBros::Sprint::Execute()
{
    m_IsSprinting ? m_CharacterInput->Sprint() : m_CharacterInput->StopSprint();
}

superMarioBros::Attack::Attack(diji::GameObject* actor)
    : GameActorCommands { actor }
{
    m_CharacterInput = actor->GetComponent<PlayerInputManager>();
}

void superMarioBros::Attack::Execute()
{
    m_CharacterInput->Attack();
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
    m_CharacterInput = actor->GetComponent<PlayerInputManager>();
}

void superMarioBros::Crouch::Execute()
{
    m_CharacterInput->Crouch(m_IsStart);
}
