#include "PlayerStates.h"

#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Core/GameObject.h"

void thomasWasLate::PlayerStates::OnEnter(const diji::GameObject* parent)
{
    m_SpriteRenderComponent = parent->GetComponent<diji::SpriteRenderComponent>();

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
        .totalAnimationFrames= 4,
        .frameDuration = 0.1f,
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
        .totalAnimationFrames= 4,
        .frameDuration = 0.05f,
        .m_IsPlaying= true,
        .m_IsLooping= true
    };

    m_CurrentState = PlayerState::Running;
}
