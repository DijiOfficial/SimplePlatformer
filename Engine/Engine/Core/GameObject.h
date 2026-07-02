#pragma once
#include "../Components/Component.h"
#include "../Components/Transform.h"
#include "../Collision/CollisionStructs.h"

#include <algorithm>
#include <memory>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <functional>

namespace diji
{
	class Collider;
	class Render;
	class Transform;

	// todo: Give GameObjects a default component to write custom code in. (equivalent of the Blueprint of an actor).
	class GameObject final
	{
	public:
		GameObject();
		explicit GameObject(const sf::Vector2f& position);
		explicit GameObject(const float x, const float y);
		explicit GameObject(const int x, const int y);
		~GameObject() noexcept = default;

		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

		void Init() const;
		void OnEnable() const;
		void Start();
		
		void FixedUpdate() const;
		void Update() const;
		void LateUpdate() const;
		void Render() const;

		void OnDisable() const;
		void OnDestroy() const;
		void SetActive(const bool isActive);
		[[nodiscard]] bool IsActive() const { return m_IsActive; }
		[[nodiscard]] bool SimulatesPhysics() const { return m_SimulatesPhysics; }

		void Destroy() const;
		void CreateDuplicate(GameObject* duplicate) const;

		void NotifyTriggerEvent(const Collider* other, const EventType& eventType, const CollisionInfo& hitInfo) const;
		std::optional<sf::FloatRect> GetBoundingBox() const;
		
#pragma region Components
		template<typename T, typename... Args>
		T* AddComponent(Args&&... args) // todo: this should return the component pointer
		{
			static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

			// Store a lambda that will re-create this component on a new GameObject
			m_ComponentStorage.push_back(
				{[storedArgs = std::make_tuple(std::forward<Args>(args)...)](GameObject* target)
					{
						std::apply([&]<typename... T0>(T0&&... unpackedArgs)
						{
							target->AddComponent<T>(std::forward<T0>(unpackedArgs)...);
						}, storedArgs);
					}
				});

			auto component = std::make_unique<T>(this, std::forward<Args>(args)...);
			T* ptr = component.get();
			m_ComponentsPtrVec.push_back(std::move(component));

			if constexpr (std::is_base_of_v<diji::Render, T>)
			{
				m_RenderCompPtr = dynamic_cast<diji::Render*>(m_ComponentsPtrVec.back().get());
			}
			else if constexpr (std::is_same_v<T, Collider>)
			{
				m_ColliderCompPtr = dynamic_cast<Collider*>(m_ComponentsPtrVec.back().get());
				m_SimulatesPhysics = true;
			}

			return ptr;
		}

		template<typename T>
		void RemoveComponent()
		{
			static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

			if constexpr (std::is_same_v<T, Collider>)
				m_SimulatesPhysics = false;

			auto it = std::remove_if(m_ComponentsPtrVec.begin(), m_ComponentsPtrVec.end(),
				[](const std::unique_ptr<Component>& comp)
				{
					return dynamic_cast<T*>(comp.get()) != nullptr;
				});

			if (it != m_ComponentsPtrVec.end())
			{
				if constexpr (std::is_base_of_v<diji::Render, T>)
					m_RenderCompPtr = nullptr;

				m_ComponentsPtrVec.erase(it, m_ComponentsPtrVec.end());
			}
		}
		
		[[nodiscard]] const std::vector<std::unique_ptr<Component>>& GetComponents() const { return m_ComponentsPtrVec; }

		template<typename T>
		[[nodiscard]] T* GetComponent() const
		{
			static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

			for (const auto& comp : m_ComponentsPtrVec)
			{
				if (dynamic_cast<T*>(comp.get()) != nullptr)
				{
					return static_cast<T*>(comp.get());
				}
			}
			return nullptr;
		}

		template<typename T>
		[[nodiscard]] bool HasComponent() const
		{
			static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

			return std::ranges::any_of(m_ComponentsPtrVec,
				[](const auto& comp)
				{
					return dynamic_cast<T*>(comp.get()) != nullptr;
				});
		}

		template<typename IInterfaceType>
		IInterfaceType* GetInterface() const
		{
			for (auto& comp : m_ComponentsPtrVec)
			{
				if (auto interface = dynamic_cast<IInterfaceType*>(comp.get()))
					return interface;
			}
			return nullptr;
		}
#pragma endregion Components
#pragma region Tranform
		[[nodiscard]] sf::Vector2f GetObjectPosition() const; // todo: get and set position should work implicitly with canvas objects so I don't have to manually handle canvas positions in the game code
		void SetObjectPosition(const sf::Vector2f& position) const;

		[[nodiscard]] sf::Angle GetObjectRotation() const;
		void SetObjectRotation(const sf::Angle& rotation) const;

		[[nodiscard]] sf::Vector2f GetObjectScale2D() const;
		void SetObjectScale2D(const sf::Vector2f& scale) const;

		// todo: add Transform struct containing location, rotation, scale?
		// [[nodiscard]] transform GetObjectTransform();
		// void SetObjectTransform(const transform& scale);

		void AddObjectWorldOffset(const sf::Vector2f& pos) const;
		void AddObjectWorldRotation(const sf::Angle& rot) const;
		void AddObjectLocalOffset(const sf::Vector2f& offset) const;
		void AddObjectLocalRotation(const sf::Angle& rotOffset) const;

		[[nodiscard]] Transform* GetRootComponent() const { return m_RootTransform.get(); }
		void AttachToObject(const GameObject* parent, bool keepWorldPosition) const;
		void DetachFromObject(bool keepWorldPosition) const;
		
	private:
		std::unique_ptr<Transform> m_RootTransform = nullptr;
#pragma endregion

	private:
		bool m_IsActive = true;
		bool m_IsInitialized = false;
		bool m_SimulatesPhysics = false;
		mutable bool m_IsPendingDestroy = false;
		
		diji::Render* m_RenderCompPtr = nullptr;
		Collider* m_ColliderCompPtr = nullptr;
		
		std::vector<std::unique_ptr<Component>> m_ComponentsPtrVec;

		struct ComponentStorage
		{
			std::function<void(GameObject*)> DuplicateComponents;
		};
		std::vector<ComponentStorage> m_ComponentStorage;

		void SetChildrenPendingDestroy() const;
	};
}
