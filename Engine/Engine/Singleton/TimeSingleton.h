#pragma once
#include "Singleton.h"

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

    private:
        float m_DeltaTime = 0.0f;
        float m_FixedTimeStep = .0f;
        float m_FixedTimeAlpha = 0.0f;
    };
}