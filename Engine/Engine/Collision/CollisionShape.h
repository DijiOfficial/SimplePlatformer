#pragma once
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Shape.hpp>

namespace diji
{
    class CollisionShape
    {
    public:
        enum class ShapeType { CIRCLE, RECT, TRIANGLE };

        CollisionShape() noexcept = default;
        virtual ~CollisionShape() noexcept = default;

        CollisionShape(const CollisionShape&) = delete;
        CollisionShape& operator=(const CollisionShape&) = delete;
        CollisionShape(CollisionShape&&) = delete;
        CollisionShape& operator=(CollisionShape&&) = delete;

        [[nodiscard]] const sf::FloatRect& GetAABB() const { return m_AABB; }
        // [[nodiscard]] virtual ShapeType Type() const = 0;
        [[nodiscard]] virtual const sf::Shape& GetShape() const = 0;
        [[nodiscard]] virtual sf::FloatRect GetLocalShapeBounds() const = 0;

        void UpdateAABB(const sf::Vector2f& pos)
        {
            m_AABB = GetLocalShapeBounds();
            m_AABB.left = pos.x;
            m_AABB.top = pos.y;
        }

        void UpdateAABB(const float x, const float y) { UpdateAABB(sf::Vector2f{x, y}); }

    protected:
        explicit CollisionShape(const sf::FloatRect& aabb) noexcept : m_AABB(aabb) {}
        sf::FloatRect m_AABB;
    };
    
    class Circle final : public CollisionShape
    {
    public:
        explicit Circle(float radius)
            : CollisionShape(), m_Circle(radius)
        {
            // Center origin to make AABB centered by default (common for physics)
            m_Circle.setOrigin(radius, radius);
        }

        // [[nodiscard]] ShapeType Type() const override { return ShapeType::CIRCLE; }
        [[nodiscard]] const sf::Shape& GetShape() const override { return m_Circle; }
        [[nodiscard]] sf::FloatRect GetLocalShapeBounds() const override { return m_Circle.getLocalBounds(); }

    private:
        sf::CircleShape m_Circle;
    };

    class Rect final : public CollisionShape
    {
    public:
        explicit Rect(const sf::Vector2f& size)
            : CollisionShape(), m_Rect(size)
        {
            // center origin
            m_Rect.setOrigin(size.x * 0.5f, size.y * 0.5f);
        }

        // [[nodiscard]] ShapeType Type() const override { return ShapeType::RECT; }
        [[nodiscard]] const sf::Shape& GetShape() const override { return m_Rect; }
        [[nodiscard]] sf::FloatRect GetLocalShapeBounds() const override { return m_Rect.getLocalBounds(); }

    private:
        sf::RectangleShape m_Rect;
    };


    class Triangle final : public CollisionShape
    {
    public:
        // Accept 3 vertex positions in local space. The constructor will create a ConvexShape.
        explicit Triangle(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& c)
            : CollisionShape(), m_Triangle(3)
        {
            m_Triangle.setPoint(0, a);
            m_Triangle.setPoint(1, b);
            m_Triangle.setPoint(2, c);

            // Optionally set origin to centroid for easier transforms:
            const sf::Vector2f centroid{ (a.x + b.x + c.x) / 3.f, (a.y + b.y + c.y) / 3.f };
            m_Triangle.setOrigin(centroid);
        }

        // [[nodiscard]] ShapeType Type() const override { return ShapeType::TRIANGLE; }
        [[nodiscard]] const sf::Shape& GetShape() const override { return m_Triangle; }
        [[nodiscard]] sf::FloatRect GetLocalShapeBounds() const override { return m_Triangle.getLocalBounds(); }

    private:
        sf::ConvexShape m_Triangle;
    };
}
