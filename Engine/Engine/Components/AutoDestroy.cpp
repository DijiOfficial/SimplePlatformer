#include "AutoDestroy.h"

#include "../Singleton/SceneManager.h"
#include "../Singleton/TimerManager.h"
#include "../Core/GameObject.h"

void diji::AutoDestroy::Init()
{
    (void)TimerManager::GetInstance().SetTimer([&]
    {
        GetOwner()->Destroy();
    }, m_LifeTime, false);
}
