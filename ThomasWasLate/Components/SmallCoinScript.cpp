#include "SmallCoinScript.h"

#include "../Singletons/GameManager.h"
#include "Engine/Components/Transform.h"
#include "Engine/Interfaces/Timeline.h"
#include "Engine/Singleton/SceneManager.h"

void thomasWasLate::SmallCoinScript::Init()
{
    diji::Timeline* timelinePtr = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());

    auto &track = timelinePtr->AddFloatTrack("MoveVertically");
    track.keys = { { .time= 0.f, .value= 0.f }, { .time= 0.3f, .value= -180.f }, { .time= 0.6f, .value= 0.f } };
    
    diji::Transform* transformPtr = GetOwner()->GetComponent<diji::Transform>();
    sf::Vector2f originalPos = transformPtr->GetPosition();
    
    track.onValue = [transformPtr, originalPos](const float y)
    {
        transformPtr->SetPosition(originalPos.x, originalPos.y + y);
    };

    auto& [eventName, eventKeysVec] = timelinePtr->AddEventTrack("OnAnimationEnd");
    eventKeysVec =
    {
        { .time= 0.6f, .callback= [&]()
            {
                diji::SceneManager::GetInstance().SetPendingDestroy(GetOwner());
                const auto& pos = GetOwner()->GetComponent<diji::Transform>()->GetPosition();
                GameManager::SpawnPointsText(pos, "200");
                GameManager::GetInstance().OnScoreAddedEvent.Broadcast(200);
            }
        }
    };
}
