#pragma once
#include <variant>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "Render.h"

namespace diji
{
    using ShapeVariant = std::variant<sf::RectangleShape, sf::CircleShape, sf::ConvexShape>;
    class ShapeRender final : public Render
    {
    public:
        using Render::Render;
        explicit ShapeRender(GameObject* ownerPtr, bool isDebug);
        ~ShapeRender() noexcept override = default;

        void Init() override;
        void RenderFrame() const override;
        void Update() override;

        void SetFillColor(const sf::Color& color) { m_FillColor = color; }
        void SetOutlineColor(const sf::Color& outlineColor) { m_OutlineColor = outlineColor; }
        void SetLineWidth(const float width) { m_LineWidth = width; }
        
        const sf::Color& GetColor() const { return m_FillColor; }
        const sf::Color& GetOutlineColor() const { return m_OutlineColor; }
        float GetLineWidth() const { return m_LineWidth; }
        void SetDebugDraw() { m_IsDebug = true; }
        
    private:
        Transform* m_TransformCompPtr = nullptr;
        ShapeVariant m_Shape;
        sf::Color m_OutlineColor = sf::Color::White;
        sf::Color m_FillColor = sf::Color::Transparent;
        float m_LineWidth = 1.f;
        bool m_IsDebug = false;
        bool m_NeedsUpdate = true;

        void SetDrawCollision();
        void ApplyStyle();
    };
}

