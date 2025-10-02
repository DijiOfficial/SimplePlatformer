#pragma once
#include <map>
#include <vector>
#include <SFML/System/Vector2.hpp>

namespace diji
{
	class Collider;
	enum Rectf;

	class PhysicsWorld final
	{
	public:
		PhysicsWorld() = default;
		~PhysicsWorld() noexcept = default;

		PhysicsWorld(const PhysicsWorld& other) = delete;
		PhysicsWorld(PhysicsWorld&& other) = delete;
		PhysicsWorld& operator=(const PhysicsWorld& other) = delete;
		PhysicsWorld& operator=(PhysicsWorld&& other) = delete;
		
		// void Reset();
		void AddCollider(const Collider* object);
		void RemoveCollider(const Collider* object);
		// void AddLevelCollider(const std::vector<sf::Vector2f>& vertices) { m_LevelCollider.emplace_back(vertices); }
		// void ParseRectInLevelCollider(const Rectf& rect);
		//
		// void UpdateCollider(const Collider* object, const Rectf& collider);

	private:
		std::vector<const Collider*> m_Colliders;
		std::vector<std::vector<sf::Vector2f>> m_LevelCollider;

		// [[nodiscard]] float Distance(const sf::Vector2f& a, const sf::Vector2f& b) const;
		// [[nodiscard]] float DistanceSquared(const sf::Vector2f& a, const sf::Vector2f& b) const;
		//
		// // taken from Prog2 Engine credits to Koen Samyn
		// [[nodiscard]] bool Raycast(const std::vector<sf::Vector2f>& vertices, const sf::Vector2f& rayP1, const sf::Vector2f& rayP2) const;
		// [[nodiscard]] bool Raycast(const sf::Vector2f* vertices, const size_t nrVertices, const sf::Vector2f& rayP1, const sf::Vector2f& rayP2) const;
		// [[nodiscard]] bool IntersectLineSegments(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& q1, const sf::Vector2f& q2, float& outLambda1, float& outLambda2, double epsilon = 1e-6) const;
		// [[nodiscard]] bool IsPointOnLineSegment(const sf::Vector2f& p, const sf::Vector2f& a, const sf::Vector2f& b) const;
		// [[nodiscard]] bool AreRectsColliding(const Rectf& rect1, const Rectf& rect2) const;
		//
		// [[nodiscard]] float CrossProduct(const sf::Vector2f& vec1, const sf::Vector2f& vec2) const { return vec1.x * vec2.y - vec1.y * vec2.x; }
		// [[nodiscard]] float DotProduct(const sf::Vector2f& vec1, const sf::Vector2f& vec2) const { return vec1.x * vec2.x + vec1.y * vec2.y; }
		//
		// [[nodiscard]] sf::Vector2f CreateVector(const sf::Vector2f& fromPoint, const sf::Vector2f& tillPoint) const { return sf::Vector2f{ tillPoint.x - fromPoint.x, tillPoint.y - fromPoint.y }; }
	};
}

