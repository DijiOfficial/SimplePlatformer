#pragma once
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
        explicit Render(GameObject* ownerPtr, int scale);
        ~Render() noexcept override = default;

        Render(const Render& other) = delete;
        Render(Render&& other) = delete;
        Render& operator=(const Render& other) = delete;
        Render& operator=(Render&& other) = delete;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}
        
        virtual void RenderFrame() const;
        virtual void UpdateTexture(sf::Texture& texture);
        void DisableRender() { m_Render = false; }
        void EnableRender() { m_Render = true; }
        
    protected:
        int m_Scale = 1;
        bool m_Render = true;
        
    private:
        Transform* m_TransformCompPtr;
        sf::Texture m_SFMLTexture;
        TextureComp* m_TextureCompPtr;
        Sprite* m_SpriteCompPtr;
        TextComp* m_TextCompPtr;
    };
}
