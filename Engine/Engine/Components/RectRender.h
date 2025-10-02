#pragma once
#include <SFML/Graphics/RectangleShape.hpp>

#include "Render.h"

namespace diji
{
    // todo: this class is kind of redundant.
    class RectRender final : public Render
    {
    public:
        using Render::Render;
        explicit RectRender(GameObject* ownerPtr, bool isDebug);
        ~RectRender() noexcept override = default;

        void Init() override;
        void RenderFrame() const override;
        void Update() override;

        void SetRectangle(const sf::RectangleShape& rectangle) const { m_RectangleShape = rectangle; }
        void SetFillColor(const sf::Color& color) { m_FillColor = color; }
        void SetOutlineColor(const sf::Color& outlineColor) { m_OutlineColor = outlineColor; }
        void SetLineWidth(const float width) { m_LineWidth = width; }
        
        sf::RectangleShape& GetRectangle() const { return m_RectangleShape; }
        const sf::Color& GetColor() const { return m_FillColor; }
        const sf::Color& GetOutlineColor() const { return m_OutlineColor; }
        float GetLineWidth() const { return m_LineWidth; }
        void SetDebugDraw() { m_IsDebug = true; }
        
    private:
        Transform* m_TransformCompPtr = nullptr;
        mutable sf::RectangleShape m_RectangleShape;
        sf::Color m_OutlineColor = sf::Color::White;
        sf::Color m_FillColor = sf::Color::Transparent;
        float m_LineWidth = 1.f;
        bool m_IsDebug = false;
        bool m_NeedsUpdate = false;

        void SetDrawCollision();
    };

}

