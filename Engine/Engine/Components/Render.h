#pragma once
#include <SFML/Graphics/Shader.hpp>

#include "Component.h"

#include <SFML/Graphics/Texture.hpp>

namespace diji
{
    class Sprite;
    class TextureComp;
    class Transform;
    class TextComp;

    // todo: should render be a separate kind of component? that goes for textures and text too.
    class Render : public Component
    {
    public:
        explicit Render(GameObject* ownerPtr);
        explicit Render(GameObject* ownerPtr, float scale);
        ~Render() noexcept override = default;

        Render(const Render& other) = delete;
        Render(Render&& other) = delete;
        Render& operator=(const Render& other) = delete;
        Render& operator=(Render&& other) = delete;

        void Init() override;
        void OnEnable() override {}
        void Start() override;
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}
        
        virtual void RenderFrame() const;
        virtual void UpdateTexture(sf::Texture& texture);
        void DisableRender() { m_Render = false; }
        void EnableRender() { m_Render = true; }
        void ToggleRendering() { m_Render = !m_Render; }
        void SetRenderWithShader(const bool renderWithShader) { m_RenderWithShader = renderWithShader; }
        void LoadShaderFromFile(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
        void SetShader(sf::Shader* shader) { m_ShaderPtr = shader; }
        [[nodiscard]] sf::Shader* GetShader() const { return m_ShaderPtr; }
        [[nodiscard]] virtual sf::Vector2f GetScaledSize() const;

        virtual void SetScale(float scale);
        
    protected:
        float m_Scale = 1;
        bool m_Render = true;
        bool m_RenderWithShader = false;
        sf::Shader* m_ShaderPtr = nullptr;
        
    private:
        Transform* m_TransformCompPtr;
        sf::Texture m_SFMLTexture;
        TextureComp* m_TextureCompPtr;
        Sprite* m_SpriteCompPtr;
        TextComp* m_TextCompPtr;
    };
}
