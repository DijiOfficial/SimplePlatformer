#pragma once
#include <SFML/System/Vector2.hpp>

namespace diji
{
    class SpriteRenderComponent;
    class GameObject;
}

namespace superMarioBros
{
    // so not an actual state pattern, better name would be animator controller
    class PlayerStates
    {
    public:
        PlayerStates() = default;
        virtual ~PlayerStates() noexcept = default;

        PlayerStates(const PlayerStates& other) = delete;
        PlayerStates(PlayerStates&& other) = delete;
        PlayerStates& operator=(const PlayerStates& other) = delete;
        PlayerStates& operator=(PlayerStates&& other) = delete;

        virtual void OnEnter(const diji::GameObject* parent);

        struct AnimationState
        {
            int startingFrameX = 0;  
            int startingFrameY = 0;
            int totalAnimationFrames = 1;
            float frameDuration = 0.25f;
            sf::Vector2i frameSize = { 16, 16 };

            bool m_IsPlaying = true;
            bool m_IsLooping = true;
        };
        
        enum class PlayerState
        {
            Idle,
            Walking,
            Running,
            Jumping,
            Death,
            Drifting,
            Growing,
            Shrinking,
            Invincibility,
            BigIdle,
            BigWalking,
            BigRunning,
            BigJumping,
            BigDrifting,
            FlagPoleSlide,
            Crouching,
        };
        [[nodiscard]] PlayerState GetState() const { return m_CurrentState; }
        
    protected:
        PlayerState m_CurrentState = PlayerState::Idle;
        AnimationState m_AnimationState;
        diji::SpriteRenderComponent* m_SpriteRenderComponent = nullptr;
    };

    class IdleState final : public PlayerStates
    {
    public:
        IdleState();
    };

    class WalkingState final : public PlayerStates
    {
    public:
        WalkingState();
    };

    class RunningState final : public PlayerStates
    {
    public:
        RunningState();
    };

    class JumpingState final : public PlayerStates
    {
    public:
        JumpingState();
    };

    class DeathState final : public PlayerStates
    {
    public:
        DeathState();
    };

    class DriftingState final : public PlayerStates
    {
    public:
        DriftingState();
    };
    
    class GrowthAnimationState final : public PlayerStates
    {
    public:
        GrowthAnimationState();
    };

    class ShrinkAnimationState final : public PlayerStates
    {
    public:
        ShrinkAnimationState();
    };

    class BigIdleState final : public PlayerStates
    {
    public:
        BigIdleState();
    };

    class BigWalkingState final : public PlayerStates
    {
    public:
        BigWalkingState();
    };

    class BigRunningState final : public PlayerStates
    {
    public:
        BigRunningState();
    };

    class BigJumpingState final : public PlayerStates
    {
    public:
        BigJumpingState();
    };

    class BigDriftingState final : public PlayerStates
    {
    public:
        BigDriftingState();
    };

    class FireIdleState final : public PlayerStates
    {
    public:
        FireIdleState();
    };

    class FireWalkingState final : public PlayerStates
    {
    public:
        FireWalkingState();
    };

    class FireRunningState final : public PlayerStates
    {
    public:
        FireRunningState();
    };

    class FireJumpingState final : public PlayerStates
    {
    public:
        FireJumpingState();
    };

    class FireDriftingState final : public PlayerStates
    {
    public:
        FireDriftingState();
    };

    class FireAnimationState final : public PlayerStates
    {
    public:
        FireAnimationState();
    };

    class ThrowingFireballState final : public PlayerStates
    {
    public:
        ThrowingFireballState();
    };

    class FlagPoleSlideState final : public PlayerStates
    {
    public:
        FlagPoleSlideState();
    };

    class BigFlagPoleSlideState final : public PlayerStates
    {
    public:
        BigFlagPoleSlideState();
    };

    class FireFlagPoleSlideState final : public PlayerStates
    {
    public:
        FireFlagPoleSlideState();
    };

    class CrouchingState final : public PlayerStates
    {
    public:
        CrouchingState();
    };

    class FireCrouchingState final : public PlayerStates
    {
    public:
        FireCrouchingState();
    };
}
