#include "BonusArea.h"

#include "../../../Input/CustomCommands.h"
#include "../../../Singletons/GameManager.h"
#include "../../../Helpers/WorldBuilder.h"
#include "../../Player/PlayerInputManager.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/GameObject.h"

#include <cmath>
#include "Engine/Collision/Collider.h"
#include "Engine/Input/InputManager.h"

void superMarioBros::BonusArea::Start()
{
    switch (m_BonusAreaEntranceDirection)
    {
    case diji::Directions::Down:
        BindCommand<TriggerBonusArea>(diji::PlayerIdx::KEYBOARD, diji::KeyState::PRESSED, sf::Keyboard::Scancode::S);
        BindCommand<TriggerBonusArea>(diji::PlayerIdx::KEYBOARD, diji::KeyState::PRESSED, sf::Keyboard::Scancode::Down);
        BindCommand<TriggerBonusArea>(diji::PlayerIdx::PLAYER1, diji::KeyState::PRESSED, diji::Controller::Button::DPadDown);
        break;
    case diji::Directions::Right:
        BindCommand<TriggerBonusArea>(diji::PlayerIdx::KEYBOARD, diji::KeyState::HELD, sf::Keyboard::Scancode::D);
        BindCommand<TriggerBonusArea>(diji::PlayerIdx::KEYBOARD, diji::KeyState::HELD, sf::Keyboard::Scancode::Right);
        BindCommand<TriggerBonusArea>(diji::PlayerIdx::PLAYER1, diji::KeyState::HELD, diji::Controller::Button::DPadRight);
        break;
    case diji::Directions::Left:
        BindCommand<TriggerBonusArea>(diji::PlayerIdx::KEYBOARD, diji::KeyState::HELD, sf::Keyboard::Scancode::A);
        BindCommand<TriggerBonusArea>(diji::PlayerIdx::KEYBOARD, diji::KeyState::HELD, sf::Keyboard::Scancode::Left);
        BindCommand<TriggerBonusArea>(diji::PlayerIdx::PLAYER1, diji::KeyState::HELD, diji::Controller::Button::DPadLeft);
        break;
    case diji::Directions::Up:
        BindCommand<TriggerBonusArea>(diji::PlayerIdx::KEYBOARD, diji::KeyState::PRESSED, sf::Keyboard::Scancode::W);
        BindCommand<TriggerBonusArea>(diji::PlayerIdx::KEYBOARD, diji::KeyState::PRESSED, sf::Keyboard::Scancode::Up);
        BindCommand<TriggerBonusArea>(diji::PlayerIdx::PLAYER1, diji::KeyState::PRESSED, diji::Controller::Button::DPadUp);
        break;
    case diji::Directions::None:
        break;
    }
}

void superMarioBros::BonusArea::OnTriggerEnter(const diji::Collider* collider, const diji::CollisionInfo&)
{
    m_PlayerIsOnTarget = collider->GetTag() == "player";

    if (m_PlayerIsOnTarget)
        m_PlayerColliderPtr = collider;
}

void superMarioBros::BonusArea::OnTriggerExit(const diji::Collider* collider, const diji::CollisionInfo& )
{
    if (collider->GetTag() == "player")
        m_PlayerIsOnTarget = false;
}

void superMarioBros::BonusArea::TryTriggerBonusArea()
{
    if (m_IsAlreadyTriggered || m_PlayerIsOnTarget == false)
        return;

    if (IsPlayerAlignedWithPipe() == false)
        return;

    GameManager::GetInstance().PrepareLoadBonusArea(m_BonusAreaEntranceDirection);
    m_IsAlreadyTriggered = true;
}

bool superMarioBros::BonusArea::IsPlayerAlignedWithPipe() const
{
    const sf::Vector2f pipeCenter = GetOwner()->GetRootComponent()->GetWorldPosition();

    if (m_BonusAreaEntranceDirection == diji::Directions::Left || m_BonusAreaEntranceDirection == diji::Directions::Right)
    {
        const sf::FloatRect playerBounds = m_PlayerColliderPtr->GetAABB();
        const float playerBottom = playerBounds.position.y + playerBounds.size.y;

        return playerBottom > pipeCenter.y && m_PlayerColliderPtr->GetOwner()->GetComponent<PlayerInputManager>()->IsOnGround();
    }

    constexpr float maxEntryOffset = WorldBuilder::WorldSettings::TILE_SIZE * 0.4f;

    return std::abs(m_PlayerColliderPtr->GetPosition().x - pipeCenter.x) <= maxEntryOffset;
}
