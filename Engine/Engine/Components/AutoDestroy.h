#pragma once
#include "../Components/Component.h"

namespace diji
{
    class AutoDestroy final : public Component
    {
    public:
        explicit AutoDestroy(GameObject* ownerPtr, const float lifeTime) : Component{ ownerPtr }, m_LifeTime{ lifeTime } {}
        ~AutoDestroy() noexcept override = default;

        AutoDestroy(const AutoDestroy& other) = delete;
        AutoDestroy(AutoDestroy&& other) = delete;
        AutoDestroy& operator=(const AutoDestroy& other) = delete;
        AutoDestroy& operator=(AutoDestroy&& other) = delete;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}

        void FixedUpdate() override {}
        void Update() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}
        
    private:
        float m_LifeTime;
    };
}
