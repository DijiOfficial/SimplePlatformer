#pragma once

namespace diji
{
    class SpriteRenderComponent;
    class GameObject;
}

namespace thomasWasLate
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

            bool m_IsPlaying = true;
            bool m_IsLooping = true;
        };
        
        enum class PlayerState
        {
            Idle,
            Walking,
            Running,
            Jumping,
            Falling
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
}
