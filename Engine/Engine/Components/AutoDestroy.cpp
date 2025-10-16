#include "AutoDestroy.h"

#include "../Singleton/SceneManager.h"
#include "../Singleton/TimerManager.h"
#include "../Core/GameObject.h"

void diji::AutoDestroy::Init()
{
    (void)TimerManager::GetInstance().SetTimer([&]
    {
        SceneManager::GetInstance().SetPendingDestroy(GetOwner());
    }, m_LifeTime, false);
}
