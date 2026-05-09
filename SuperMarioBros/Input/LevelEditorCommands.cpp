#include "LevelEditorCommands.h"

#include "../Components/LevelEditor/SelectorControls.h"
#include "Engine/Core/GameObject.h"

superMarioBros::MoveSelector::MoveSelector(diji::GameObject* actor, const sf::Vector2f& direction, const bool isStart)
    : GameActorCommands { actor }
    , m_Direction { direction }
    , m_IsStart { isStart }
{
    m_Selector = actor->GetComponent<SelectorControls>();
}

void superMarioBros::MoveSelector::Execute()
{
    m_Selector->Move(m_Direction, m_IsStart);
}

superMarioBros::SelectMenuItem::SelectMenuItem(diji::GameObject* actor)
    : GameActorCommands { actor }
{
    m_Selector = actor->GetComponent<SelectorControls>();
}

void superMarioBros::SelectMenuItem::Execute()
{
    m_Selector->SelectCurrentMenuItem();
}

superMarioBros::ClearSpecialMenu::ClearSpecialMenu(diji::GameObject* actor)
    : GameActorCommands { actor }
{
    m_Selector = actor->GetComponent<SelectorControls>();
}

void superMarioBros::ClearSpecialMenu::Execute()
{
    m_Selector->ClearOutOfSpecialMenu();
}
