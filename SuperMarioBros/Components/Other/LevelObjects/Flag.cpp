#include "Flag.h"

#include "../../Player/PlayerCharacter.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Components/Transform.h"

void superMarioBros::Flag::Init()
{
    diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar")->GetComponent<PlayerCharacter>()->OnLevelFinishedEvent.AddListener(this, &Flag::StartAnimation);
}

void superMarioBros::Flag::StartAnimation()
{
    m_TimelinePtr = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());

    auto &track = m_TimelinePtr->AddFloatTrack("MoveVertically");
    track.keys = { { .time= 0.f, .value= 0.f }, { .time= 1.31f, .value= 490.f }, };
    
    diji::Transform* transformPtr = GetOwner()->GetRootComponent();
    sf::Vector2f originalPos = transformPtr->GetWorldPosition();
    
    track.onValue = [transformPtr, originalPos](const float y)
    {
        transformPtr->SetWorldPosition(sf::Vector2f{ originalPos.x, originalPos.y + y });
    };

    auto& [eventName, eventKeysVec] = m_TimelinePtr->AddEventTrack("OnAnimationEnd");
    eventKeysVec =
    {
        { .time= 1.31f, .callback= [&]()
            {
                OnFlagAnimationFinishedEvent.Broadcast();
                m_TimelinePtr->Stop();
            }
        }
    };
}


