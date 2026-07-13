#pragma once
#include "Engine/Input/GameActorCommand.h"

namespace superMarioBros
{
    class BlockSelector;
    class SelectorControls;
}

namespace superMarioBros
{
    class PlayerInputManager;

    class MoveSelector final : public diji::GameActorCommands
    {
    public:
        explicit MoveSelector(diji::GameObject* actor, const sf::Vector2f& direction, bool isStart);
        ~MoveSelector() noexcept override = default;

        void Execute() override;

    private:
        SelectorControls* m_Selector = nullptr;
        const sf::Vector2f m_Direction;
        bool m_IsStart;
    };

    class SelectMenuItem final : public diji::GameActorCommands
    {
    public:
        explicit SelectMenuItem(diji::GameObject* actor);
        ~SelectMenuItem() noexcept override = default;

        void Execute() override;
        
    private:
        SelectorControls* m_Selector = nullptr;
    };

    class ClearSpecialMenu final : public diji::GameActorCommands
    {
    public:
        explicit ClearSpecialMenu(diji::GameObject* actor);
        ~ClearSpecialMenu() noexcept override = default;

        void Execute() override;
        
    private:
        SelectorControls* m_Selector = nullptr;
    };

    class OpenSaveMenu final : public diji::GameActorCommands
    {
    public:
        explicit OpenSaveMenu(diji::GameObject* actor);
        ~OpenSaveMenu() noexcept override = default;

        void Execute() override;
        
    private:
        SelectorControls* m_Selector = nullptr;
    };
    
    class OpenBlocksMenu final : public diji::GameActorCommands
    {
    public:
        explicit OpenBlocksMenu(diji::GameObject* actor);
        ~OpenBlocksMenu() noexcept override = default;

        void Execute() override;
        
    private:
        SelectorControls* m_Selector = nullptr;
    };

    class SelectNextBlock final : public diji::GameActorCommands
    {
    public:
        explicit SelectNextBlock(diji::GameObject* actor, bool isNext);
        ~SelectNextBlock() noexcept override = default;

        void Execute() override;
        
    private:
        bool m_IsNext;
        BlockSelector* m_Selector = nullptr;
    };

    class  CopyBlockHoveredPosition final : public diji::GameActorCommands
    {
    public:
        explicit CopyBlockHoveredPosition(diji::GameObject* actor);
        ~CopyBlockHoveredPosition() noexcept override = default;

        void Execute() override;
        
    private:
        BlockSelector* m_Selector = nullptr;
    };
}
