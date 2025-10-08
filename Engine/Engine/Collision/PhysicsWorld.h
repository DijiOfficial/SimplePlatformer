#pragma once
#include <vector>
#include <SFML/Graphics/Rect.hpp>
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
		void SetGravity(const sf::Vector2f& gravity) { m_Gravity = gravity; }
		[[nodiscard]] sf::Vector2f GetGravity() const { return m_Gravity; }
		
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

		struct Prediction
		{
			Collider* collider;
			sf::FloatRect AABB;
			sf::Vector2f pos;
			sf::Vector2f vel;
			std::vector<CollisionInfo> collisionInfoVec;
		};
		
		static float Right(const sf::FloatRect& r)  { return r.left + r.width; }
		static float Bottom(const sf::FloatRect& r) { return r.top  + r.height; }
	
	private:
		std::vector<Collider*> m_DynamicColliders;
		std::vector<StaticColliderInfo> m_StaticInfos;
		
		sf::Vector2f m_Gravity{ 0.f, 980.f }; // This doesn't need to be a vector unless we want to simulate planets or some shit
		const TimeSingleton& m_TimeSingletonInstance = TimeSingleton::GetInstance();

		// physics trigger events
		struct TriggerPair
		{
			Collider* trigger;
			Collider* other;
            
			bool operator==(const TriggerPair& rhs) const
			{
				return (trigger == rhs.trigger && other == rhs.other) || (trigger == rhs.other && other == rhs.trigger);
			}
		};

		enum class EventType
		{
			Enter = 0,
			Stay = 1,
			Exit = 2
		};
		
		std::vector<TriggerPair> m_ActiveTriggers;
		std::vector<TriggerPair> m_PreviousFrameTriggers;
		void ProcessTriggerEvents();
		static void NotifyTriggerEvent(const TriggerPair& trigger, EventType eventType);
		
		// Collision detection/resolution
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
		static bool HandleStaticCollisions(Prediction& dynamicCollider, const Collider* staticCollider);
		// void HanldeDynamicCollisions(const sf::FloatRect& rectA, const sf::FloatRect& rectB) const;
	};
}

