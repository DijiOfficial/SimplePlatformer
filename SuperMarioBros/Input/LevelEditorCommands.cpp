#include "LevelEditorCommands.h"

#include "../Components/LevelEditor/SelectorControls.h"
#include "../Components/LevelEditor/MenuItems/BlockSelector.h"
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

superMarioBros::OpenSaveMenu::OpenSaveMenu(diji::GameObject* actor)
    : GameActorCommands { actor }
{
    m_Selector = actor->GetComponent<SelectorControls>();
}

void superMarioBros::OpenSaveMenu::Execute()
{
    m_Selector->OpenSaveMenu();
}

superMarioBros::OpenBlocksMenu::OpenBlocksMenu(diji::GameObject* actor)
    : GameActorCommands { actor }
{
    m_Selector = actor->GetComponent<SelectorControls>();
}

void superMarioBros::OpenBlocksMenu::Execute()
{
    m_Selector->OpenBlocksMenu();
}

superMarioBros::SelectNextBlock::SelectNextBlock(diji::GameObject* actor, const bool isNext)
    : GameActorCommands { actor }
    , m_IsNext { isNext }
{
    m_Selector = actor->GetComponent<BlockSelector>();
}

void superMarioBros::SelectNextBlock::Execute()
{
    m_Selector->SelectNextOrPreviousBlock(m_IsNext);
}

superMarioBros::CopyBlockHoveredPosition::CopyBlockHoveredPosition(diji::GameObject* actor)
    : GameActorCommands { actor }
{
    m_Selector = actor->GetComponent<BlockSelector>();
}

void superMarioBros::CopyBlockHoveredPosition::Execute()
{
    m_Selector->CopyBlockHoveredPosition();
}
