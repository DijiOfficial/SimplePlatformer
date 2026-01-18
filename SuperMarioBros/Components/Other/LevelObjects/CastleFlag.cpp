#include "CastleFlag.h"

#include "../HUD/HudManager.h"
#include "../../../Singletons/GameManager.h"
#include "Engine/Components/AutoDestroy.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Components/Transform.h"
#include "Engine/Interfaces/ISoundSystem.h"
#include "Engine/Singleton/RandNumber.h"
#include "Engine/Singleton/TimerManager.h"

void superMarioBros::CastleFlag::Init()
{
    const auto HUDEvent = diji::SceneManager::GetInstance().GetGameObject("Z_HUDManager");
    if (HUDEvent)
        HUDEvent->GetComponent<HudManager>()->OnScoreCountedEvent.AddListener(this, &CastleFlag::StartAnimation);
}

void superMarioBros::CastleFlag::Update()
{
    if (m_FireworksToSpawn <= 0)
        diji::TimerManager::GetInstance().ClearTimer(m_TimerHandle);
}

void superMarioBros::CastleFlag::StartAnimation(const int fireworksToSpawn)
{
    m_FireworksToSpawn = fireworksToSpawn;
    const auto timeline = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());

    auto &track = timeline->AddFloatTrack("MoveVertically");
    track.keys = { { .time= 0.f, .value= 0.f }, { .time= 0.5f, .value= 75.f }, };
    
    diji::Transform* transformPtr = GetOwner()->GetRootComponent();
    sf::Vector2f originalPos = transformPtr->GetWorldPosition();
    
    track.onValue = [transformPtr, originalPos](const float y)
    {
        transformPtr->SetWorldPosition(sf::Vector2f{ originalPos.x, originalPos.y - y });
    };

    auto& [eventName, eventKeysVec] = timeline->AddEventTrack("OnAnimationEnd");
    eventKeysVec =
    {
        { .time= 0.5f, .callback= [&]()
            {
                if (m_EventWasTriggered) return;
                m_EventWasTriggered = true;
            
                if (m_FireworksToSpawn > 0)
                {
                    m_TimerHandle = diji::TimerManager::GetInstance().SetTimer([&]()
                    {
                        SpawnFirework();
                        --m_FireworksToSpawn;
                    }, 0.5f, true, -0.5f);
                }

                (void)diji::TimerManager::GetInstance().SetTimer([&]()
                {
                    GameManager::GetInstance().SetLevelCleared();
                }, static_cast<float>(m_FireworksToSpawn) * 0.5f + 0.82f, false);
            }
        }
    };
}

void superMarioBros::CastleFlag::SpawnFirework()
{
    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_fireworks.wav", false);

    const sf::Vector2f center = GetOwner()->GetRootComponent()->GetWorldPosition();

    constexpr float halfWidth = 400.0f;
    constexpr float heightAbove = 400.0f;
    const float xMin = m_LastFireworkSpawnedIsOnLeftSide ? (center.x - halfWidth) : center.x;
    const float xMax = m_LastFireworkSpawnedIsOnLeftSide ? center.x : (center.x + halfWidth);
    const float randomX = diji::RandNumber::GetRandomRangeFloat(xMin, xMax);
    const float randomY = diji::RandNumber::GetRandomRangeFloat(center.y - heightAbove, center.y);
    const sf::Vector2f spawnPos{ randomX, randomY };

    auto fireWork = std::make_unique<diji::GameObject>();
    fireWork->AddComponent<diji::SpriteRenderComponent>("graphics/explosion.png", sf::Vector2i{ 50, 50 }, 3, 0.135f);
    fireWork->AddComponent<diji::AutoDestroy>(0.405f);

    (void)diji::SceneManager::GetInstance().SpawnGameObject("G_fireWork", std::move(fireWork), spawnPos);
    GameManager::GetInstance().OnScoreAddedEvent.Broadcast(500);

    m_LastFireworkSpawnedIsOnLeftSide = !m_LastFireworkSpawnedIsOnLeftSide;
}

