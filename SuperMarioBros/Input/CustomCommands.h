#pragma once
#include "Engine/Input/GameActorCommand.h"

namespace superMarioBros
{
    class tempTest;
}

namespace diji
{
    class Collider;
}

namespace superMarioBros
{
    class PlayerInputManager;

    class MoveCharacter final : public diji::GameActorCommands
    {
    public:
        explicit MoveCharacter(diji::GameObject* actor, const sf::Vector2f& direction, bool isMoving);
        ~MoveCharacter() noexcept override = default;

        void Execute() override;

    private:
        const sf::Vector2f m_Direction;
        PlayerInputManager* m_CharacterInput = nullptr;
        bool m_IsMoving = false;
    };

    class CharacterJump final : public diji::GameActorCommands
    {
    public:
        explicit CharacterJump(diji::GameObject* actor, const bool isJumping);
        ~CharacterJump() noexcept override = default;

        void Execute() override;

    private:
        PlayerInputManager* m_CharacterInput = nullptr;
        bool m_IsJumping = false;
    };

    class TempAddImpulse final : public diji::GameActorCommands
    {
    public:
        explicit TempAddImpulse(diji::GameObject* actor);
        ~TempAddImpulse() noexcept override = default;

        void Execute() override;

    private:
        diji::Collider* m_Collider = nullptr;
        float m_RandomForce = 2000.f;
    };

    class Sprint final : public diji::GameActorCommands
    {
    public:
        explicit Sprint(diji::GameObject* actor, bool isSprinting);
        ~Sprint() noexcept override = default;

        void Execute() override;
        
    private:
        PlayerInputManager* m_CharacterInput = nullptr;
        bool m_IsSprinting = false;
    };

    class Attack final : public diji::GameActorCommands
    {
    public:
        explicit Attack(diji::GameObject* actor);
        ~Attack() noexcept override = default;

        void Execute() override;
        
    private:
        PlayerInputManager* m_CharacterInput = nullptr;
    };

    class StartGame final : public diji::GameActorCommands
    {
    public:
        explicit StartGame(diji::GameObject* actor) : GameActorCommands{ actor } {};
        ~StartGame() noexcept override = default;

        void Execute() override;
    };

    class Pause final : public diji::GameActorCommands
    {
    public:
        explicit Pause(diji::GameObject* actor) : GameActorCommands{ actor } {}
        ~Pause() noexcept override = default;

        void Execute() override;
    };

    class Crouch final : public diji::GameActorCommands
    {
    public:
        explicit Crouch(diji::GameObject* actor, bool isStart);
        ~Crouch() noexcept override = default;

        void Execute() override;
        
    private:
        PlayerInputManager* m_CharacterInput = nullptr;
        bool m_IsStart = false;
    };
}
