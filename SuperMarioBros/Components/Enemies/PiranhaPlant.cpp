#include "PiranhaPlant.h"

#include "../../Singletons/GameManager.h"
#include "../Player/PlayerCharacter.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Interfaces/ISoundSystem.h"
#include "Engine/Components/Transform.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Interfaces/Timeline.h"

void superMarioBros::PiranhaPlant::Start()
{
    BaseEnemy::Start();

    if (const auto player = diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar"))
    {
        m_PlayerTransformCompPtr = player->GetComponent<diji::Transform>();
        player->GetComponent<PlayerCharacter>()->OnHitByEnemyEvent.AddListener(this, &PiranhaPlant::PauseTimeline);
        player->GetComponent<PlayerCharacter>()->OnPoweringUpEvent.AddListener(this, &PiranhaPlant::PauseTimeline);
    }

    CreateTimeLine();
    m_TimelinePtr->Pause();
}

void superMarioBros::PiranhaPlant::Update()
{
    if (m_Paused) return;
    if (!m_CanAttack) return;

    constexpr float triggerDistance = 100.f * 100.f;
    const float distance = m_TransformCompPtr->GetPosition().x - m_PlayerTransformCompPtr->GetPosition().x;
    if (distance * distance <= triggerDistance) return;
    
    m_TimelinePtr->PlayFromStart();
    m_CanAttack = false;
    m_IsEventTriggered = false;
}

void superMarioBros::PiranhaPlant::OnTriggerEnter(const diji::Collider* other, const diji::CollisionInfo&)
{
    if (m_Paused) return;
    if (other->GetTag() == "fireBall")
        return Kill(false, true);
}

void superMarioBros::PiranhaPlant::OnDestroy()
{
    BaseEnemy::OnDestroy();

    if (m_TimelinePtr)
        m_TimelinePtr->Stop();
    diji::TimerManager::GetInstance().ClearTimer(m_TimerHandle);
}

void superMarioBros::PiranhaPlant::Kill(const bool, const bool addPoints)
{
    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_kick.wav", false);
    m_Paused = true;

    if (!addPoints) return;
    GameManager::SpawnPointsText(m_TransformCompPtr->GetPosition(), "200");
    GameManager::GetInstance().OnScoreAddedEvent.Broadcast(200);

    Destroy();
}

void superMarioBros::PiranhaPlant::CreateTimeLine()
{
    m_TimelinePtr = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());

    auto &track = m_TimelinePtr->AddFloatTrack("MoveVertically");
    track.keys = { { .time= 0.f, .value= 0.f }, { .time= 0.9f, .value= -80.f }, { .time= 1.9f, .value= -80.f }, { .time= 2.8f, .value= 0.f }, };

    const sf::Vector2f originalPos = m_TransformCompPtr->GetPosition();
    const auto transformPtr = m_TransformCompPtr;
    track.onValue = [originalPos, transformPtr](const float y)
    {
        transformPtr->SetPosition(originalPos.x, originalPos.y + y);
    };

    auto& [eventName, eventKeysVec] = m_TimelinePtr->AddEventTrack("OnAnimationEnd");
    eventKeysVec =
    {
        { .time= 2.8f, .callback= [&]()
            {
                if (m_IsEventTriggered) return;
                m_IsEventTriggered = true;

                m_TimerHandle = diji::TimerManager::GetInstance().SetTimer([&]()
                {
                    m_CanAttack = true;
                }, 1.f, false);
                m_TimelinePtr->Stop();
            }
        }
    };
}

void superMarioBros::PiranhaPlant::PauseTimeline() const
{
    m_TimelinePtr->Pause();
}

void superMarioBros::PiranhaPlant::PauseTimeline(const bool isPaused) const
{
    if (isPaused)
        m_TimelinePtr->Pause();
    else
        m_TimelinePtr->Play();  
}
