#pragma once
#include "Engine/Components/Component.h"

namespace superMarioBros
{
    class SceneLoader final : public diji::Component
    {
    public:
        explicit SceneLoader(diji::GameObject* ownerPtr, const float lifeTime) : Component{ ownerPtr }, m_LifeTime{ lifeTime } {}
        ~SceneLoader() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}

        void FixedUpdate() override {}
        void Update() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void SetCallback(std::function<void()> callback) { m_Callback = std::move(callback); }
        
    private:
        float m_LifeTime;
        std::function<void()> m_Callback;
    };
}
