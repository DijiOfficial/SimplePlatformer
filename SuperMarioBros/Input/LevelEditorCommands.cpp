#include "LevelEditorCommands.h"

#include "../Components/LevelEditor/SelectorMovement.h"
#include "Engine/Core/GameObject.h"

superMarioBros::MoveSelector::MoveSelector(diji::GameObject* actor, const sf::Vector2f& direction, const bool isStart)
    : GameActorCommands { actor }
    , m_Direction { direction }
    , m_IsStart { isStart }
{
    m_Selector = actor->GetComponent<SelectorMovement>();
}

void superMarioBros::MoveSelector::Execute()
{
    m_Selector->Move(m_Direction, m_IsStart);
}
