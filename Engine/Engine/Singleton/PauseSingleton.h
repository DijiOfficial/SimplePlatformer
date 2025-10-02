#pragma once
#include "Singleton.h"

namespace diji
{
    class PauseSingleton final : public Singleton<PauseSingleton>
    {
    public:
        bool GetIsPaused() const { return m_IsPaused; }
        void SetIsPaused(const bool isPaused) { m_IsPaused = isPaused; }
        void TogglePause() { m_IsPaused = !m_IsPaused; }

    private:
        bool m_IsPaused = false;
    };
}