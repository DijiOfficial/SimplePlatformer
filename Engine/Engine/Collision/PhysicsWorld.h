#pragma once
#include "../Singleton/TimeSingleton.h"
#include "QuadTree.h"

#include <optional>
#include <unordered_set>
#include <SFML/System/Vector2.hpp>

namespace diji
{
	class Collider;
	class QuadTree;

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
		void LateFixedUpdate() const;
		void EndFrameUpdate();
		void SetGravity(const sf::Vector2f& gravity) { m_Gravity = gravity; }
		[[nodiscard]] sf::Vector2f GetGravity() const { return m_Gravity; }

		struct CollisionDetectionResult
		{
			bool overlap;
			bool hit;
		};

		static float Right(const sf::FloatRect& r)  { return r.position.x + r.size.x; }
		static float Left(const sf::FloatRect& r)	{ return r.position.x - r.size.x; }
		static float Bottom(const sf::FloatRect& r) { return r.position.y  + r.size.y; }
		static float Top(const sf::FloatRect& r)	{ return r.position.y  - r.size.y; }

		std::optional<RaycastHit> Raycast(const sf::Vector2f& origin, const sf::Vector2f& direction, float maxDistance, const Collider* collider = nullptr) const;

	private:
		std::vector<Collider*> m_DynamicColliders;
		std::vector<StaticColliderInfo> m_StaticInfos;
		std::unordered_set<SleepingCollider, SleepingColliderHash, SleepingColliderEqual> m_SleepingColliders;
		std::vector<Prediction> m_Predictions;
		std::unique_ptr<QuadTree> m_QuadTree = nullptr;
		sf::FloatRect m_WorldBounds;
		
		sf::Vector2f m_Gravity{ 0.f, 980.f }; // This doesn't need to be a vector unless we want to simulate planets or some shit
		const TimeSingleton& m_TimeSingletonInstance = TimeSingleton::GetInstance();

		const float SLEEP_VELOCITY = 1.0f;
		const float SLEEP_VELOCITY_SQUARED = SLEEP_VELOCITY * SLEEP_VELOCITY;
		const float SLEEP_TIME = 5.0f;
		
		// physics trigger events
		struct TriggerPair
		{
			const Collider* trigger;
			const Collider* other;
			CollisionInfo hitInfo;
			
			bool operator==(const TriggerPair& rhs) const
			{
				return (trigger == rhs.trigger && other == rhs.other) || (trigger == rhs.other && other == rhs.trigger);
			}
		};
		
		std::vector<TriggerPair> m_HitEventTriggers;
		std::vector<TriggerPair> m_ActiveTriggers;
		std::vector<TriggerPair> m_PreviousFrameTriggers;
		void GenerateEvents(const std::vector<Prediction>& predictionsVec);
		void RemoveFromTriggerLists(Collider* collider);
		void ProcessTriggerEvents();
		static void NotifyTriggerEvent(const TriggerPair& trigger, EventType eventType);
		static void NotifyHitEvent(const TriggerPair& trigger, EventType eventType);
		
		static bool AABBOverlap(const sf::FloatRect& a, const sf::FloatRect& b)
		{
			return !(Right(a) <= Left(b) ||
					 Right(b) <= Left(a) ||
					 Bottom(a) <= Top(b) ||
					 Bottom(b) <= Top(a));
		}
		
		void PredictMovement(std::vector<Prediction>& predictionsVec) const;
		void DetectCollisions(std::vector<Prediction>& predictionsVec);
		static void ResolveCollision(Prediction& prediction, const CollisionInfo& collision);

		static void UpdateFinalPosition(const Prediction& prediction);
		static CollisionDetectionResult HandleStaticCollisions(Prediction& dynamicCollider, const Collider* staticCollider);
		static CollisionDetectionResult HandleDynamicCollisions(Prediction& dynamicColliderA, Prediction& dynamicColliderB);

		// different friction models, not sure which one to use
		void ApplyFrictionOnceWithStaticKinetic(Prediction& prediction) const;
		void ApplyFrictionOnce(Prediction& prediction) const;
		void ApplyFriction(Prediction& prediction, const CollisionInfo& collision) const;
		static void ApplyFriction(Prediction& prediction);
		
		// QuadTree Functions
		void UpdateWorldBounds(const sf::FloatRect& aabb);
	};
}

