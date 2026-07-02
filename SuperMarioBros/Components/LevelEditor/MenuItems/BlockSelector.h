#pragma once
#include "MenuItem.h"

namespace superMarioBros
{
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

        void SetSelectorGO(diji::GameObject* selectorGO) { m_SelectorGO = selectorGO; }
        void SetSelector(Selector* selector) { m_Selector = selector; }
        
    private:
        diji::GameObject* m_ChildChoiceGO = nullptr;
        diji::GameObject* m_SelectorGO = nullptr;
        Selector* m_Selector = nullptr;
        sf::Vector2f m_SelectorPosition;
        int m_CurrentBlockIndex = 0;
        int m_GridWidth = 0;
        int m_GridHeight = 0;

        enum eBlockType : std::uint8_t
        {
            Floor = 20,
            FloorGreen = 21,
            PipeTopLeft = 1,
            PipeTopRight = 2,
            FlagPoleTop = 3,
            Wall = 24,
            WallGreen = 25,
            PipeBottomLeft = 5,
            PipeBottomRight = 6,
            FlagPole = 7,
            PipeSideTopLeft = 8,
            PipeSideTopMiddle = 9,
            PipeSideTopRight = 10,
            PlaceHolder = 11,
            PipeSideBottomLeft = 12,
            PipeSideBottomMiddle = 13,
            PipeSideBottomRight = 14,
            PlaceHolder2 = 15,
            LuckyBlock = 16,
            BreakableBlock = 28,
            BreakableBlockGreen = 29,
            Coin = 32,
            PlaceHolder3 = 17,
            PlaceHolder4 = 18,
            PlaceHolder5 = 19,
            PlaceHolder6 = 22,
            PlaceHolder7 = 23,
            PlaceHolder8 = 26,
            PlaceHolder9 = 27,
            PlaceHolder10 = 30,
            PlaceHolder11 = 31,
            PlaceHolder12 = 33,
            PlaceHolder13 = 34,
            PlaceHolder14 = 35,
            PlaceHolder15 = 0,
            PlaceHolder16 = 4,

        };
        static std::unordered_map<int, eBlockType> m_BlockTypeMap;
        std::unordered_map<int, sf::Vector2f> m_BlockTypePositionsMap;

        void SetAllBlocksPositions();
        void UpdateSelectorPosition();
    };
}
