#include "PlayerStates.h"

#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Core/GameObject.h"

void thomasWasLate::PlayerStates::OnEnter(const diji::GameObject* parent)
{
    m_SpriteRenderComponent = parent->GetComponent<diji::SpriteRenderComponent>();

    m_SpriteRenderComponent->SetFrameSize(m_AnimationState.frameSize);
    m_SpriteRenderComponent->SetStartingFrame(m_AnimationState.startingFrameX, m_AnimationState.startingFrameY);
    m_SpriteRenderComponent->SetTotalAnimationFrames(m_AnimationState.totalAnimationFrames);
    m_SpriteRenderComponent->SetFrameDuration(m_AnimationState.frameDuration);
    m_SpriteRenderComponent->SetLooping(m_AnimationState.m_IsLooping);
    if (m_AnimationState.m_IsPlaying)
        m_SpriteRenderComponent->Play();
    else
        m_SpriteRenderComponent->Pause();

    m_SpriteRenderComponent->SetCurrentAnimationFrame(0);

    m_SpriteRenderComponent->UpdateFrame();
}

thomasWasLate::IdleState::IdleState()
{
    m_AnimationState =
    {
        .startingFrameX= 0,
        .startingFrameY= 0,
        .totalAnimationFrames= 1,
        .frameDuration = 0.25f,
        .m_IsPlaying= false,
        .m_IsLooping= false
    };

    m_CurrentState = PlayerState::Idle;
}

thomasWasLate::WalkingState::WalkingState()
{
    m_AnimationState =
    {
        .startingFrameX= 1,
        .startingFrameY= 0,
        .totalAnimationFrames= 3,
        .frameDuration = 0.12f,
        .m_IsPlaying= true,
        .m_IsLooping= true
    };

    m_CurrentState = PlayerState::Walking;
}

thomasWasLate::RunningState::RunningState()
{
    m_AnimationState =
{
        .startingFrameX= 1,
        .startingFrameY= 0,
        .totalAnimationFrames= 3,
        .frameDuration = 0.065f,
        .m_IsPlaying= true,
        .m_IsLooping= true
    };

    m_CurrentState = PlayerState::Running;
}

thomasWasLate::JumpingState::JumpingState()
{
    m_AnimationState =
    {
        .startingFrameX= 0,
        .startingFrameY= 1,
        .totalAnimationFrames= 1,
        .frameDuration = 0.25f,
        .m_IsPlaying= false,
        .m_IsLooping= false
    };

    m_CurrentState = PlayerState::Jumping;
}

thomasWasLate::DeathState::DeathState()
{
    m_AnimationState =
    {
        .startingFrameX= 1,
        .startingFrameY= 1,
        .totalAnimationFrames= 1,
        .frameDuration = 0.25f,
        .m_IsPlaying= false,
        .m_IsLooping= false
    };

    m_CurrentState = PlayerState::Death;
}

thomasWasLate::DriftingState::DriftingState()
{
    m_AnimationState =
    {
        .startingFrameX= 2,
        .startingFrameY= 1,
        .totalAnimationFrames= 1,
        .frameDuration = 0.25f,
        .m_IsPlaying= false,
        .m_IsLooping= false
    };

    m_CurrentState = PlayerState::Drifting;
}

thomasWasLate::GrowthAnimationState::GrowthAnimationState()
{
    m_AnimationState =
    {
        .startingFrameX= 0,
        .startingFrameY= 1,
        .totalAnimationFrames= 12,
        .frameDuration = 0.065f,
        .frameSize = sf::Vector2i{ 16, 32 },
        
        .m_IsPlaying= true,
        .m_IsLooping= false
    };

    m_CurrentState = PlayerState::Drifting;
}

thomasWasLate::ShrinkAnimationState::ShrinkAnimationState()
{
    m_AnimationState =
    {
        .startingFrameX= 0,
        .startingFrameY= 2,
        .totalAnimationFrames= 2,
        .frameDuration = 0.02f,
        .frameSize = sf::Vector2i{ 16, 32 },
        
        .m_IsPlaying= true,
        .m_IsLooping= true
    };

    m_CurrentState = PlayerState::Shrinking;
}

thomasWasLate::BigIdleState::BigIdleState()
{
    m_AnimationState =
    {
        .startingFrameX= 11,
        .startingFrameY= 1,
        .totalAnimationFrames= 1,
        .frameDuration = 0.065f,
        .frameSize = sf::Vector2i{ 16, 32 },
    
        .m_IsPlaying= false,
        .m_IsLooping= false
    };

    m_CurrentState = PlayerState::BigIdle;
}

thomasWasLate::BigWalkingState::BigWalkingState()
{
    m_AnimationState =
    {
        .startingFrameX= 4,
        .startingFrameY= 0,
        .totalAnimationFrames= 3,
        .frameDuration = 0.12f,
        .frameSize = sf::Vector2i{ 16, 32 },
    
        .m_IsPlaying= true,
        .m_IsLooping= true
    };

    m_CurrentState = PlayerState::BigWalking;
}

thomasWasLate::BigRunningState::BigRunningState()
{
    m_AnimationState =
    {
        .startingFrameX= 4,
        .startingFrameY= 0,
        .totalAnimationFrames= 3,
        .frameDuration = 0.065f,
        .frameSize = sf::Vector2i{ 16, 32 },
    
        .m_IsPlaying= true,
        .m_IsLooping= true
    };

    m_CurrentState = PlayerState::BigRunning;
}

thomasWasLate::BigJumpingState::BigJumpingState()
{
    m_AnimationState =
    {
        .startingFrameX= 7,
        .startingFrameY= 0,
        .totalAnimationFrames= 1,
        .frameDuration = 0.065f,
        .frameSize = sf::Vector2i{ 16, 32 },
    
        .m_IsPlaying= false,
        .m_IsLooping= false
    };

    m_CurrentState = PlayerState::BigJumping;
}

thomasWasLate::BigDriftingState::BigDriftingState()
{
    m_AnimationState =
    {
        .startingFrameX= 8,
        .startingFrameY= 0,
        .totalAnimationFrames= 1,
        .frameDuration = 0.065f,
        .frameSize = sf::Vector2i{ 16, 32 },
    
        .m_IsPlaying= false,
        .m_IsLooping= false
    };

    m_CurrentState = PlayerState::BigDrifting;
}
