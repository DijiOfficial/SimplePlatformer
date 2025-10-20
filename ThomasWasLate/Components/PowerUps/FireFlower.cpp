#include "FireFlower.h"

#include "Engine/Core/GameObject.h"
#include "Engine/Components/Transform.h"
#include "Engine/Singleton/SceneManager.h"
#include "../Player/PlayerCharacter.h"
#include "../../Singletons/GameManager.h"
#include "Engine/Collision/Collider.h"

void thomasWasLate::FireFlower::Init()
{
    m_TransformCompPtr = GetOwner()->GetComponent<diji::Transform>();

    PlayStartAnimation();
}

void thomasWasLate::FireFlower::OnTriggerEnter(const diji::Collider* other)
{
    if (other->GetTag() != "player")
        return;
    
    diji::SceneManager::GetInstance().SetPendingDestroy(GetOwner());
    const auto& pos = m_TransformCompPtr->GetPosition();
    constexpr float yOffset = 50.f;
    const auto& scorePos = sf::Vector2f{ pos.x, pos.y - yOffset };
    GameManager::SpawnPointsText(scorePos, "1000");
}

void thomasWasLate::FireFlower::PlayStartAnimation() const
{
    const auto& timelinePtr = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());

    auto &track = timelinePtr->AddFloatTrack("MoveVertically");
    track.keys = { { .time= 0.f, .value= 0.f }, { .time= 0.8f, .value= -50.f } };

    diji::Transform* transformPtr = GetOwner()->GetComponent<diji::Transform>();
    sf::Vector2f originalPos = transformPtr->GetPosition();

    track.onValue = [transformPtr, originalPos](const float y)
    {
        transformPtr->SetPosition(originalPos.x, originalPos.y + y);
    };
}
