#include "FireBall.h"

#include "../../Singletons/GameManager.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/AutoDestroy.h"
#include "Engine/Components/Camera.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/SpriteRenderComp.h"

namespace diji
{
    class SpriteRenderComponent;
}

thomasWasLate::FireBall::FireBall(diji::GameObject* ownerPtr, diji::Collider* owner, const bool isGoingRight)
    : Component{ ownerPtr }
    , m_PlayerCollider{ owner }
    , m_IsGoingRight{ isGoingRight }
{
}

void thomasWasLate::FireBall::Init()
{
    m_CameraPtr = diji::SceneManager::GetInstance().GetMainCamera()->GetComponent<diji::Camera>();
    m_ColliderCompPtr = GetOwner()->GetComponent<diji::Collider>();
    
    const auto collider = GetOwner()->GetComponent<diji::Collider>();
    collider->ApplyImpulse(sf::Vector2f{ m_IsGoingRight ? 1000.f : -1000.f , 1000.f });
    collider->SetRestitution(1.2f);
    collider->SetMass(0.5f);
    collider->SetStaticFriction(0.0f);
    collider->SetKineticFriction(0.0f);
    collider->SetMaxVelocity(sf::Vector2f{ 1000.f, 750.f });
    collider->SetGenerateHitEvents(true);
    collider->SetTag("fireBall");

    collider->IgnoreCollider(m_PlayerCollider);
    m_PlayerCollider = nullptr;
}

void thomasWasLate::FireBall::LateUpdate()
{
    if (m_CameraPtr->GetViewBounds().intersects(m_ColliderCompPtr->GetAABB())) return;

    Destroy();
}

void thomasWasLate::FireBall::OnDestroy()
{
    GameManager::GetInstance().FireballRemoved();
}

void thomasWasLate::FireBall::OnHitEvent(const diji::Collider* other, const diji::CollisionInfo& hitInfo)
{
    if (diji::Helpers::isZero(hitInfo.normal.x) && other->GetTag() != "enemy")
        return;

    Destroy();

    auto explosion = std::make_unique<diji::GameObject>();
    explosion->AddComponents<diji::Transform>(300, 200);
    explosion->AddComponents<diji::SpriteRenderComponent>("graphics/explosion.png", sf::Vector2i{ 50,50 }, 4, 0.035f);
    explosion->GetComponent<diji::SpriteRenderComponent>()->SetLooping(false);
    explosion->AddComponents<diji::AutoDestroy>(1.f);
    diji::SceneManager::GetInstance().SpawnGameObject("Y_fireBallExplosion", std::move(explosion), GetOwner()->GetComponent<diji::Transform>()->GetPosition());
}
