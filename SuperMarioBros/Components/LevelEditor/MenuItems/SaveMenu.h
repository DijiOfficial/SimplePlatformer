#pragma once
#include "MenuItem.h"

namespace superMarioBros
{
    class SaveMenu final : public MenuItem
    {
    public:
        using MenuItem::MenuItem;
        
        [[nodiscard]] MenuInfo ActivateMenu() override;
        void CloseMenu() override;
    };
}
