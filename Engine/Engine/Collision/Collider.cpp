#include "Collider.h"

#include "PhysicsWorld.h"
#include "../Components/TextureComp.h"
#include "../Components/Transform.h"
#include "../Core/GameObject.h"
#include "../Singleton/Helpers.h"
#include "../Singleton/SceneManager.h"

void diji::Collider::Start()
{
    m_TransformCompPtr = GetOwner()->GetComponent<Transform>();
    m_LastPosition = m_TransformCompPtr->GetPosition();
    m_NewPosition = m_TransformCompPtr->GetPosition();

    const auto& pos = m_TransformCompPtr->GetPosition();
    m_Shape->UpdateAABB(pos);
    
    SceneManager::GetInstance().GetPhysicsWorld()->AddCollider(this);

    if (m_IsStatic)
        m_Shape->SetPosition(m_TransformCompPtr->GetPosition());
}

void diji::Collider::FixedUpdate()
{
    if (m_IsStatic) return;
    
    m_LastPosition = m_NewPosition;
}

void diji::Collider::Update()
{
    if (m_IsStatic) return;
    
    m_TransformCompPtr->SetPosition(Helpers::lerp(m_LastPosition, m_NewPosition, m_TimeSingletonInstance.GetFixedTimeAlpha()));
}

void diji::Collider::OnDestroy()
{
    SceneManager::GetInstance().GetPhysicsWorld()->RemoveCollider(this);
}

void diji::Collider::SetVelocity(const sf::Vector2f& vel)
{
    m_Velocity.x = std::clamp(vel.x, -m_MaxVelocity.x, m_MaxVelocity.x);
    m_Velocity.y = std::clamp(vel.y, -m_MaxVelocity.y, m_MaxVelocity.y);
}

void diji::Collider::SetPosition(const sf::Vector2f& pos) const
{
    m_TransformCompPtr->SetPosition(pos);
}

sf::Vector2f diji::Collider::GetPosition() const
{
    return m_TransformCompPtr->GetPosition(); // null check? fuck no if you have no transform comp wtf did you do
}

sf::FloatRect diji::Collider::GetAABB() const
{
    return GetAABBAt(GetPosition());
}

sf::FloatRect diji::Collider::GetAABBAt(const sf::Vector2f& pos) const
{
    sf::FloatRect rect;
    const sf::FloatRect& local = m_Shape->GetLocalShapeBounds();

    rect.left   = pos.x + local.left;
    rect.top    = pos.y + local.top;
    rect.width  = local.width;
    rect.height = local.height;
    return rect;
}

void diji::Collider::IgnoreCollider(const Collider* collider)
{
    m_IgnoredColliders.push_back(collider);
}

bool diji::Collider::IsIgnoringCollider(const Collider* collider) const
{
    return std::ranges::find(m_IgnoredColliders, collider) != m_IgnoredColliders.end();
}
