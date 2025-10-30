#pragma once
#include "Engine/Components/Component.h"

namespace diji
{
    class ScoreCounter;
}

namespace thomasWasLate
{
    class HudManager final : public diji::Component
    {
    public:
        explicit HudManager(diji::GameObject* ownerPtr, diji::ScoreCounter* score, diji::ScoreCounter* coins, diji::ScoreCounter* timer);
        ~HudManager() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override {}
        void FixedUpdate() override;
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        diji::Event<int> OnScoreCountedEvent;
        
    private:
        diji::ScoreCounter* m_ScoreCounterCompPtr = nullptr;
        diji::ScoreCounter* m_CoinsCounterCompPtr = nullptr;
        diji::ScoreCounter* m_TimerCounterCompPtr = nullptr;
        bool m_IsCountingTimerPoints = false;
        int m_FireWorksToSpawn = 0;

        void Handle100CoinsCollected() const;
        void SaveScoreAndCoins() const;
        void StartTimerPointCount();
    };
}
