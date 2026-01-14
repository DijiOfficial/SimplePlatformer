#include "PlayerStates.h"

#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Core/GameObject.h"

void superMarioBros::PlayerStates::OnEnter(const diji::GameObject* parent)
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

superMarioBros::IdleState::IdleState()
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

superMarioBros::WalkingState::WalkingState()
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

superMarioBros::RunningState::RunningState()
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

superMarioBros::JumpingState::JumpingState()
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

superMarioBros::DeathState::DeathState()
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

superMarioBros::DriftingState::DriftingState()
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

superMarioBros::GrowthAnimationState::GrowthAnimationState()
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

superMarioBros::ShrinkAnimationState::ShrinkAnimationState()
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

superMarioBros::BigIdleState::BigIdleState()
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

superMarioBros::BigWalkingState::BigWalkingState()
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

superMarioBros::BigRunningState::BigRunningState()
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

superMarioBros::BigJumpingState::BigJumpingState()
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

superMarioBros::BigDriftingState::BigDriftingState()
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

superMarioBros::FireIdleState::FireIdleState()
{
    m_AnimationState =
    {
        .startingFrameX= 0,
        .startingFrameY= 3,
        .totalAnimationFrames= 1,
        .frameDuration = 0.065f,
        .frameSize = sf::Vector2i{ 16, 32 },
    
        .m_IsPlaying= false,
        .m_IsLooping= false
    };

    m_CurrentState = PlayerState::BigIdle;
}

superMarioBros::FireWalkingState::FireWalkingState()
{
    m_AnimationState =
    {
        .startingFrameX= 4,
        .startingFrameY= 3,
        .totalAnimationFrames= 3,
        .frameDuration = 0.12f,
        .frameSize = sf::Vector2i{ 16, 32 },
    
        .m_IsPlaying= true,
        .m_IsLooping= true
    };

    m_CurrentState = PlayerState::BigWalking;
}

superMarioBros::FireRunningState::FireRunningState()
{
    m_AnimationState =
    {
        .startingFrameX= 4,
        .startingFrameY= 3,
        .totalAnimationFrames= 3,
        .frameDuration = 0.065f,
        .frameSize = sf::Vector2i{ 16, 32 },
    
        .m_IsPlaying= true,
        .m_IsLooping= true
    };

    m_CurrentState = PlayerState::BigRunning;
}

superMarioBros::FireJumpingState::FireJumpingState()
{
    m_AnimationState =
    {
        .startingFrameX= 7,
        .startingFrameY= 3,
        .totalAnimationFrames= 1,
        .frameDuration = 0.065f,
        .frameSize = sf::Vector2i{ 16, 32 },
    
        .m_IsPlaying= false,
        .m_IsLooping= false
    };

    m_CurrentState = PlayerState::BigJumping;
}

superMarioBros::FireDriftingState::FireDriftingState()
{
    m_AnimationState =
    {
        .startingFrameX= 8,
        .startingFrameY= 3,
        .totalAnimationFrames= 1,
        .frameDuration = 0.065f,
        .frameSize = sf::Vector2i{ 16, 32 },
    
        .m_IsPlaying= false,
        .m_IsLooping= false
    };

    m_CurrentState = PlayerState::BigDrifting;
}

superMarioBros::FireAnimationState::FireAnimationState()
{
    m_AnimationState =
    {
        .startingFrameX= 0,
        .startingFrameY= 3,
        .totalAnimationFrames= 4,
        .frameDuration = 0.065f,
        .frameSize = sf::Vector2i{ 16, 32 },
        
        .m_IsPlaying= true,
        .m_IsLooping= true
    };

    m_CurrentState = PlayerState::Growing;
}

superMarioBros::ThrowingFireballState::ThrowingFireballState()
{
    m_AnimationState =
    {
        .startingFrameX= 10,
        .startingFrameY= 3,
        .totalAnimationFrames= 1,
        .frameDuration = 0.065f,
        .frameSize = sf::Vector2i{ 16, 32 },
        
        .m_IsPlaying= false,
        .m_IsLooping= false
    };

    m_CurrentState = PlayerState::Growing;
}

superMarioBros::FlagPoleSlideState::FlagPoleSlideState()
{
    m_AnimationState =
    {
        .startingFrameX= 11,
        .startingFrameY= 5,
        .totalAnimationFrames= 2,
        .frameDuration = 0.065f,
        .frameSize = sf::Vector2i{ 16, 16 },
        
        .m_IsPlaying= true,
        .m_IsLooping= true
    };

    m_CurrentState = PlayerState::FlagPoleSlide;
}

superMarioBros::BigFlagPoleSlideState::BigFlagPoleSlideState()
{
    m_AnimationState =
    {
        .startingFrameX= 10,
        .startingFrameY= 0,
        .totalAnimationFrames= 2,
        .frameDuration = 0.065f,
        .frameSize = sf::Vector2i{ 16, 32 },
        
        .m_IsPlaying= true,
        .m_IsLooping= true
    };

    m_CurrentState = PlayerState::FlagPoleSlide;
}

superMarioBros::FireFlagPoleSlideState::FireFlagPoleSlideState()
{
    m_AnimationState =
    {
        .startingFrameX= 11,
        .startingFrameY= 3,
        .totalAnimationFrames= 2,
        .frameDuration = 0.065f,
        .frameSize = sf::Vector2i{ 16, 32 },
        
        .m_IsPlaying= true,
        .m_IsLooping= true
    };

    m_CurrentState = PlayerState::FlagPoleSlide;
}

superMarioBros::CrouchingState::CrouchingState()
{
    m_AnimationState =
    {
        .startingFrameX= 9,
        .startingFrameY= 0,
        .totalAnimationFrames= 1,
        .frameDuration = 0.065f,
        .frameSize = sf::Vector2i{ 16, 32 },
        
        .m_IsPlaying= false,
        .m_IsLooping= false
    };

    m_CurrentState = PlayerState::Crouching;
}

superMarioBros::FireCrouchingState::FireCrouchingState()
{
    m_AnimationState =
    {
        .startingFrameX= 9,
        .startingFrameY= 3,
        .totalAnimationFrames= 1,
        .frameDuration = 0.065f,
        .frameSize = sf::Vector2i{ 16, 32 },
        
        .m_IsPlaying= false,
        .m_IsLooping= false
    };

    m_CurrentState = PlayerState::Crouching;
}
