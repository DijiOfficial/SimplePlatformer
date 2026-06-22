#pragma once
#include "../Components/Component.h"

namespace diji
{
    class Render;

    class Blinking final : public Component
    {
    public:
        explicit Blinking(GameObject* ownerPtr) : Component{ ownerPtr } {}
        explicit Blinking(GameObject* ownerPtr, const float offScreenTime, const float onScreenTime) : Component{ ownerPtr }, m_OffScreenTime{ offScreenTime }, m_OnScreenTime{ onScreenTime } {}
        ~Blinking() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}

        void FixedUpdate() override {}
        void Update() override;
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}
        void SetOffScreenTime(const float time) { m_OffScreenTime = time; }
        void SetOnScreenTime(const float time) { m_OnScreenTime = time; }
        void InitializeRenderComp();
        
    private:
        Render* m_RenderCompPtr = nullptr;
        float m_OffScreenTime = 0.5f;
        float m_OnScreenTime = 0.5f;
        float m_ElapsedTime = 0.0f;
    };
}
