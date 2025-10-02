#pragma once
#include "../Components/Component.h"

#include <algorithm>
#include <memory>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <functional>

namespace diji
{
	class Render;
	class Transform;

	// todo: Give GameObjects a default component to write custom code in. (equivalent of the Blueprint of an actor).
	// todo: Every GameObject should have a Transform component and should have a Getter here in the parent.
	class GameObject final
	{
	public:
		GameObject() = default;
		~GameObject() noexcept = default;

		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

		void Init() const;
		void OnEnable() const;
		void Start() const;
		
		void FixedUpdate() const;
		void Update() const;
		void LateUpdate() const;
		void Render() const;

		void OnDisable() const;
		void OnDestroy() const;

		void Destroy() const;
		void CreateDuplicate(GameObject* duplicate) const;
		
#pragma region Components
		template<typename T, typename... Args>
		void AddComponents(Args&&... args)
		{
			static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

			// Store a lambda that will re-create this component on a new GameObject
			m_ComponentStorage.push_back(
				{[storedArgs = std::make_tuple(std::forward<Args>(args)...)](GameObject* target)
					{
						std::apply([&]<typename... T0>(T0&&... unpackedArgs)
						{
							target->AddComponents<T>(std::forward<T0>(unpackedArgs)...);
						}, storedArgs);
					}
				});

			m_ComponentsPtrVec.push_back(std::make_unique<T>(this, std::forward<Args>(args)...));

			if constexpr (std::is_same_v<T, Transform>)
			{
				m_TransformCompPtr = dynamic_cast<Transform*>(m_ComponentsPtrVec.back().get());
				m_LocalPosition = GetWorldPosition();
			}
			else if constexpr (std::is_base_of_v<diji::Render, T>)
			{
				m_RenderCompPtr = dynamic_cast<diji::Render*>(m_ComponentsPtrVec.back().get());
			}
		}

		template<typename T>
		void RemoveComponent()
		{
			static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

			auto it = std::remove_if(m_ComponentsPtrVec.begin(), m_ComponentsPtrVec.end(),
				[](const std::unique_ptr<Component>& comp)
				{
					return dynamic_cast<T*>(comp.get()) != nullptr;
				});

			if (it != m_ComponentsPtrVec.end())
			{
				if constexpr (std::is_same_v<T, Transform>)
					m_TransformCompPtr = nullptr;
				else if constexpr (std::is_base_of_v<diji::Render, T>)
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
#pragma endregion Components
		
		//SceneGraph
		[[nodiscard]] GameObject* GetParent() const { return m_ParentPtr; }
		[[nodiscard]] GameObject* GetChild(const int index) const { return m_ChildrenPtrVec[index]; }
		[[nodiscard]] int GetChildCount() const { return static_cast<int>(m_ChildrenPtrVec.size()); }
		sf::Vector2f GetWorldPosition();

		void SetParent(GameObject* parent, bool keepWorldPosition);
		void SetLocalPosition(const sf::Vector2f& pos);
		
	private:
		bool m_PositionIsDirty = false;
		
		diji::Render* m_RenderCompPtr = nullptr;
		GameObject* m_ParentPtr = nullptr;
		Transform* m_TransformCompPtr = nullptr;
		
		sf::Vector2f m_LocalPosition = { 0 ,0 };
		
		std::vector<std::unique_ptr<Component>> m_ComponentsPtrVec;
		std::vector<GameObject*> m_ChildrenPtrVec;



		struct ComponentStorage
		{
			std::function<void(GameObject*)> DuplicateComponents;
		};
		std::vector<ComponentStorage> m_ComponentStorage;



		
		bool IsChildOf(GameObject* potentialChild) const;
		void AddChild(GameObject* child);
		void RemoveChild(GameObject* child);

		void SetPositionDirty();
		void UpdateWorldPosition();
	};
}
