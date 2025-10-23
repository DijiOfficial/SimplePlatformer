#include "HudManager.h"

#include "../../Singletons/GameManager.h"
#include "../Player/PlayerCharacter.h"
#include "Engine/Components/ScoreCounter.h"
#include "Engine/Singleton/SceneManager.h"

thomasWasLate::HudManager::HudManager(diji::GameObject* ownerPtr, diji::ScoreCounter* score, diji::ScoreCounter* coins)
    : Component{ ownerPtr }
    , m_ScoreCounterCompPtr{ score }
    , m_CoinsCounterCompPtr{ coins }
{
}

void thomasWasLate::HudManager::Init()
{
    diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar")->GetComponent<PlayerCharacter>()->OnHitByEnemyEvent.AddListener(this, &HudManager::SaveScoreAndCoins);
    diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar")->GetComponent<PlayerCharacter>()->OnCastleReachedEvent.AddListener(this, &HudManager::SaveScoreAndCoins);
    m_CoinsCounterCompPtr->OnGivenScoreReachedEvent.AddListener(this, &HudManager::Handle100CoinsCollected);
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
