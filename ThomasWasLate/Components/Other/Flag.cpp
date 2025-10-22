#include "Flag.h"

#include "../Player/PlayerCharacter.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Components/Transform.h"

void thomasWasLate::Flag::Init()
{
    diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar")->GetComponent<PlayerCharacter>()->OnLevelFinishedEvent.AddListener(this, &Flag::StartAnimation);
}

void thomasWasLate::Flag::StartAnimation()
{
    m_TimelinePtr = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());

    auto &track = m_TimelinePtr->AddFloatTrack("MoveVertically");
    track.keys = { { .time= 0.f, .value= 0.f }, { .time= 1.f, .value= 340.f }, };
    
    diji::Transform* transformPtr = GetOwner()->GetComponent<diji::Transform>();
    sf::Vector2f originalPos = transformPtr->GetPosition();
    
    track.onValue = [transformPtr, originalPos](const float y)
    {
        transformPtr->SetPosition(originalPos.x, originalPos.y + y);
    };

    auto& [eventName, eventKeysVec] = m_TimelinePtr->AddEventTrack("OnAnimationEnd");
    eventKeysVec =
    {
        { .time= 1.f, .callback= [&]()
            {
                OnFlagAnimationFinishedEvent.Broadcast();
                m_TimelinePtr->Stop();
            }
        }
    };
}


