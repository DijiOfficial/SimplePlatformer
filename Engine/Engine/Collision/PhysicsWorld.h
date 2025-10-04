#pragma once
#include <vector>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/System/Vector2.hpp>

#include "../Singleton/TimeSingleton.h"

namespace diji
{
	class Collider;

	class PhysicsWorld final
	{
	public:
		PhysicsWorld() = default;
		~PhysicsWorld() noexcept = default;

		PhysicsWorld(const PhysicsWorld& other) = delete;
		PhysicsWorld(PhysicsWorld&& other) = delete;
		PhysicsWorld& operator=(const PhysicsWorld& other) = delete;
		PhysicsWorld& operator=(PhysicsWorld&& other) = delete;
		
		void Reset();
		void AddCollider(Collider* collider);
		void RemoveCollider(Collider* collider);
		void FixedUpdate();
		
		struct StaticColliderInfo
		{
			sf::FloatRect aabb;
			Collider* collider; // perhaps this can be optimized for memory usage if needed
		};

		struct CollisionInfo
		{
			sf::Vector2f point;          // Contact point
			sf::Vector2f normal;         // Collision normal (pointing away from surface)
			float penetration;           // How much objects overlap
			mutable float normalImpulse; // Impulse magnitude (calculated during resolution)
			sf::Vector2f tangent;        // Tangent vector for friction calculation
			bool hasCollision;           // Whether collision occurred
		};
		
		static float Right(const sf::FloatRect& r)  { return r.left + r.width; }
		static float Bottom(const sf::FloatRect& r) { return r.top  + r.height; }
	
	private:
		struct Prediction
		{
			Collider* collider;
			sf::FloatRect AABB;
			sf::Vector2f pos;
			sf::Vector2f vel;
			std::vector<CollisionInfo> collisionInfoVec;
		};
		
		std::vector<Collider*> m_DynamicColliders;
		std::vector<StaticColliderInfo> m_StaticInfos;
		
		sf::Vector2f m_Gravity{0.f, 980.f}; // This doesn't need to be a vector unless we want to simulate planets or some shit
		const TimeSingleton& m_TimeSingletonInstance = TimeSingleton::GetInstance();

		static bool AABBOverlap(const sf::FloatRect& a, const sf::FloatRect& b)
		{
			return !(Right(a) <= b.left ||
					 Right(b) <= a.left ||
					 Bottom(a) <= b.top ||
					 Bottom(b) <= a.top);
		}
		
		void PredictMovement(std::vector<Prediction>& predictionsVec) const;
		void DetectCollisions(std::vector<Prediction>& predictionsVec);

		static void ResolveCollision(Prediction& prediction, const CollisionInfo& collision);
		void ApplyFriction(Prediction& prediction, const CollisionInfo& collision) const;

		static void UpdateFinalPosition(const Prediction& prediction);
		void HandleStaticCollisions(const sf::FloatRect& dynamicRect, const sf::FloatRect& staticRect) const;
	};
}

