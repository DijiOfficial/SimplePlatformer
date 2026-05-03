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
    if (m_PlayerCharacterCompPtr->IsDeadOrPaused()) return;
    if (m_ColliderCompPtr->GetVelocity().y >= 0) return;
    
    const float offset = m_PlayerCharacterCompPtr->IsSmallMario() ? 22.f : 44.f;
    const sf::Vector2f origin = m_TransformCompPtr->GetWorldPosition();
    constexpr sf::Vector2f dir = { 0, -1 };
    const sf::Vector2f top = { origin.x, origin.y - offset };
    
    if (const auto hit =  diji::SceneManager::GetInstance().GetPhysicsWorld()->Raycast(top, dir, 10.f, m_ColliderCompPtr))
    {
        if (!hit->info.hasCollision) return;

        m_PlayerInputManagerCompPtr->Bump();

        const auto playerBumpableInterface = diji::InterfaceRegistry::GetInterface<IPlayerBumpable>(hit->collider->GetParent());
        if (!playerBumpableInterface) return;

        playerBumpableInterface->Bump();
    }
}
