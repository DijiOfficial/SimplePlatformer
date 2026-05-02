#include "GameObject.h"

#include "../Components/Render.h"
#include "../Collision/Collider.h"
#include "../Singleton/SceneManager.h"

diji::GameObject::GameObject()
    : GameObject{ sf::Vector2f{ 0, 0 } }
{
}

diji::GameObject::GameObject(const sf::Vector2f& position)
{
    m_RootTransform = std::make_unique<Transform>(position);
}

diji::GameObject::GameObject(const float x, const float y)
    : GameObject{ sf::Vector2f{ x, y } }
{
}

diji::GameObject::GameObject(const int x, const int y)
    : GameObject{ sf::Vector2f{ static_cast<float>(x), static_cast<float>(y) } }
{
}

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

void diji::GameObject::Start()
{
    if (!m_IsActive) return;
    
    for (const auto& component : m_ComponentsPtrVec)
    {
        component->Start();
    }

    m_IsInitialized = true;
}

void diji::GameObject::FixedUpdate() const
{
    if (!m_IsActive) return;

    if (m_SimulatesPhysics)
        m_ColliderCompPtr->SyncTransform();
    
    for (const auto& component : m_ComponentsPtrVec)
    {
        component->FixedUpdate();
    }
}

void diji::GameObject::Update() const
{
    if (!m_IsActive) return;

    if (m_SimulatesPhysics)
        m_ColliderCompPtr->SyncTransform();
    
    for (const auto& component : m_ComponentsPtrVec)
    {
        component->Update();
    }
}

void diji::GameObject::LateUpdate() const
{
    if (!m_IsActive) return;

    for (const auto& component : m_ComponentsPtrVec)
    {
        component->LateUpdate();
    }
}

void diji::GameObject::Render() const
{
    if (!m_IsActive) return;

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
    // Call component OnDestroy methods
    for (auto& component : m_ComponentsPtrVec)
    {
        component->OnDestroy();
    }

    // Remove all event listeners for all components
    for (auto& component : m_ComponentsPtrVec)
    {
        EventRegistry::GetInstance().RemoveAllListenersForObject(component.get());
    }
}

void diji::GameObject::SetActive(const bool isActive)
{
    m_IsActive = isActive;

    if (isActive)
    {
        OnEnable();
        
        if (!m_IsInitialized)
            Start();
    }
    else
        OnDisable();

    if (m_ColliderCompPtr)
        m_ColliderCompPtr->SetActive(isActive);
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
    
    duplicate->SetObjectPosition(GetObjectPosition());
}

void diji::GameObject::NotifyTriggerEvent(const Collider* other, const EventType& eventType, const CollisionInfo& hitInfo) const
{
    for (const auto& component : m_ComponentsPtrVec)
    {
        switch (eventType)
        {
        case EventType::Enter:
            component->OnTriggerEnter(other, hitInfo);
            break;
        case EventType::Stay:
            component->OnTriggerStay(other, hitInfo);
            break;
        case EventType::Exit:
            component->OnTriggerExit(other, hitInfo);
            break;
        case EventType::Hit:
            component->OnHitEvent(other, hitInfo);
            break;
        default:  // NOLINT(clang-diagnostic-covered-switch-default)
            throw std::exception("Unknown trigger event");
        }
    }
}

std::optional<sf::FloatRect> diji::GameObject::GetBoundingBox() const
{
    if (m_RenderCompPtr)
        return m_RenderCompPtr->GetBoundingBox();

    return std::nullopt;
}

sf::Vector2f diji::GameObject::GetObjectPosition() const
{
    return m_RootTransform->GetWorldPosition();
}

void diji::GameObject::SetObjectPosition(const sf::Vector2f& position) const
{
    m_RootTransform->SetWorldPosition(position);
}

sf::Angle diji::GameObject::GetObjectRotation() const
{
    return m_RootTransform->GetWorldRotation();
}

void diji::GameObject::SetObjectRotation(const sf::Angle& rotation) const
{
    m_RootTransform->SetWorldRotation(rotation);
}

sf::Vector2f diji::GameObject::GetObjectScale2D() const
{
    return m_RootTransform->GetWorldScale2D();
}

void diji::GameObject::SetObjectScale2D(const sf::Vector2f& scale) const
{
    m_RootTransform->SetWorldScale2D(scale);
}

void diji::GameObject::AddObjectWorldOffset(const sf::Vector2f& pos) const
{
    SetObjectPosition(GetObjectPosition() + pos);
}

void diji::GameObject::AddObjectWorldRotation(const sf::Angle& rot) const
{
    SetObjectRotation(GetObjectRotation() + rot);
}

void diji::GameObject::AddObjectLocalOffset(const sf::Vector2f& offset) const
{
    m_RootTransform->SetLocalPosition(m_RootTransform->GetLocalPosition() + offset);
}

void diji::GameObject::AddObjectLocalRotation(const sf::Angle& rotOffset) const
{
    m_RootTransform->SetLocalRotation(m_RootTransform->GetLocalRotation() + rotOffset);
}

void diji::GameObject::AttachToObject(const GameObject* parent, const bool keepWorldPosition) const
{
    if (!parent || parent == this)
        throw std::invalid_argument("A GameObject cannot be parented to itself.");

    m_RootTransform->AttachToObject(parent->m_RootTransform.get(), keepWorldPosition);
}

void diji::GameObject::DetachFromObject(const bool keepWorldPosition) const
{
    m_RootTransform->DetachFromObject(keepWorldPosition);
}