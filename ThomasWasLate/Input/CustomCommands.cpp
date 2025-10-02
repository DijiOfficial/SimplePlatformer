#include "CustomCommands.h"
#include "Engine/Singleton/SceneManager.h"
#include "../Components/PlayerCharacter.h"

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

