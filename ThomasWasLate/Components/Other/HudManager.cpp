#include "HudManager.h"

#include "../../Singletons/GameManager.h"
#include "../Player/PlayerCharacter.h"
#include "Engine/Components/ScoreCounter.h"
#include "Engine/Singleton/SceneManager.h"

thomasWasLate::HudManager::HudManager(diji::GameObject* ownerPtr, diji::ScoreCounter* score, diji::ScoreCounter* coins, diji::ScoreCounter* timer)
    : Component{ ownerPtr }
    , m_ScoreCounterCompPtr{ score }
    , m_CoinsCounterCompPtr{ coins }
    , m_TimerCounterCompPtr{ timer }
{
}

void thomasWasLate::HudManager::Init()
{
    const auto player = diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar")->GetComponent<PlayerCharacter>();

    player->OnHitByEnemyEvent.AddListener(this, &HudManager::SaveScoreAndCoins);
    player->OnFallingInHoleEvent.AddListener(this, &HudManager::SaveScoreAndCoins);
    player->OnCastleReachedEvent.AddListener(this, &HudManager::SaveScoreAndCoins);
    player->OnCastleReachedEvent.AddListener(this, &HudManager::StartTimerPointCount);
    
    m_CoinsCounterCompPtr->OnGivenScoreReachedEvent.AddListener(this, &HudManager::Handle100CoinsCollected);
}

void thomasWasLate::HudManager::FixedUpdate()
{
    if (!m_IsCountingTimerPoints) return;

    const int totalScore = m_TimerCounterCompPtr->GetScore();
    if (totalScore <= 0)
    {
        m_IsCountingTimerPoints = false;
        OnScoreCountedEvent.Broadcast(m_FireWorksToSpawn);
        return;
    }

    m_TimerCounterCompPtr->DecreaseScore();
    GameManager::GetInstance().OnScoreAddedEvent.Broadcast(50);
}

void thomasWasLate::HudManager::Handle100CoinsCollected() const
{
    m_CoinsCounterCompPtr->Reset();
    GameManager::GetInstance().AddLife();
}

void thomasWasLate::HudManager::SaveScoreAndCoins() const
{
    GameManager::GetInstance().SaveCoins(m_CoinsCounterCompPtr->GetScore());
    GameManager::GetInstance().SaveScore(m_ScoreCounterCompPtr->GetScore());
}

void thomasWasLate::HudManager::StartTimerPointCount()
{
    const int totalScore = m_TimerCounterCompPtr->GetScore();
    switch (totalScore % 10)
    {
    case 1:
        m_FireWorksToSpawn = 1;
        break;
    case 3:
        m_FireWorksToSpawn = 3;
        break;
    case 6:
        m_FireWorksToSpawn = 6;
        break;
    default:
        m_FireWorksToSpawn = 0;
        break;
    }

    m_IsCountingTimerPoints = true;
}

