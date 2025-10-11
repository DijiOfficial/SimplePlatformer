#include "TimerScript.h"

#include "PlayerCharacter.h"
#include "Engine/Components/ScoreCounter.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/SceneManager.h"

void thomasWasLate::TimerScript::Init()
{
    m_ScoreCounterCompPtr = GetOwner()->GetComponent<diji::ScoreCounter>();

    m_Timer = m_TickTime;
    m_ScoreCounterCompPtr->SetGoalScore(0);

    diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar")->GetComponent<PlayerCharacter>()->OnHitByEnemyEvent.AddListener([this]()
    {
        m_Paused = true;
    });

    diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar")->GetComponent<PlayerCharacter>()->OnFallingInHoleEvent.AddListener([this]()
    {
        m_Paused = true;
    });
}

void thomasWasLate::TimerScript::Update()
{
    if (m_Paused) return;
    
    m_Timer -= m_TimeSingletonInstance.GetDeltaTime();

    if (m_Timer <= 0)
    {
        m_Timer += m_TickTime;
        m_ScoreCounterCompPtr->DecreaseScore();
    }
}
