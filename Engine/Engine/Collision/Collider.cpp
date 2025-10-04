#include "Collider.h"

#include "PhysicsWorld.h"
#include "../Components/TextureComp.h"
#include "../Components/Transform.h"
#include "../Components/RectRender.h"
#include "../Core/GameObject.h"
#include "../Singleton/Helpers.h"
#include "../Singleton/SceneManager.h"


void diji::Collider::Start()
{
    m_TransformCompPtr = GetOwner()->GetComponent<Transform>();
    m_LastPosition = m_TransformCompPtr->GetPosition();
    m_NewPosition = m_TransformCompPtr->GetPosition();

    // if (!m_IsCollisionSet)
    // {
    //     const auto& size = GetOwner()->GetComponent<TextureComp>()->GetSize();
    //     m_CollisionBox = { .left = 0, .bottom = 0, .width = static_cast<float>(size.x), .height = static_cast<float>(size.y) };
    //     m_IsCollisionSet = true;
    //     
    //     if (GetOwner()->GetComponent<TextureComp>()->IsCentered())
    //         m_Offset = { static_cast<float>(size.x) * -0.5f, static_cast<float>(size.y) * -0.5f };
    // }

    const auto& pos = m_TransformCompPtr->GetPosition();
    m_Shape->UpdateAABB(pos);
    // m_CollisionBox.left = pos.x + m_Offset.x;
    // m_CollisionBox.bottom = pos.y + m_Offset.y;

    // Add collider to WorldPhysics
    // CollisionSingleton::GetInstance().AddCollider(this, m_CollisionBox);

    SceneManager::GetInstance().GetPhysicsWorld()->AddCollider(this);

    if (const auto& rectRenderComp = GetOwner()->GetComponent<RectRender>())
        rectRenderComp->SetRectangle(sf::RectangleShape{ m_Shape->GetAABB().getSize() });

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

// // todo: this should be late update and check for world collision it;s shit right now
// void diji::Collider::Update() // todo: not a fan, consider updating position when necessary rather than every frame, using dirty flag or other
// {
//     m_LastState = m_CollisionBox;
//     const auto& pos = m_TransformCompPtr->GetPosition();
//     if (std::is_eq( pos.x <=> m_CollisionBox.left) and std::is_eq(pos.y <=> m_CollisionBox.bottom))
//         return;
//
//     m_CollisionBox.left = pos.x + m_Offset.x;
//     m_CollisionBox.bottom = pos.y + m_Offset.y;
//
//     CollisionSingleton::GetInstance().UpdateCollider(this, m_CollisionBox);
// }

void diji::Collider::OnDestroy()
{
    SceneManager::GetInstance().GetPhysicsWorld()->RemoveCollider(this);
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

// void diji::Collider::UpdateColliderFromTexture()
// {
//     const auto& size = GetOwner()->GetComponent<TextureComp>()->GetSize();
//     m_CollisionBox = { .left = 0, .bottom = 0, .width = static_cast<float>(size.x), .height = static_cast<float>(size.y) };
//     m_IsCollisionSet = true;
//     
//     if (GetOwner()->GetComponent<TextureComp>()->IsCentered())
//         m_Offset = { static_cast<float>(size.x) * -0.5f, static_cast<float>(size.y) * -0.5f };
//
//     const auto& pos = m_TransformCompPtr->GetPosition();
//     m_CollisionBox.left = pos.x + m_Offset.x;
//     m_CollisionBox.bottom = pos.y + m_Offset.y;
//
//     CollisionSingleton::GetInstance().UpdateCollider(this, m_CollisionBox);
// }

// sf::Vector2f diji::Collider::GetPosition() const
// {
//     return sf::Vector2f{ m_CollisionBox.left, m_CollisionBox.bottom };
// }
//
// sf::Vector2f diji::Collider::GetCenter() const
// {
//     return sf::Vector2f{ m_CollisionBox.left + m_CollisionBox.width * 0.5f, m_CollisionBox.bottom + m_CollisionBox.height * 0.5f };
// }
