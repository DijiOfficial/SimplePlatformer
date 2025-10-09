#pragma once
#include "Engine/Components/Render.h"

namespace thomasWasLate
{
    class CustomBackgroundRenderer final : public diji::Render
    {
    public:
        explicit CustomBackgroundRenderer(diji::GameObject* ownerPtr) : Render(ownerPtr) {}
        ~CustomBackgroundRenderer() noexcept override = default;
        
        CustomBackgroundRenderer(const CustomBackgroundRenderer& other) = delete;
        CustomBackgroundRenderer(CustomBackgroundRenderer&& other) = delete;
        CustomBackgroundRenderer& operator=(const CustomBackgroundRenderer& other) = delete;
        CustomBackgroundRenderer& operator=(CustomBackgroundRenderer&& other) = delete;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}
        
        void RenderFrame() const override;

    private:
        diji::TextureComp* m_TextureCompPtr = nullptr;
        diji::Transform* m_PlayerTransformPtr = nullptr;
        diji::Transform* m_TransformCompPtr = nullptr;
        sf::Texture m_SFMLTexture;

        float m_Width = 0;
    };
}
