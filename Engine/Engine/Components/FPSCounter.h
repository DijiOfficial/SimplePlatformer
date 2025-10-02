#pragma once
#include "Component.h"

namespace diji
{
    class TextComp;
    class FPSCounter final : public Component
    {
    public:
        explicit FPSCounter(GameObject* ownerPtr);
        ~FPSCounter() noexcept override = default;

        FPSCounter(const FPSCounter& other) = delete;
        FPSCounter(FPSCounter&& other) = delete;
        FPSCounter& operator=(const FPSCounter& other) = delete;
        FPSCounter& operator=(FPSCounter&& other) = delete;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}

        void FixedUpdate() override {}
        void Update() override;
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}
        float GetFPS() const { return m_Fps; }

    private:
        const float REFRESH_RATE = 0.1f;
        TextComp* m_TextComponentPtr;
        int m_FrameCount;
        float m_Fps;
        float m_ElapsedTime;
    };
}

