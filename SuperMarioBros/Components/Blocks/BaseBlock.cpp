#include "BaseBlock.h"
#include "../../Helpers/MarioHelpers.h"
#include "Engine/Interfaces/ISoundSystem.h"
#include "Engine/Interfaces/Timeline.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Components/Transform.h"

#include <format>

superMarioBros::BaseBlock::BaseBlock(diji::GameObject* ownerPtr, const ItemSpawnType itemSpawnType, std::string audioFileName, const bool shouldSwitch)
    : Component(ownerPtr)
    , m_AudioName(std::move(audioFileName))
    , m_ItemSpawnType(itemSpawnType)
    , m_ShouldSwitchOnHit(shouldSwitch)
{
}

void superMarioBros::BaseBlock::Init()
{
    CreateTimeline();
    CreateItemTemplate();
    m_ColliderCompPtr = GetOwner()->GetComponent<diji::Collider>();
    m_TransformCompPtr = GetOwner()->GetRootComponent();
}

void superMarioBros::BaseBlock::Bump()
{
    if (m_IsHit)
        return;

    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_bump.wav", false);
    if (!m_AudioName.empty())
        diji::ServiceLocator::GetSoundSystem().AddSoundRequest(std::format("sound/{}", m_AudioName), false);

    MarioHelpers::CheckForCollisionAboveBlock(m_ColliderCompPtr);
    PlayAnimation();
    OnAnimationStart();

    if (m_ShouldSwitchOnHit)
        SwitchToEmptyBlockState();

    // OnCustomBumpLogic(); // not necessary?
}

void superMarioBros::BaseBlock::OnAnimationStart()
{
    if (m_ItemSpawnType != ItemSpawnType::Coin) return;

    MarioHelpers::SpawnCoinAboveBlock(m_ColliderCompPtr->GetPosition());
}

void superMarioBros::BaseBlock::CreateTimeline()
{
    m_TimelinePtr = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());

    auto &track = m_TimelinePtr->AddFloatTrack("MoveVertically");
    track.keys = { { .time= 0.f, .value= 0.f }, { .time= 0.1f, .value= -20.f }, { .time= 0.2f, .value= 0.f } };
    
    diji::Transform* transformPtr = GetOwner()->GetRootComponent();
    sf::Vector2f originalPos = transformPtr->GetWorldPosition();
    
    track.onValue = [transformPtr, originalPos](const float y)
    {
        transformPtr->SetWorldPosition(sf::Vector2f{ originalPos.x, originalPos.y + y });
    };

    auto& [eventName, eventKeysVec] = m_TimelinePtr->AddEventTrack("OnAnimationEnd");
    eventKeysVec =
    {
        { .time= 0.2f, .callback= [&]()
            {
                OnAnimationEnd();
                
                if (m_ItemTemplateUPtr)
                    (void)diji::SceneManager::GetInstance().SpawnGameObject("C_PowerUp", m_ItemTemplateUPtr.get(), m_TransformCompPtr->GetWorldPosition());
            }
        }
    };

    m_TimelinePtr->Pause();
}

void superMarioBros::BaseBlock::PlayAnimation() const
{
    m_TimelinePtr->PlayFromStart();
}

void superMarioBros::BaseBlock::SwitchToEmptyBlockState()
{
    m_IsHit = true;
    UnregisterInterface();

    const auto spriteRenderComp = GetOwner()->GetComponent<diji::SpriteRenderComponent>();
    
    spriteRenderComp->SetStartingFrame(0, 0);
    spriteRenderComp->SetTotalAnimationFrames(0);
    spriteRenderComp->SetFrameDuration(0);
    spriteRenderComp->SetLooping(false);
    spriteRenderComp->Pause();
    spriteRenderComp->SetCurrentAnimationFrame(0);
}
