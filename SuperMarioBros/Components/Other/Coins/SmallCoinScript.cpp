#include "SmallCoinScript.h"

#include "../../../Singletons/GameManager.h"
#include "Engine/Components/Transform.h"
#include "Engine/Interfaces/ISoundSystem.h"
#include "Engine/Interfaces/Timeline.h"
#include "Engine/Singleton/SceneManager.h"

void superMarioBros::SmallCoinScript::Init()
{
    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_coin.wav", false);

    diji::Timeline* timelinePtr = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());

    auto &track = timelinePtr->AddFloatTrack("MoveVertically");
    track.keys = { { .time= 0.f, .value= 0.f }, { .time= 0.3f, .value= -180.f }, { .time= 0.6f, .value= 0.f } };
    
    diji::Transform* transformPtr = GetOwner()->GetRootComponent();
    sf::Vector2f originalPos = transformPtr->GetWorldPosition();
    
    track.onValue = [transformPtr, originalPos](const float y)
    {
        transformPtr->SetWorldPosition(sf::Vector2f{ originalPos.x, originalPos.y + y });
    };

    auto& [eventName, eventKeysVec] = timelinePtr->AddEventTrack("OnAnimationEnd");
    eventKeysVec =
    {
        { .time= 0.6f, .callback= [&]()
            {
                Destroy();
            
                const auto& pos = GetOwner()->GetObjectPosition();
                GameManager::SpawnPointsText(pos, "200");
                GameManager::GetInstance().OnScoreAddedEvent.Broadcast(200);
                GameManager::GetInstance().OnCoinCollectedEvent.Broadcast();
            }
        }
    };
}
