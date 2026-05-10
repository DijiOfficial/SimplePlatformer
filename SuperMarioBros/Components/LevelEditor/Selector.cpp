#include "Selector.h"

#include "../../Singletons/LevelEditorManager.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/Helpers.h"

void superMarioBros::Selector::Init()
{
    LevelEditorManager::GetInstance().Init();

    const sf::Vector2f startPos = LevelEditorManager::GetInstance().m_SelectorPosition;
    if (!diji::Helpers::AreVectorsEqual(startPos, sf::Vector2f{ 0.f, 0.f }))
    {
        GetOwner()->SetObjectPosition(startPos);
    }
}
