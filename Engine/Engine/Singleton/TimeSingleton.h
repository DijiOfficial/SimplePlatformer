#pragma once
#include "Singleton.h"

#include <algorithm>

namespace diji
{
    class TimeSingleton final : public Singleton<TimeSingleton>
    {
    public:
        [[nodiscard]] float GetDeltaTime() const { return m_DeltaTime; }
        void SetDeltaTime(const float deltaTime) { m_DeltaTime = deltaTime; }

        [[nodiscard]] float GetFixedUpdateDeltaTime() const { return m_FixedTimeStep; }
        void SetFixedUpdateDeltaTime(const float fixedTimeStep) { m_FixedTimeStep = fixedTimeStep; }

        [[nodiscard]] float GetFixedTimeAlpha() const { return m_FixedTimeAlpha; }
        void SetFixedTimeAlpha(const float alpha) { m_FixedTimeAlpha = alpha; }

        void StartGlobalClock() { m_GlobalTime = 0.0; m_IsGlobalClockRunning = true; }
        void StopGlobalClock() { m_GlobalTime = 0.0; m_IsGlobalClockRunning = false; }
        void PauseGlobalClock() { m_IsGlobalClockRunning = false; }
        void ResumeGlobalClock() { m_IsGlobalClockRunning = true; }
        void ResetGlobalClock() { m_GlobalTime = 0.0; }
        void TickGlobalClock(const float deltaTime)
        {
            if (m_IsGlobalClockRunning == false || deltaTime <= 0.0f) return;

            m_GlobalTime += static_cast<double>(deltaTime) * static_cast<double>(m_GlobalTimeScale);
        }

        [[nodiscard]] bool IsGlobalClockRunning() const { return m_IsGlobalClockRunning; }
        [[nodiscard]] double GetGlobalTime() const { return m_GlobalTime; }

        [[nodiscard]] float GetGlobalTimeScale() const { return m_GlobalTimeScale; }
        void SetGlobalTimeScale(const float scale) { m_GlobalTimeScale = std::max(scale, 0.0f); }

    private:
        float m_DeltaTime = 0.0f;
        float m_FixedTimeStep = 0.0f;
        float m_FixedTimeAlpha = 0.0f;

        double m_GlobalTime = 0.0;
        float m_GlobalTimeScale = 1.0f;
        bool m_IsGlobalClockRunning = true;
    };
}