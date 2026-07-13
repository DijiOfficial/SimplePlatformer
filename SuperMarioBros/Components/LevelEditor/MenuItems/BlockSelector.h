#pragma once
#include "MenuItem.h"

namespace superMarioBros
{
    class SelectorControls;
    
    class BlockSelector final : public MenuItem
    {
    public:
        using MenuItem::MenuItem;

        void Start() override;

        [[nodiscard]] bool ActivateMenu() override;
        void CloseMenu() override;
        void Move(const sf::Vector2f& direction, bool isStart) override;
        [[nodiscard]] bool Select() override;
        [[nodiscard]] bool Return() override;

        void SetSelectorControls(SelectorControls* selectorControls) { m_SelectorControls = selectorControls; }
        void SetSelector(Selector* selector) { m_Selector = selector; }
        void SelectNextOrPreviousBlock(bool isNext);
        void CopyBlockHoveredPosition() const;
        
    private:
        diji::GameObject* m_ChildChoiceGO = nullptr;
        SelectorControls* m_SelectorControls = nullptr;
        Selector* m_Selector = nullptr;
        sf::Vector2f m_SelectorPosition;
        int m_CurrentBlockIndex = 0;
        int m_GridWidth = 0;
        int m_GridHeight = 0;

        std::unordered_map<int, sf::Vector2f> m_BlockTypePositionsMap;

        void SetAllBlocksPositions();
        void UpdateSelectorPosition();
    };
}
