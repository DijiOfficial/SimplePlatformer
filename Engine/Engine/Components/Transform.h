#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>

namespace diji
{
	class GameObject;
}

namespace diji
{
	inline sf::Vector2f operator*(const sf::Vector2f& lhs, const sf::Vector2f& rhs)
	{
		return { lhs.x * rhs.x, lhs.y * rhs.y };
	}

	inline sf::Vector2f operator/(const sf::Vector2f& lhs, const sf::Vector2f& rhs)
	{
		return { lhs.x / rhs.x, lhs.y / rhs.y };
	}

	inline sf::Vector2f& operator*=(sf::Vector2f& lhs, const sf::Vector2f& rhs)
	{
		lhs.x *= rhs.x;
		lhs.y *= rhs.y;
		return lhs;
	}

	inline sf::Vector2f& operator/=(sf::Vector2f& lhs, const sf::Vector2f& rhs)
	{
		lhs.x /= rhs.x;
		lhs.y /= rhs.y;
		return lhs;
	}
	
	class Transform final
	{
	public:
		explicit Transform(const sf::Vector2f& pos) { SetLocalPosition(pos); m_WorldPosition = pos; }
		~Transform() noexcept = default;

		Transform(const Transform& other) = delete;
		Transform(Transform&& other) = delete;
		Transform& operator=(const Transform& other) = delete;
		Transform& operator=(Transform&& other) = delete;

		[[nodiscard]] sf::Vector2f GetLocalPosition() const { return m_LocalPosition; }
		void SetLocalPosition(const sf::Vector2f& pos) { m_LocalPosition = pos; }
		
		[[nodiscard]] sf::Angle GetLocalRotation() const { return m_Rotation; }
		void SetLocalRotation(const sf::Angle& rotation) { m_Rotation = rotation; }
		
		[[nodiscard]] sf::Vector2f GetLocalScale2D() const { return m_Scale2D; }
		void SetLocalScale2D(const sf::Vector2f& scale) { m_Scale2D = scale; }
		
		[[nodiscard]] sf::Vector2f GetWorldPosition();
		void SetWorldPosition(const sf::Vector2f& pos);

		[[nodiscard]] sf::Angle GetWorldRotation();
		void SetWorldRotation(const sf::Angle& rotation);

		[[nodiscard]] sf::Vector2f GetWorldScale2D();
		void SetWorldScale2D(const sf::Vector2f& scale);

		void AttachToObject(Transform* parent, bool keepWorldPosition);
		void DetachFromObject(bool keepWorldPosition);
		// todo: complete GetPArent and Children
		// [[nodiscard]] GameObject* GetParentObject() const;
		// [[nodiscard]] const std::vector<Transform*>& GetChildObjects() const;

		static constexpr sf::Vector2f UP{ 0.f, -1.f };
		static constexpr sf::Vector2f RIGHT{ 1.f, 0.f };
	private:
		Transform* m_ParentTransformCompPtr = nullptr;
		std::vector<Transform*> m_ChildrenTransformCompPtrVec;
		
		sf::Vector2f m_LocalPosition;
		sf::Vector2f m_WorldPosition;
		sf::Vector2f m_Scale2D = { 1.f, 1.f };
		sf::Vector2f m_WorldScale2D = { 1.f, 1.f };
		sf::Angle m_Rotation = sf::degrees(0.f);
		sf::Angle m_WorldRotation = sf::degrees(0.f);

		bool m_IsPositionDirty = false;
		bool m_IsRotationDirty = false;
		bool m_IsScaleDirty = false;
		bool m_IsParented = false;

		void MarkDirtyMember(bool Transform::*dirtyMember);
		void MarkPositionDirty()	{ MarkDirtyMember(&Transform::m_IsPositionDirty);	}
		void MarkRotationDirty()	{ MarkDirtyMember(&Transform::m_IsRotationDirty);	}
		void MarkScaleDirty	()		{ MarkDirtyMember(&Transform::m_IsScaleDirty);		}

		void UpdateWorldPosition() { m_WorldPosition = m_IsParented ? m_ParentTransformCompPtr->GetWorldPosition() + m_LocalPosition : m_LocalPosition; }
		void UpdateWorldRotation() { m_WorldRotation = m_IsParented ? m_ParentTransformCompPtr->GetWorldRotation() + m_Rotation : m_Rotation; }
		void UpdateWorldScale2D() { m_WorldScale2D = m_IsParented ? m_ParentTransformCompPtr->GetWorldScale2D() * m_Scale2D : m_Scale2D; }
		
		template<typename T>
		[[nodiscard]] T GetWorld(T& worldValue, bool& isDirty, void (Transform::*updateFunc)())
		{
			if (!isDirty) return worldValue;
			isDirty = false;

			(this->*updateFunc)();
			return worldValue;
		}
	};
}
