#include "Component.h"

#include "../Singleton/SceneManager.h"
#include "../Singleton/TimerManager.h"
#include "../Core/GameObject.h"

diji::Component::~Component()
{
    if (m_TimerHandles.empty())
        return;

    const auto handles = m_TimerHandles;
    for (size_t id : handles)
    {
        TimerManager::GetInstance().ClearTimer({id});
    }
}

void diji::Component::SetActive(const bool isActive) const
{
    m_OwnerPtr->SetActive(isActive);
}

void diji::Component::Destroy() const
{
    GetOwner()->Destroy();
}

void diji::Component::Destroy(const float lifeTime) const
{
    (void)TimerManager::GetInstance().SetTimer(this, [&]
    {
        Destroy();
    }, lifeTime, false);
}
