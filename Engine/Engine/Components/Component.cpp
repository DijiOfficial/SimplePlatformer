#include "Component.h"

#include "../Singleton/SceneManager.h"
#include "../Singleton/TimerManager.h"
#include "../Core/GameObject.h"

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
    (void)TimerManager::GetInstance().SetTimer([&]()
    {
        Destroy();
    }, lifeTime, false);
}