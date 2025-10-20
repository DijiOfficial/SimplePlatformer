#pragma once
#include "Engine/Components/Component.h"
#include "Engine/Singleton/TimeSingleton.h"

namespace diji
{
    class ScoreCounter;
}

namespace thomasWasLate
{
    class TimerScript final : public diji::Component
    {
    public:
        explicit TimerScript(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~TimerScript() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override;
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

    private:
        const diji::TimeSingleton& m_TimeSingletonInstance = diji::TimeSingleton::GetInstance();
        diji::ScoreCounter* m_ScoreCounterCompPtr = nullptr;
        bool m_Paused = false;

        float m_Timer = 0;
        float m_TickTime = 0.4f;
    };
}
