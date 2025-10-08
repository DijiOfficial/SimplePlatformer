#pragma once
#include "Component.h"
#include "../Singleton/TimeSingleton.h"

#include <SFML/System/Vector2.hpp>

namespace diji
{
	// todo: Could be interesting to add basic AI steering behaviours (use GPP as reference)
	// todo: Transform should not be a component, but rather a GameObject property
	class Transform final : public Component
	{
	public:
		explicit Transform(GameObject* ownerPtr) : Component(ownerPtr) { SetPosition(0, 0); }
		explicit Transform(GameObject* ownerPtr, const float x, const float y) : Component(ownerPtr) { SetPosition(x, y); }
		explicit Transform(GameObject* ownerPtr, const int x, const int y) : Component(ownerPtr) { SetPosition(static_cast<float>(x), static_cast<float>(y)); }
		explicit Transform(GameObject* ownerPtr, const sf::Vector2f& pos) : Component(ownerPtr) { SetPosition(pos.x, pos.y); }
		~Transform() noexcept override = default;

		Transform(const Transform& other) = delete;
		Transform(Transform&& other) = delete;
		Transform& operator=(const Transform& other) = delete;
		Transform& operator=(Transform&& other) = delete;

		void Init() override {}
		void OnEnable() override {}
		void Start() override {}
		
		void FixedUpdate() override {}
		void Update() override {}
		void LateUpdate() override {}

		void OnDisable() override {}
		void OnDestroy() override {}

		[[nodiscard]] sf::Vector2f GetPosition() const { return m_Position; }
		void SetPosition(const float x, const float y) { m_Position.x = x; m_Position.y = y; }
		void SetPosition(const sf::Vector2f pos) { m_Position = pos; }
		void AddOffset(const float  x, const float y) { m_Position.x += x; m_Position.y += y; }
		void AddOffset(const sf::Vector2f& offset) { m_Position.x += offset.x; m_Position.y += offset.y; }

		// this probably should be separate later or if revisited with more behaviours
		void Seek(float speed);
		void SetTarget(const GameObject* target);
		[[nodiscard]] Transform* GetTarget() const { return m_Target; }
		
	private:
		sf::Vector2f m_Position;

		// also separate 
		Transform* m_Target = nullptr;
		TimeSingleton& m_TimeSingleton = TimeSingleton::GetInstance();
	};
}
