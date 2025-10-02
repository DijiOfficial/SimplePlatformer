#pragma once
#include <string>

#include "Component.h"

namespace diji
{
    class TextComp;

    // todo: rename to more general counter display name
    class ScoreCounter : public Component
    {
    public:
        explicit ScoreCounter(GameObject* ownerPtr, int score = 0, bool shouldCallEvent = false);
        ~ScoreCounter() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override;
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}
        
        void IncreaseScore([[maybe_unused]] bool isLeft = false); // todo: why the bool?
        void IncreaseScore(int score = 1);
        void IncreaseScore();
        void DecreaseScore(int score = 1);
        void DecreaseScore();
        [[nodiscard]] int GetScore() const { return m_Score; }
        void SetString(const std::string& str) { m_StringScore = str; }
        void SetGoalScore(const int goalScore) { m_GoalScore = goalScore; }
        void Reset();
        void SetScoreIncreaseIncrement(const int increment) { m_ScoreIncrease = increment; }

        Event<int> OnScoreIncreasedEvent;
        Event<int> OnScoreDecreasedEvent;
        Event<> OnGivenScoreReachedEvent;

    protected:
        int m_Score;
        void ScoreChangeCheck(bool isInitialCheck = false);

    private:
        const int m_StartingScore = 0;
        bool m_ShouldCallEvent = false;
        int m_GoalScore = 0;
        int m_ScoreIncrease = 1;
        std::string m_StringScore = "Score = ";
        
        TextComp* m_TextCompPtr;
    };
}

