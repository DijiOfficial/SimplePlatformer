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
        explicit HudManager(diji::GameObject* ownerPtr, diji::ScoreCounter* score, diji::ScoreCounter* coins);
        ~HudManager() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}
        
    private:
        diji::ScoreCounter* m_ScoreCounterCompPtr = nullptr;
        diji::ScoreCounter* m_CoinsCounterCompPtr = nullptr;

        void Handle100CoinsCollected() const;
        void SaveScoreAndCoins() const;
    };
}
