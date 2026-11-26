#include "PlayMenu.h"

#include "../../../Singletons/LevelEditorManager.h"
#include "Engine/Components/TextComp.h"
#include "Engine/Components/Render.h"
#include "Engine/Core/Engine.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/SceneManager.h"

void superMarioBros::PlayMenu::Init()
{
    m_TextTemplateUPtr = std::make_unique<diji::GameObject>();
    m_TextTemplateUPtr->AddComponent<diji::TextComp>("PLAY", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    m_TextTemplateUPtr->GetComponent<diji::TextComp>()->GetText().setCharacterSize(25);
    m_TextTemplateUPtr->GetComponent<diji::TextComp>()->SetCentered(true);
    m_TextTemplateUPtr->AddComponent<diji::Render>();

    m_ErrorText = diji::SceneManager::GetInstance().AddGameObjectToCanvas("ZZ_PlayErrorText", m_TextTemplateUPtr.get(), sf::Vector2f{ window::VIEWPORT.x * 0.5f, window::VIEWPORT.y * 0.3f });
    m_ErrorText->SetActive(false);
}

bool superMarioBros::PlayMenu::ActivateMenu()
{
    LevelEditorManager& levelEditorManager = LevelEditorManager::GetInstance();
    if (levelEditorManager.GetCurrentMapInfo().start.has_value() == false)
    {
        ShowError("Place the start position before playing");
        return false;
    }

    levelEditorManager.StartPlayTest();
    return false;
}

void superMarioBros::PlayMenu::ShowError(const std::string& message)
{
    m_ErrorText->GetComponent<diji::TextComp>()->GetText().setString(message);
    m_ErrorText->SetActive(true);

    diji::TimerManager::GetInstance().ClearTimer(m_ErrorTimerHandle);
    m_ErrorTimerHandle = diji::TimerManager::GetInstance().SetTimer(this, [this]()
    {
        m_ErrorText->SetActive(false);
    }, ERROR_MESSAGE_DURATION);
}
