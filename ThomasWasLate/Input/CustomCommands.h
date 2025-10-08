#pragma once
#include "Engine/Input/GameActorCommand.h"

namespace thomasWasLate
{
    class tempTest;
}

namespace diji
{
    class Collider;
}

namespace thomasWasLate
{
    class PlayerCharacter;

    class MoveCharacter final : public diji::GameActorCommands
    {
    public:
        explicit MoveCharacter(diji::GameObject* actor, const sf::Vector2f& direction);
        ~MoveCharacter() noexcept override = default;

        void Execute() override;

    private:
        const sf::Vector2f m_Direction;
        PlayerCharacter* m_Character = nullptr;
    };

    class CharacterJump final : public diji::GameActorCommands
    {
    public:
        explicit CharacterJump(diji::GameObject* actor, const bool isJumping);
        ~CharacterJump() noexcept override = default;

        void Execute() override;

    private:
        PlayerCharacter* m_Character = nullptr;
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
        PlayerCharacter* m_Character = nullptr;
        bool m_IsSprinting = false;
    };

    class SpawnBox final : public diji::GameActorCommands
    {
    public:
        explicit SpawnBox(diji::GameObject* actor);
        ~SpawnBox() noexcept override = default;

        void Execute() override;
        // void Execute(const sf::Vector2i& pos) override;
        
    private:
        tempTest* huhBoxThing = nullptr;
    };

    class TrackMouse final : public diji::GameActorCommands
    {
    public:
        explicit TrackMouse(diji::GameObject* actor);
        ~TrackMouse() noexcept override = default;

        void Execute() override {}
        void Execute(const sf::Vector2i& pos) override;
        
    private:
        tempTest* huhBoxThing = nullptr;
    };
}
