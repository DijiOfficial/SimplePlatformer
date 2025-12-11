#include "HiddenBlock.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Collision/Collider.h"

void superMarioBros::HiddenBlock::Start()
{
    const auto spriteRender = GetOwner()->GetComponent<diji::SpriteRenderComponent>();
    spriteRender->SetStartingFrame(2, 0);
    spriteRender->SetTotalAnimationFrames(0);
    spriteRender->SetFrameDuration(0);
    spriteRender->SetLooping(false);
    spriteRender->Pause();
    spriteRender->SetCurrentAnimationFrame(0);

    spriteRender->UpdateFrame();
}

void superMarioBros::HiddenBlock::OnAnimationStart()
{
    m_ColliderCompPtr->SetCollisionResponse(diji::Collider::CollisionResponse::Block);
}