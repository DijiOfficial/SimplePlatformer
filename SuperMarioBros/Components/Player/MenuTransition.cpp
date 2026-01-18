#include "MenuTransition.h"
#include "PlayerCharacter.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Interfaces/ISoundSystem.h"
#include "Engine/Singleton/SceneManager.h"

void superMarioBros::MenuTransition::Start()
{
    GetOwner()->GetComponent<PlayerCharacter>()->SetTransitionState();

    // create timeline for moving down the pole
    m_TransformCompPtr = GetOwner()->GetRootComponent();
    const auto timeline = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());
    sf::Vector2f originalPos = m_TransformCompPtr->GetWorldPosition();

    auto &track = timeline->AddFloatTrack("MoveVertically");
    track.keys = { { .time= 0.f, .value= 0.f }, { .time= 3.f, .value= 1450 } };
        
    track.onValue = [&, originalPos](const float x)
    {
        m_TransformCompPtr->SetWorldPosition(sf::Vector2f{ originalPos.x + x, m_TransformCompPtr->GetWorldPosition().y });
    };

    auto& [eventName, eventKeysVec] = timeline->GetEventTrack("OnAnimationEnd");
    eventKeysVec =
    {

        { .time= 2.75f, .callback= [&]()
            {
                diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_pipe.wav", false);
            }
        },

        
        { .time= 3.f, .callback= [&]()
            {
                SetActive(false);
            }
        }
    };
}
