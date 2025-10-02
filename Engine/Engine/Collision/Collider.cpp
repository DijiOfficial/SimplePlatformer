#include "Collider.h"

#include "../Components/TextureComp.h"
#include "../Components/Transform.h"
#include "../Core/GameObject.h"

void diji::Collider::Start()
{
    m_TransformCompPtr = GetOwner()->GetComponent<Transform>();

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
    // todo: destroy from WorldPhysics
    // CollisionSingleton::GetInstance().RemoveCollider(this);
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
