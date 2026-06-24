#pragma once
#include <memory>

#include "MenuItem.h"

namespace onScreenKeyboard
{
    class OnScreenKeyboardManager;
}

namespace diji
{
    class TextComp;
}

namespace superMarioBros
{
    class SaveMenu final : public MenuItem
    {
    public:
        using MenuItem::MenuItem;

        void Init() override;
        
        [[nodiscard]] bool ActivateMenu() override;
        void CloseMenu() override;
        void Move(const sf::Vector2f& direction, bool isStart) override;
        [[nodiscard]] bool Select() override;
        [[nodiscard]] bool Return() override;

        diji::Event<const std::string&> OnSaveLevelEvent;

    private:
        onScreenKeyboard::OnScreenKeyboardManager* m_KeyboardManagerRef = nullptr;
        diji::GameObject* m_ConfirmMenu = nullptr;
        diji::GameObject* m_YesNoMenu = nullptr;
        diji::GameObject* m_LevelText = nullptr;
        diji::GameObject* m_FakeCursor = nullptr;
        std::unique_ptr<diji::GameObject> m_ItemTemplateUPtr = nullptr;
        diji::TextComp* m_LevelNameText;
        sf::Vector2f m_YesPosition;
        sf::Vector2f m_NoPosition;
        sf::Vector2f m_LevelNamePosition;
        bool m_IsEditingLevelName;
        bool m_IsSaveSelected = true;
        bool m_IsYesSelected = true;
        bool m_IsTyping;
        bool m_IsFirstTimeEditing = true;
        bool m_IsConfirmChoice;

        const float ITEM_SPACING = 60.0f;

        sf::Vector2f GetSelectorPosition(const diji::GameObject* button) const;
        sf::Vector2f GetSelectorPosition(diji::TextComp* text) const;

        void UpdateCursorPosition() const;
    };
}
