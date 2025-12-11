#include "CheckPlayerTopPixel.h"
#include "PlayerCharacter.h"
#include "../../Interfaces/IPlayerBumpable.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/SceneManager.h"

void superMarioBros::CheckPlayerTopPixel::Init()
{
    m_TransformCompPtr = GetOwner()->GetComponent<diji::Transform>();
    m_PlayerCharacterCompPtr = GetOwner()->GetComponent<PlayerCharacter>();
    m_ColliderCompPtr = GetOwner()->GetComponent<diji::Collider>();
}

void superMarioBros::CheckPlayerTopPixel::Update()
{
    if (m_PlayerCharacterCompPtr->IsDead() || m_PlayerCharacterCompPtr->IsPaused()) return;
    
    const float offset = m_PlayerCharacterCompPtr->GetPowerUpState() == 0 ? 22.f : 44.f;
    const sf::Vector2f origin = m_TransformCompPtr->GetPosition();
    const sf::Vector2f dir = { 0, -1 };
    const sf::Vector2f top = { origin.x, origin.y - offset };
    
    if (const auto hit =  diji::SceneManager::GetInstance().GetPhysicsWorld()->Raycast(top, dir, 10.f, m_ColliderCompPtr))
    {
        if (!hit->info.hasCollision) return;

        m_PlayerCharacterCompPtr->Bump();

        const auto playerBumpableInterface = diji::InterfaceRegistry::GetInterface<IPlayerBumpable>(hit->collider->GetParent());
        if (!playerBumpableInterface) return;
        
        playerBumpableInterface->Bump();
    }
}
