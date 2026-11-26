#pragma once
#include "MenuItem.h"
#include "Engine/Singleton/TimerManager.h"

#include <memory>

namespace superMarioBros
{
    class PlayMenu final : public MenuItem
    {
    public:
        using MenuItem::MenuItem;

        void Init() override;

        [[nodiscard]] bool ActivateMenu() override;
        void CloseMenu() override {}
        void Move(const sf::Vector2f&, bool) override {}
        [[nodiscard]] bool Select() override { return false; }

    private:
        std::unique_ptr<diji::GameObject> m_TextTemplateUPtr = nullptr;
        diji::GameObject* m_ErrorText = nullptr;
        diji::TimerManager::TimerHandle m_ErrorTimerHandle = 0;

        void ShowError(const std::string& message);
    };
}
