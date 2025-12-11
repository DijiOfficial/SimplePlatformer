#include "MultiCoinBlock.h"
#include "Engine/Interfaces/Timeline.h"

void superMarioBros::MultiCoinBlock::Init()
{
    BaseBlock::Init();

    m_ShouldSwitchOnHit = false;
}

void superMarioBros::MultiCoinBlock::OnAnimationStart()
{
    if (!m_CanBeHit) return;

    BaseBlock::OnAnimationStart();
    diji::TimerManager::GetInstance().ClearTimer(m_TimerHandle);

    --m_CoinsLeft;
    if (m_CoinsLeft <= 0 || m_IsLastHit)
    {
        m_ShouldSwitchOnHit = true;
        return;
    }

    m_CanBeHit = false;
    (void)diji::TimerManager::GetInstance().SetTimer([&]
    {
        m_CanBeHit = true;
    }, 0.067f, false);
}

void superMarioBros::MultiCoinBlock::OnAnimationEnd()
{
    m_TimerHandle = diji::TimerManager::GetInstance().SetTimer([&]()
    {
        m_IsLastHit = true;
    }, 1.f, false);
}
