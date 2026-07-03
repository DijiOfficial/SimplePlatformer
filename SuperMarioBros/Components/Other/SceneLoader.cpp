#include "SceneLoader.h"
#include "../../Singletons/GameManager.h"

void superMarioBros::SceneLoader::Init()
{
    (void)diji::TimerManager::GetInstance().SetTimer(this, [this]
    {
        m_Callback();
    }, m_LifeTime, false);
}
