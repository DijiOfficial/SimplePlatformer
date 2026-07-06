#include "Transform.h"
#include "../Collision/Collider.h"
#include "../Core/GameObject.h"

#include <stdexcept>

#include "../Singleton/SceneManager.h"

sf::Vector2f diji::Transform::GetWorldPosition()
{
    return GetWorld(m_WorldPosition, m_IsPositionDirty, &Transform::UpdateWorldPosition);
}

void diji::Transform::SetWorldPosition(const sf::Vector2f& pos)
{
    m_LocalPosition = m_IsParented ? pos - m_ParentTransformCompPtr->GetWorldPosition() : pos;
    UpdateWorldPosition();
    MarkPositionDirty();
    m_IsPositionDirty = false;
}

sf::Angle diji::Transform::GetWorldRotation()
{
    return GetWorld(m_WorldRotation, m_IsRotationDirty, &Transform::UpdateWorldRotation);
}

void diji::Transform::SetWorldRotation(const sf::Angle& rotation)
{
    m_Rotation = m_IsParented ? rotation - m_ParentTransformCompPtr->GetWorldRotation() : rotation;
    UpdateWorldRotation();
    MarkRotationDirty();
    m_IsRotationDirty = false;
}

sf::Vector2f diji::Transform::GetWorldScale2D()
{
    return GetWorld(m_WorldScale2D, m_IsScaleDirty, &Transform::UpdateWorldScale2D);
}

void diji::Transform::SetWorldScale2D(const sf::Vector2f& scale)
{
    m_Scale2D = m_IsParented ? scale / m_ParentTransformCompPtr->GetWorldScale2D() : scale;
    UpdateWorldScale2D();
    MarkScaleDirty();
    m_IsScaleDirty = false;
}

void diji::Transform::AttachToObject(Transform* parent, const bool keepWorldPosition)
{
    if (!parent || parent == this)
        throw std::invalid_argument("Invalid parent.");

    for (const Transform* p = parent; p; p = p->m_ParentTransformCompPtr)
        if (p == this)
            throw std::logic_error("Cycle detected.");

    if (m_IsParented)
        DetachFromObject(keepWorldPosition);

    m_ParentTransformCompPtr = parent;
    m_IsParented = true;
    m_ParentTransformCompPtr->m_ChildrenTransformCompPtrVec.push_back(this);

    if (keepWorldPosition)
    {
        m_LocalPosition = GetWorldPosition() - m_ParentTransformCompPtr->GetWorldPosition();
        m_Rotation = GetWorldRotation() - m_ParentTransformCompPtr->GetWorldRotation();
        m_Scale2D = GetWorldScale2D() / m_ParentTransformCompPtr->GetWorldScale2D();
    }

    MarkPositionDirty();
    MarkRotationDirty();
    MarkScaleDirty();
}

void diji::Transform::DetachFromObject(const bool keepWorldPosition)
{
    if (!m_IsParented || !m_ParentTransformCompPtr)
        return;

    if (keepWorldPosition)
    {
        m_LocalPosition = GetWorldPosition();
        m_Rotation = GetWorldRotation();
        m_Scale2D = GetWorldScale2D();
    }

    auto& siblings = m_ParentTransformCompPtr->m_ChildrenTransformCompPtrVec;
    std::erase(siblings, this);

    m_ParentTransformCompPtr = nullptr;
    m_IsParented = false;

    MarkPositionDirty();
    MarkRotationDirty();
    MarkScaleDirty();
}

void diji::Transform::MarkDirtyMember(bool Transform::* dirtyMember)
{
    if (this->*dirtyMember) return;
    this->*dirtyMember = true;

    for (Transform* child : m_ChildrenTransformCompPtrVec)
        if (child)
            child->MarkDirtyMember(dirtyMember);
}

void diji::Transform::UpdateInSceneChunk() const
{
    SceneManager::GetInstance().UpdateGameObjectInChunk(m_OwnerPtr);
}
