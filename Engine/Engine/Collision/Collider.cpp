#include "Collider.h"

#include "PhysicsWorld.h"
#include "../Components/TextureComp.h"
#include "../Components/Transform.h"
#include "../Core/GameObject.h"
#include "../Singleton/Helpers.h"
#include "../Singleton/SceneManager.h"

void diji::Collider::Start()
{
    if (m_IsInitialized) return;
    m_IsInitialized = true;
    
    m_TransformCompPtr = GetOwner()->GetRootComponent();
    m_LastPosition = m_TransformCompPtr->GetWorldPosition();
    m_NewPosition = m_LastPosition;

    m_Shape->UpdateAABB(m_NewPosition);
    
    SceneManager::GetInstance().GetPhysicsWorld()->AddCollider(this);

    if (m_IsActive && !GetOwner()->IsActive())
        SetActive(false);
}

void diji::Collider::Update()
{
    if (m_IsStatic) return;

    // todo: I don't like having to do this duplicate position update. it should be done once and the rest updated properly
    const auto pos = Helpers::lerp(m_LastPosition, m_NewPosition, m_TimeSingletonInstance.GetFixedTimeAlpha());
    m_TransformCompPtr->SetWorldPosition(pos);
    m_Shape->SetPosition(pos);
}

void diji::Collider::SyncTransform()
{
    SetNewPosition(m_TransformCompPtr->GetWorldPosition());
}

void diji::Collider::OnDestroy()
{
    SceneManager::GetInstance().GetPhysicsWorld()->RemoveCollider(this);
}

void diji::Collider::SetVelocity(const sf::Vector2f& vel)
{
    m_Velocity.x = std::clamp(vel.x, -m_MaxVelocity.x, m_MaxVelocity.x);
    m_Velocity.y = std::clamp(vel.y, -m_MaxVelocity.y, m_MaxVelocity.y);
    QueueWake(); 
}

sf::Vector2f diji::Collider::GetPosition() const
{
    return m_TransformCompPtr->GetWorldPosition();
}

sf::FloatRect diji::Collider::GetAABB() const
{
    return m_Shape->GetLocalShapeBounds();
}

// todo: rename the function to GetProposedAABB or something
sf::FloatRect diji::Collider::GetAABBAt(const sf::Vector2f& pos) const
{
    sf::FloatRect rect;
    const sf::FloatRect& local = m_Shape->GetLocalShapeBounds();

    rect.position.x = pos.x;
    rect.position.y = pos.y;
    rect.size.x     = local.size.x;
    rect.size.y     = local.size.y;
    return rect;
}

void diji::Collider::IgnoreCollider(const Collider* collider)
{
    m_IgnoredColliders.insert(collider);
}

void diji::Collider::ClearAllIgnoredColliders()
{
    m_IgnoredColliders = std::unordered_set<const Collider*>();
}

bool diji::Collider::IsIgnoringCollider(const Collider* collider) const
{
    return m_IgnoredColliders.contains(collider);
}

void diji::Collider::OverlapCollider(const Collider* collider)
{
     m_CollidersToOverlap.insert(collider);
}

bool diji::Collider::IsOverlappingCollider(const Collider* collider) const
{
    return m_CollidersToOverlap.contains(collider);
}

void diji::Collider::ClearOverlappedCollider(const Collider* collider)
{
    m_CollidersToOverlap.erase(collider);
}

void diji::Collider::ClearAllOverlappedCollider()
{
    m_CollidersToOverlap = std::unordered_set<const Collider*>();
}

void diji::Collider::ResizeCollider(const sf::Vector2f& size) const
{
    if (m_Type == CollisionShape::ShapeType::RECT)
        m_Shape->Resize(size);
    else
        throw std::logic_error("ResizeCollider with sf::Vector2f argument is only valid for RECT shape type.");

    m_Shape->UpdateAABB(m_TransformCompPtr->GetWorldPosition());
}

void diji::Collider::ResizeCollider(const float radius) const
{
    if (m_Type == CollisionShape::ShapeType::CIRCLE)
        m_Shape->Resize(radius);
    else
        throw std::logic_error("ResizeCollider with float argument is only valid for CIRCLE shape type.");

    m_Shape->UpdateAABB(m_TransformCompPtr->GetWorldPosition());
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
