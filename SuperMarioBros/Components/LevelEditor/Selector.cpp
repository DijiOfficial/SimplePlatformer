#include "Selector.h"

#include "../../Singletons/GameManager.h"
#include "../../Singletons/LevelEditorManager.h"
#include "../Backgrounds/BackgroundHandler.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/SceneManager.h"

void superMarioBros::Selector::Init()
{
    LevelEditorManager::GetInstance().Init();

    const sf::Vector2f startPos = LevelEditorManager::GetInstance().m_SelectorPosition;
    if (!diji::Helpers::AreVectorsEqual(startPos, sf::Vector2f{ 0.f, 0.f }))
    {
        GetOwner()->SetObjectPosition(startPos);
    }

    CreateBackgroundTexture();
}

void superMarioBros::Selector::Start()
{
    m_TransformCompPtr = GetOwner()->GetRootComponent();
}

void superMarioBros::Selector::SetFramePosition(const int frameX, const int frameY)
{
    m_SpriteRenderComp->SetStartingFrame(frameX, frameY);
    m_SpriteRenderComp->UpdateFrame();
    m_CurrentFramePos.x = frameX;
    m_CurrentFramePos.y = frameY;
}

void superMarioBros::Selector::ActivateBackgroundTexture() const
{
    m_TextureGO->SetActive(true);
}

void superMarioBros::Selector::DeactivateBackgroundTexture() const
{
    m_TextureGO->SetActive(false);
}

void superMarioBros::Selector::TryPlaceItem()
{
    const sf::Vector2f& pos = m_TransformCompPtr->GetWorldPosition();
    const char itemChar = m_AtlasToPosMap[m_CurrentFramePos];
    const int row = static_cast<int>(pos.y - 25.f) / 50;
    const int col = static_cast<int>(pos.x - 25.f) / 50;
    const int cols = LevelEditorManager::GetInstance().SetCharAtPosition(col, row, itemChar);
    GameManager::GetInstance().PlaceNewItem(col, row, itemChar);
    m_BackgroundHandlerRef->TempReload(cols, LevelEditorManager::GetInstance().GetLevelInfo());
}

void superMarioBros::Selector::CreateBackgroundTexture()
{
    auto texture = std::make_unique<diji::GameObject>();
    const auto& sprite = texture->AddComponent<diji::SpriteRenderComponent>("graphics/tiles_sheet_selection.png");
    sprite->SetFrameSize(sf::Vector2i{ 50, 50 });
    sprite->SetTotalAnimationFrames(1);
    sprite->Pause();

    m_TextureGO = diji::SceneManager::GetInstance().SpawnGameObject("X_SelectorTexture", std::move(texture), sf::Vector2f{ 0.f, 0.f });
    m_TextureGO->AttachToObject(GetOwner(), false);

    m_SpriteRenderComp = m_TextureGO->GetComponent<diji::SpriteRenderComponent>();
    m_TextureGO->SetActive(false);
}
