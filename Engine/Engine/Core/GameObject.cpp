#include "GameObject.h"

#include "../Components/Render.h"
#include "../Components/Transform.h"
#include "../Singleton/SceneManager.h"

#include <variant>

void diji::GameObject::Init() const
{
    for (const auto& component : m_ComponentsPtrVec)
    {
        component->Init();
    }
}

void diji::GameObject::OnEnable() const
{
    for (const auto& component : m_ComponentsPtrVec)
    {
        component->OnEnable();
    }
}

void diji::GameObject::Start() const
{
    for (const auto& component : m_ComponentsPtrVec)
    {
        component->Start();
    }
}

void diji::GameObject::FixedUpdate() const
{
    for (const auto& component : m_ComponentsPtrVec)
    {
        component->FixedUpdate();
    }
}

void diji::GameObject::Update() const
{
    for (const auto& component : m_ComponentsPtrVec)
    {
        component->Update();
    }
}

void diji::GameObject::LateUpdate() const
{
    for (const auto& component : m_ComponentsPtrVec)
    {
        component->LateUpdate();
    }
}

void diji::GameObject::Render() const
{
    if (m_RenderCompPtr)
        m_RenderCompPtr->RenderFrame();
}

void diji::GameObject::OnDisable() const
{
    for (const auto& component : m_ComponentsPtrVec)
    {
        component->OnDisable();
    }
}

void diji::GameObject::OnDestroy() const
{
    for (const auto& component : m_ComponentsPtrVec)
    {
        component->OnDestroy();
    }
}

void diji::GameObject::Destroy() const
{
    SceneManager::GetInstance().SetPendingDestroy(this);
}

void diji::GameObject::CreateDuplicate(GameObject* duplicate) const
{
    for (const auto& entry : m_ComponentStorage)
    {
        entry.DuplicateComponents(duplicate);
    }
    
    duplicate->SetLocalPosition(m_LocalPosition);
}

sf::Vector2f diji::GameObject::GetWorldPosition()
{
    if (m_PositionIsDirty)
        UpdateWorldPosition();

    return m_TransformCompPtr->GetPosition();
}

void diji::GameObject::SetParent(GameObject* parent, const bool keepWorldPosition)
{
    if (parent == m_ParentPtr || parent == this || IsChildOf(parent))
        return;

    if (parent)
    {
        if (keepWorldPosition)
            SetLocalPosition(m_LocalPosition - parent->GetWorldPosition());
        SetPositionDirty();
    }
    else
        SetLocalPosition(GetWorldPosition());

    if (m_ParentPtr)
        m_ParentPtr->RemoveChild(this);

    m_ParentPtr = parent;
    
    if (m_ParentPtr)
        m_ParentPtr->AddChild(this);    
}

void diji::GameObject::SetLocalPosition(const sf::Vector2f& pos)
{
    m_LocalPosition = pos;
    SetPositionDirty();
}

bool diji::GameObject::IsChildOf(GameObject* potentialChild) const
{
    return std::ranges::any_of(m_ChildrenPtrVec,
       [&](const GameObject* child)
       {
           if (child == potentialChild)
               return true;

           return child->IsChildOf(potentialChild);
       });
}

void diji::GameObject::AddChild(GameObject* child)
{
    if (child == nullptr || child->IsChildOf(this))
        return;

    if (child->m_ParentPtr)
    {
        child->m_ParentPtr->RemoveChild(child);
    }

    child->m_ParentPtr = this;

    m_ChildrenPtrVec.push_back(child);

    child->UpdateWorldPosition();
}

void diji::GameObject::RemoveChild(GameObject* child)
{
    const auto it = std::ranges::find(m_ChildrenPtrVec, child);
    if (child == nullptr || it == m_ChildrenPtrVec.end())
        return;

    m_ChildrenPtrVec.erase(it);
    
    child->SetLocalPosition(GetWorldPosition() - child->m_LocalPosition); //local position now becomes world position
    child->SetPositionDirty();

    child->m_ParentPtr = nullptr;
}

void diji::GameObject::SetPositionDirty()
{
    m_PositionIsDirty = true;
    for (const auto& childPtr : m_ChildrenPtrVec)
    {
        if (not childPtr->m_PositionIsDirty)
            childPtr->SetPositionDirty();
    }
}

void diji::GameObject::UpdateWorldPosition()
{
    if (not m_PositionIsDirty)
        return;

    if (m_ParentPtr)
        m_TransformCompPtr->SetPosition(m_ParentPtr->GetWorldPosition() + m_LocalPosition);
    else
        m_TransformCompPtr->SetPosition(m_LocalPosition);
    
    m_PositionIsDirty = false;
}


