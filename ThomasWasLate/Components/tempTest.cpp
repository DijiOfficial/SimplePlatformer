#include "tempTest.h"

#include "Engine/Core/GameObject.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/ShapeRender.h"
#include "Engine/Singleton/RandNumber.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Components/Transform.h"

void thomasWasLate::tempTest::Init()
{
    // m_ColliderPtr = GetOwner()->GetComponent<diji::Collider>();
    // m_ColliderPtr->GetShape()->SetRotation(-90.f);
}

void thomasWasLate::tempTest::FixedUpdate()
{
    // m_Angle += 45.f * diji::TimeSingleton::GetInstance().GetFixedUpdateDeltaTime();
    // m_ColliderPtr->GetShape()->SetRotation(m_Angle);
}

void thomasWasLate::tempTest::SpawnRandomCollider()
{
    auto tempBound = std::make_unique<diji::GameObject>();
    tempBound->AddComponents<diji::Transform>(m_MousePos);
    tempBound->AddComponents<diji::ShapeRender>();
    tempBound->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ diji::RandNumber::GetRandomRangeFloat(25, 100), diji::RandNumber::GetRandomRangeFloat(25, 100) });
    const auto collider = tempBound->GetComponent<diji::Collider>();
    collider->SetMass(25);

    diji::SceneManager::GetInstance().SpawnGameObject("ColliderBox", std::move(tempBound), m_MousePos);

    OnBoxSpawnedEvent.Broadcast();
}
