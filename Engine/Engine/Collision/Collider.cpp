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

    if (m_IsActive && !GetOwner()->IsActive())
        SetActive(false);
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

void diji::Collider::OverlapCollider(const Collider* collider)
{
     m_CollidersToOverlap.push_back(collider);
}

bool diji::Collider::IsOverlappingCollider(const Collider* collider) const
{
    return std::ranges::find(m_CollidersToOverlap, collider) != m_CollidersToOverlap.end();
}

void diji::Collider::ClearOverlappedCollider(const Collider* collider)
{
    std::erase(m_CollidersToOverlap, collider);
}

void diji::Collider::ClearAllOverlappedCollider()
{
    m_CollidersToOverlap = std::vector<const Collider*>();
}

void diji::Collider::ResizeCollider(const sf::Vector2f& size) const
{
    if (m_Type == CollisionShape::ShapeType::RECT)
        m_Shape->Resize(size);
    else
        throw std::logic_error("ResizeCollider with sf::Vector2f argument is only valid for RECT shape type.");

    m_Shape->UpdateAABB(m_TransformCompPtr->GetPosition());
}

void diji::Collider::ResizeCollider(const float radius) const
{
    if (m_Type == CollisionShape::ShapeType::CIRCLE)
        m_Shape->Resize(radius);
    else
        throw std::logic_error("ResizeCollider with float argument is only valid for CIRCLE shape type.");

    m_Shape->UpdateAABB(m_TransformCompPtr->GetPosition());
}

sf::Vector2f diji::Collider::GetSurfaceNormalAt(const sf::Vector2f& point) const
{
    return m_Shape->GetSurfaceNormalAt(point);
}

void diji::Collider::ValidateColliderLists()
{
    std::erase_if(m_IgnoredColliders, [](const Collider* collider)
    {
        return collider == nullptr;
    });

    std::erase_if(m_CollidersToOverlap, [](const Collider* collider)
    {
        return collider == nullptr;
    });
}
