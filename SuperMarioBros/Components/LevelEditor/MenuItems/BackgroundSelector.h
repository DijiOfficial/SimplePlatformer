#pragma once
#include "GridMenuItem.h"
#include "../../../Helpers/Vector2iHash.h"

#include <string>
#include <unordered_map>

namespace superMarioBros
{
    class BackgroundSelector final : public GridMenuItem
    {
    public:
        using GridMenuItem::GridMenuItem;

        [[nodiscard]] bool ActivateMenu() override;
        [[nodiscard]] bool Select() override;

    private:
        const std::unordered_map<sf::Vector2i, std::string, Vector2iHash> m_GridToBackgroundMap =
        {
            { {0, 0}, "background.png" },
            { {1, 0}, "black" },
        };
    };
}
