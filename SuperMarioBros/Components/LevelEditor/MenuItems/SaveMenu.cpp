#include "SaveMenu.h"

#include "../../../Singletons/LevelEditorManager.h"
#include "../OnScreenKeyboard/OnScreenKeyboardManager.h"
#include "Engine/Components/Blinking.h"
#include "Engine/Components/TextComp.h"
#include "Engine/Components/Render.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/SceneManager.h"

void superMarioBros::SaveMenu::Init()
{
    auto keyboard = std::make_unique<diji::GameObject>();
    keyboard->AddComponent<onScreenKeyboard::OnScreenKeyboardManager>();
    m_KeyboardManagerRef = keyboard->GetComponent<onScreenKeyboard::OnScreenKeyboardManager>();
    diji::SceneManager::GetInstance().SpawnGameObject("X_Keyboard", std::move(keyboard), sf::Vector2f{0, 0 });
    
    m_ConfirmMenu = diji::SceneManager::GetInstance().SpawnGameObject("X_KeyboardMenu", std::make_unique<diji::GameObject>(), sf::Vector2f{ window::VIEWPORT.x * 0.5f, window::VIEWPORT.y * 0.5f });

    m_ItemTemplateUPtr = std::make_unique<diji::GameObject>();
    m_ItemTemplateUPtr->AddComponent<diji::TextComp>("SAVE", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    m_ItemTemplateUPtr->GetComponent<diji::TextComp>()->GetText().setCharacterSize(25);
    m_ItemTemplateUPtr->GetComponent<diji::TextComp>()->SetCentered(true);
    m_ItemTemplateUPtr->AddComponent<diji::Render>();

    const auto& buttonSave = diji::SceneManager::GetInstance().AddGameObjectToCanvas("ZZ_ButtonYES", m_ItemTemplateUPtr.get(), sf::Vector2f{ -ITEM_SPACING * 2, 0 });
    const auto& buttonReturn = diji::SceneManager::GetInstance().AddGameObjectToCanvas("ZZ_ButtonNO", m_ItemTemplateUPtr.get(), sf::Vector2f{ ITEM_SPACING * 2, 0 });
    
    m_FakeCursor = diji::SceneManager::GetInstance().AddGameObjectToCanvas("ZZ_ButtonNO", m_ItemTemplateUPtr.get(), sf::Vector2f{ ITEM_SPACING * 2, 0 });
    m_LevelText = diji::SceneManager::GetInstance().AddGameObjectToCanvas("ZZ_Text", m_ItemTemplateUPtr.get(), sf::Vector2f{ 0, -ITEM_SPACING });
    m_LevelText->SetObjectPosition(m_ConfirmMenu->GetObjectPosition() - sf::Vector2f{ 0, window::VIEWPORT.y * 0.05f });
    m_FakeCursor->SetObjectPosition(m_LevelText->GetObjectPosition());
    const auto& blink = m_FakeCursor->AddComponent<diji::Blinking>(0.2f, 0.2f);
    blink->InitializeRenderComp();
    m_FakeCursor->GetComponent<diji::TextComp>()->GetText().setString("|");
    m_FakeCursor->SetActive(false);
    
    buttonReturn->GetComponent<diji::TextComp>()->GetText().setString("RETURN");
    buttonSave->AttachToObject(m_ConfirmMenu, false);
    buttonReturn->AttachToObject(m_ConfirmMenu, false);

    m_LevelNameText = m_LevelText->GetComponent<diji::TextComp>();
    m_ConfirmMenu->SetActive(false);
    m_LevelText->SetActive(false);
    
    m_YesPosition = GetSelectorPosition(buttonSave);
    m_NoPosition  = GetSelectorPosition(buttonReturn);

    //yes no confirmation menu
    m_YesNoMenu = diji::SceneManager::GetInstance().SpawnGameObject("X_YesNoMenu", std::make_unique<diji::GameObject>(), sf::Vector2f{ window::VIEWPORT.x * 0.5f, window::VIEWPORT.y * 0.5f });
    const auto& yes = diji::SceneManager::GetInstance().AddGameObjectToCanvas("ZZ_ButtonYES_YES", m_ItemTemplateUPtr.get(), sf::Vector2f{ -ITEM_SPACING * 2, 0 });
    const auto& no = diji::SceneManager::GetInstance().AddGameObjectToCanvas("ZZ_ButtonNO_NO", m_ItemTemplateUPtr.get(), sf::Vector2f{ ITEM_SPACING * 2, 0 });
    const auto& confirmText = diji::SceneManager::GetInstance().AddGameObjectToCanvas("ZZ_ConfirmText", m_ItemTemplateUPtr.get(), sf::Vector2f{ 0, ITEM_SPACING * -2 });

    yes->AttachToObject(m_YesNoMenu, false);
    no->AttachToObject(m_YesNoMenu, false);
    confirmText->AttachToObject(m_YesNoMenu, false);
    yes->GetComponent<diji::TextComp>()->GetText().setString("YES");
    no->GetComponent<diji::TextComp>()->GetText().setString("NO");
    confirmText->GetComponent<diji::TextComp>()->GetText().setString("This level already exists. Overwrite?");
    m_YesNoMenu->SetActive(false);

    m_SaveErrorText = diji::SceneManager::GetInstance().AddGameObjectToCanvas("ZZ_SaveErrorText", m_ItemTemplateUPtr.get(), sf::Vector2f{ window::VIEWPORT.x * 0.5f, window::VIEWPORT.y * 0.3f });
    m_SaveErrorText->SetActive(false);
}

bool superMarioBros::SaveMenu::ActivateMenu()
{
    const std::string& levelName = LevelEditorManager::GetInstance().GetCurrentLevelName() == "New Level" ? "Custom Level" : LevelEditorManager::GetInstance().GetCurrentLevelName();

    m_LevelNameText->GetText().setString(levelName);
    m_ConfirmMenu->SetActive(true);
    m_MenuArrow->SetActive(true);
    m_LevelText->SetActive(true);

    m_LevelNamePosition = GetSelectorPosition(m_LevelNameText);
    m_MenuArrow->SetObjectPosition(m_LevelNamePosition);
    m_IsEditingLevelName = true;
    m_IsTyping = false;
    
    return true;
}

void superMarioBros::SaveMenu::CloseMenu()
{
    if (m_IsConfirmChoice)
    {
        m_IsConfirmChoice = false;
        m_YesNoMenu->SetActive(false);
        m_ConfirmMenu->SetActive(true);
        m_MenuArrow->SetObjectPosition(m_IsSaveSelected ? m_YesPosition : m_NoPosition);
        return;
    }
    
    if (m_IsTyping)
    {
        m_KeyboardManagerRef->Toggle();
        m_IsTyping = false;
        m_ConfirmMenu->SetActive(true);
        m_MenuArrow->SetActive(true);
        m_LevelText->AddObjectWorldOffset(sf::Vector2f{ 0, 75 });
        if (m_LevelNameText->GetText().getString().isEmpty())
        {
            m_IsFirstTimeEditing = true;
            m_LevelNameText->GetText().setString("Custom Level");
        }
        m_FakeCursor->SetActive(false);
        
        m_LevelNamePosition = GetSelectorPosition(m_LevelNameText);
        m_MenuArrow->SetObjectPosition(m_LevelNamePosition);
        m_IsEditingLevelName = true;
        return;
    }

    m_LevelText->SetActive(false);
    m_ConfirmMenu->SetActive(false);
    m_MenuArrow->SetActive(false);
    m_IsEditingLevelName = false;
}

void superMarioBros::SaveMenu::Move(const sf::Vector2f& direction, const bool isStart)
{
    if (isStart == false)
        return;

    if (m_IsTyping)
    {
        m_KeyboardManagerRef->Move(direction);
        return;
    }

    if (m_IsConfirmChoice)
    {
        if (diji::Helpers::AreFloatEqual(direction.x, 0.0f))
            return;
        
        m_IsYesSelected = m_IsYesSelected == false;
        m_MenuArrow->SetObjectPosition(m_IsYesSelected ? m_YesPosition : m_NoPosition);
        
        return;
    }
    
    if (diji::Helpers::AreFloatEqual(direction.y, 0.0f) == false)
    {
        if (m_IsEditingLevelName)
        {
            m_MenuArrow->SetObjectPosition(m_YesPosition);
            m_IsEditingLevelName = false;
            m_IsSaveSelected = true;
        }
        else
        {
            m_LevelNamePosition = GetSelectorPosition(m_LevelNameText);
            m_MenuArrow->SetObjectPosition(m_LevelNamePosition);
            m_IsEditingLevelName = true;
        }
        return;
    }
    
    if (m_IsEditingLevelName)
        return;
    
    m_IsSaveSelected = m_IsSaveSelected == false;
    m_MenuArrow->SetObjectPosition(m_IsSaveSelected ? m_YesPosition : m_NoPosition);
}

bool superMarioBros::SaveMenu::Select()
{   
    if (m_IsConfirmChoice)
    {
        m_YesNoMenu->SetActive(false);
        m_IsConfirmChoice = false;

        if (m_IsYesSelected)
        {
            SaveLevel();
            return false;
        }
        
        m_ConfirmMenu->SetActive(true);
        m_MenuArrow->SetObjectPosition(m_IsSaveSelected ? m_YesPosition : m_NoPosition);
        return true;
    }
    
    if (m_IsTyping)
    {
        const char key = m_KeyboardManagerRef->ConfirmSelection();

        // todo: handle special keys like backspace, enter, etc.
        if (m_LevelNameText->GetText().getString().getSize() < 20)
            m_LevelNameText->GetText().setString(m_LevelNameText->GetText().getString() + key);
        
        UpdateCursorPosition();
        return true;
    }

    if (m_IsEditingLevelName)
    {
        if (m_IsFirstTimeEditing)
        {
            m_LevelNameText->GetText().setString("");
            m_IsFirstTimeEditing = false;
        }
        
        m_FakeCursor->SetActive(true);
        m_LevelText->AddObjectWorldOffset(sf::Vector2f{ 0, -75 });
        UpdateCursorPosition();
        m_ConfirmMenu->SetActive(false);
        m_MenuArrow->SetActive(false);
        m_KeyboardManagerRef->Toggle();
        m_IsTyping = true;
        return true;
    }

    if (m_IsSaveSelected)
    {
        if (LevelEditorManager::GetInstance().DoesLevelExist(m_LevelNameText->GetText().getString()))
        {
            m_YesNoMenu->SetActive(true);
            m_ConfirmMenu->SetActive(false);
            m_MenuArrow->SetObjectPosition(m_IsYesSelected ? m_YesPosition : m_NoPosition);
            m_IsConfirmChoice = true;
            return true;
        }
        
        SaveLevel();
    }

    return false;
}

bool superMarioBros::SaveMenu::Return()
{
    const bool quit = m_IsTyping || m_IsConfirmChoice;
    CloseMenu();
    
    return quit;
}

sf::Vector2f superMarioBros::SaveMenu::GetSelectorPosition(const diji::GameObject* button) const
{
    const auto& text = button->GetComponent<diji::TextComp>()->GetText();
    const sf::FloatRect bounds = text.getGlobalBounds();

    return sf::Vector2f
    {
        button->GetRootComponent()->GetWorldPosition().x - bounds.size.x * 0.5f - SELECTOR_SPACING,
        button->GetRootComponent()->GetWorldPosition().y
    };
}

sf::Vector2f superMarioBros::SaveMenu::GetSelectorPosition(diji::TextComp* text) const
{
    const sf::FloatRect bounds = text->GetText().getGlobalBounds();
    return sf::Vector2f
    {
        text->GetOwner()->GetRootComponent()->GetWorldPosition().x - bounds.size.x * 0.5f - SELECTOR_SPACING,
        text->GetOwner()->GetRootComponent()->GetWorldPosition().y
    };
}

void superMarioBros::SaveMenu::UpdateCursorPosition() const
{
    m_FakeCursor->SetObjectPosition(m_LevelText->GetObjectPosition() + sf::Vector2f{ m_LevelNameText->GetText().getGlobalBounds().size.x * 0.5f + m_FakeCursor->GetComponent<diji::TextComp>()->GetText().getGlobalBounds().size.y * 0.5f, 0 });
}

void superMarioBros::SaveMenu::SaveLevel()
{
    const std::string missingRequirement = LevelEditorManager::GetInstance().GetMissingSaveRequirement();
    if (missingRequirement.empty() == false)
    {
        ShowSaveError(missingRequirement);
        return;
    }

    const std::string levelName = m_LevelNameText->GetText().getString();
    LevelEditorManager::GetInstance().SaveNewMap(levelName, levelName + ".txt");
    OnSaveLevelEvent.Broadcast(levelName);
}

void superMarioBros::SaveMenu::ShowSaveError(const std::string& message)
{
    m_SaveErrorText->GetComponent<diji::TextComp>()->GetText().setString(message);
    m_SaveErrorText->SetActive(true);

    diji::TimerManager::GetInstance().ClearTimer(m_SaveErrorTimerHandle);
    m_SaveErrorTimerHandle = diji::TimerManager::GetInstance().SetTimer(this, [this]()
    {
        m_SaveErrorText->SetActive(false);
    }, SAVE_ERROR_DURATION);
}
