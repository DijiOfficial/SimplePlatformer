#pragma once
#include "MenuItem.h"

#include <unordered_map>

namespace superMarioBros
{
    class SelectorControls;

    class GridMenuItem : public MenuItem
    {
    public:
        using MenuItem::MenuItem;

        void Start() override;

        [[nodiscard]] bool ActivateMenu() override;
        void CloseMenu() override;
        void Move(const sf::Vector2f& direction, bool isStart) override;
        [[nodiscard]] bool Return() override;

        void SetSelectorControls(SelectorControls* selectorControls) { m_SelectorControls = selectorControls; }
        void SelectNextOrPrevious(bool isNext);

    protected:
        int m_CurrentIndex = 0;
        int m_GridWidth = 0;
        int m_GridHeight = 0;

        [[nodiscard]] sf::Vector2i GetCurrentGridPos() const { return sf::Vector2i{ m_CurrentIndex % m_GridWidth, m_CurrentIndex / m_GridWidth }; }
        void SetCurrentGridPos(const sf::Vector2i& gridPos) { m_CurrentIndex = gridPos.y * m_GridWidth + gridPos.x; }
        void UpdateSelectorPosition();
        [[nodiscard]] virtual bool IsCellSelectable(int) const { return true; }

    private:
        diji::GameObject* m_ChildChoiceGO = nullptr;
        SelectorControls* m_SelectorControls = nullptr;
        std::unordered_map<int, sf::Vector2f> m_GridPositionsMap;

        void SetAllGridPositions();
    };
}
