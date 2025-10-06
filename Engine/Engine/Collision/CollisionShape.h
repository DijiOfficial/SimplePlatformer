#pragma once
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Shape.hpp>

#include "PhysicsWorld.h"

namespace diji
{
    class PhysicsWorld;
    struct PhysicsWorld::StaticColliderInfo;
    
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
        // I tried using template but my brain fried
        // [[nodiscard]] virtual const sf::Shape& GetPredictedShape(const sf::Vector2f& pos) const = 0;
        [[nodiscard]] virtual const sf::Shape& GetShape() const = 0;
        [[nodiscard]] virtual sf::FloatRect GetLocalShapeBounds() const = 0;
        virtual void SetPosition(const sf::Vector2f& pos) = 0;
        virtual void SetRotation(float angleDeg) = 0;
        
        void UpdateAABB(const sf::Vector2f& pos) // todo: not necessary?
        {
            m_AABB = GetLocalShapeBounds();
            m_AABB.left = pos.x;
            m_AABB.top = pos.y;
        }

        void UpdateAABB(const float x, const float y) { UpdateAABB(sf::Vector2f{x, y}); }

        virtual void CollideWith(std::vector<PhysicsWorld::CollisionInfo>& collisionsVec, const PhysicsWorld::StaticColliderInfo& info, const sf::Vector2f& pos) = 0;
        virtual void CollideWith(std::vector<PhysicsWorld::CollisionInfo>& collisionsVec, const sf::FloatRect& staticAABB, const sf::FloatRect& predictedAABB) = 0;

        virtual void HandleStaticCollisionWithRect(std::vector<PhysicsWorld::CollisionInfo>& collisionsVec, const sf::RectangleShape& movingShape, const sf::Vector2f& predictedPos, const sf::RectangleShape& staticShape) = 0;
        virtual void HandleStaticCollisionWithCircle(class Circle& circle, PhysicsWorld::StaticColliderInfo const& info) = 0;
        virtual void HandleStaticCollisionWithTriangle(class Triangle& triangle, PhysicsWorld::StaticColliderInfo const& info) = 0;
        
    protected:
        explicit CollisionShape(const sf::FloatRect& aabb) noexcept : m_AABB(aabb) {}
        sf::FloatRect m_AABB;

        // todo: look into optimizing these functions
        static std::vector<sf::Vector2f> GetCorners(const sf::RectangleShape& rect);
        static std::vector<sf::Vector2f> GetAxes(const std::vector<sf::Vector2f>& corners);
        static void ProjectOntoAxis(const std::vector<sf::Vector2f>& points, const sf::Vector2f& axis, float& min, float& max);
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

        [[nodiscard]] const sf::Shape& GetShape() const override { return m_Circle; }
        [[nodiscard]] sf::FloatRect GetLocalShapeBounds() const override { return m_Circle.getGlobalBounds(); }
        void CollideWith(std::vector<PhysicsWorld::CollisionInfo>&, const PhysicsWorld::StaticColliderInfo&, const sf::Vector2f&) override {}
        void CollideWith(std::vector<PhysicsWorld::CollisionInfo>&, const sf::FloatRect&, const sf::FloatRect&) override {}

        void SetPosition(const sf::Vector2f& pos) override { m_Circle.setPosition(pos); }
        void SetRotation(const float angleDeg) override { m_Circle.setRotation(angleDeg); }

    protected:
        void HandleStaticCollisionWithRect(std::vector<PhysicsWorld::CollisionInfo>&, const sf::RectangleShape&, const sf::Vector2f&, const sf::RectangleShape&) override {}
        void HandleStaticCollisionWithCircle(Circle&, const PhysicsWorld::StaticColliderInfo&) override {}
        void HandleStaticCollisionWithTriangle(Triangle&, const PhysicsWorld::StaticColliderInfo&) override {}


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

        [[nodiscard]] const sf::Shape& GetShape() const override { return m_Rect; }
        [[nodiscard]] sf::FloatRect GetLocalShapeBounds() const override { return m_Rect.getGlobalBounds(); }
        void CollideWith(std::vector<PhysicsWorld::CollisionInfo>& collisionsVec, const PhysicsWorld::StaticColliderInfo& info, const sf::Vector2f& pos) override;
        void CollideWith(std::vector<PhysicsWorld::CollisionInfo>& collisionsVec, const sf::FloatRect& staticAABB, const sf::FloatRect& predictedAABB) override;

        void HandleStaticCollisionWithRect(std::vector<PhysicsWorld::CollisionInfo>& collisionsVec, const sf::RectangleShape& movingShape, const sf::Vector2f& predictedPos, const sf::RectangleShape& staticShape) override;
        void HandleStaticCollisionWithCircle(Circle&, const PhysicsWorld::StaticColliderInfo&) override;
        void HandleStaticCollisionWithTriangle(Triangle&, const PhysicsWorld::StaticColliderInfo&) override;
        void SetPosition(const sf::Vector2f& pos) override { m_Rect.setPosition(pos); }
        void SetRotation(const float angleDeg) override { m_Rect.setRotation(angleDeg); }

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

        [[nodiscard]] const sf::Shape& GetShape() const override { return m_Triangle; }
        [[nodiscard]] sf::FloatRect GetLocalShapeBounds() const override { return m_Triangle.getGlobalBounds(); }

        void CollideWith(std::vector<PhysicsWorld::CollisionInfo>&, const PhysicsWorld::StaticColliderInfo&, const sf::Vector2f&) override {}
        void CollideWith(std::vector<PhysicsWorld::CollisionInfo>&, const sf::FloatRect&, const sf::FloatRect&) override {}

        void HandleStaticCollisionWithRect(std::vector<PhysicsWorld::CollisionInfo>&, const sf::RectangleShape&, const sf::Vector2f&, const sf::RectangleShape&) override {}
        void HandleStaticCollisionWithCircle(Circle&, const PhysicsWorld::StaticColliderInfo&) override {}
        void HandleStaticCollisionWithTriangle(Triangle&, const PhysicsWorld::StaticColliderInfo&) override {}
        void SetPosition(const sf::Vector2f& pos) override { m_Triangle.setPosition(pos); }
        void SetRotation(const float angleDeg) override { m_Triangle.setRotation(angleDeg); }

    private:
        sf::ConvexShape m_Triangle;
    };
}
