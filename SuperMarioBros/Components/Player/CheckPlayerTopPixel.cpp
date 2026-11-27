#include "CheckPlayerTopPixel.h"
#include "PlayerCharacter.h"
#include "PlayerInputManager.h"
#include "../../Interfaces/IPlayerBumpable.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/SceneManager.h"

void superMarioBros::CheckPlayerTopPixel::Init()
{
    m_PlayerInputManagerCompPtr = GetOwner()->GetComponent<PlayerInputManager>();
    m_PlayerCharacterCompPtr = GetOwner()->GetComponent<PlayerCharacter>();
    m_ColliderCompPtr = GetOwner()->GetComponent<diji::Collider>();
    m_TransformCompPtr = GetOwner()->GetRootComponent();
}

void superMarioBros::CheckPlayerTopPixel::Update()
{
    if (m_PlayerCharacterCompPtr->IsDeadOrPaused())
    {
        m_WasRising = false;
        return;
    }

    const bool isRising = m_ColliderCompPtr->GetVelocity().y < 0.0f;
    const bool wasRising = m_WasRising;
    m_WasRising = isRising;

    if (isRising == false && wasRising == false)
        return;

    const float offset = m_PlayerCharacterCompPtr->IsSmallMario() ? 22.0f : 44.0f;
    const sf::Vector2f origin = m_TransformCompPtr->GetWorldPosition();
    constexpr sf::Vector2f dir = { 0, -1 };
    const sf::Vector2f top = { origin.x, origin.y - offset };

    if (const auto hit =  diji::SceneManager::GetInstance().GetPhysicsWorld()->Raycast(top, dir, 10.0f, m_ColliderCompPtr))
    {
        if (hit->info.hasCollision == false) return;

        const auto playerBumpableInterface = diji::InterfaceRegistry::GetInterface<IPlayerBumpable>(hit->collider->GetParent());
        if (hit->collider->GetCollisionResponse() == diji::Collider::CollisionResponse::Overlap && playerBumpableInterface == nullptr) return;

        m_PlayerInputManagerCompPtr->Bump();
        m_WasRising = false;

        if (playerBumpableInterface == nullptr) return;

        playerBumpableInterface->Bump();
    }
}
