#include "ScoreCounter.h"
#include "../Core/GameObject.h"
#include "TextComp.h"

diji::ScoreCounter::ScoreCounter(GameObject* ownerPtr, const int score, const bool shouldCallEvent)
    : Component(ownerPtr)
    , m_Score{ score }
    , m_StartingScore{ score }
    , m_ShouldCallEvent{ shouldCallEvent }
    , m_TextCompPtr{ nullptr }
{
}

void diji::ScoreCounter::Init()
{
    m_TextCompPtr = GetOwner()->GetComponent<TextComp>();
}

void diji::ScoreCounter::Start()
{
    ScoreChangeCheck(true);
}

void diji::ScoreCounter::IncreaseScore(bool)
{
    ++m_Score;

    OnScoreIncreasedEvent.Broadcast(m_Score);
    ScoreChangeCheck();
}

void diji::ScoreCounter::IncreaseScore(const int score)
{
    m_Score += score;
    
    OnScoreIncreasedEvent.Broadcast(m_Score);
    ScoreChangeCheck();
}
void diji::ScoreCounter::IncreaseScore()
{
    m_Score += m_ScoreIncrease;

    OnScoreIncreasedEvent.Broadcast(m_Score);
    ScoreChangeCheck();
}

void diji::ScoreCounter::DecreaseScore(const int score)
{
    m_Score -= score;
    OnScoreDecreasedEvent.Broadcast(m_Score);
    ScoreChangeCheck();
}

void diji::ScoreCounter::DecreaseScore()
{
    --m_Score;
    OnScoreDecreasedEvent.Broadcast(m_Score);
    ScoreChangeCheck();
}

void diji::ScoreCounter::Reset()
{
    m_Score = m_StartingScore;
    if (m_ShouldCallEvent)
    {
        OnScoreIncreasedEvent.Broadcast(m_Score);
        OnScoreDecreasedEvent.Broadcast(m_Score);
    }

    ScoreChangeCheck();
}

void diji::ScoreCounter::ScoreChangeCheck(const bool isInitialCheck)
{
    const std::string newScore = m_StringScore + std::to_string(m_Score);
    m_TextCompPtr->GetText().setString(newScore);
    
    if (m_Score == m_GoalScore && !isInitialCheck)
    {
        OnGivenScoreReachedEvent.Broadcast();
    }
}
