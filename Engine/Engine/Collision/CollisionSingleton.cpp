#include "CollisionSingleton.h"
#include "Collider.h"

#include <algorithm>

void diji::CollisionSingleton::Reset()
{
	m_Colliders = std::map<const Collider*, Rectf>();
	m_LevelCollider = std::vector<std::vector<sf::Vector2f>>();
}

void diji::CollisionSingleton::AddCollider(const Collider* object, const Rectf& collider)
{
	//const auto colliders = std::vector<RectI>(1, collider);
	m_Colliders[object] = collider;
}

void diji::CollisionSingleton::RemoveCollider(const Collider* object)
{
	m_Colliders.erase(object);
}

void diji::CollisionSingleton::ParseRectInLevelCollider(const Rectf& rect)
{
	const sf::Vector2f bottomLeft(rect.left, rect.bottom);
	const sf::Vector2f topLeft(rect.left, rect.bottom + rect.height);
	const sf::Vector2f topRight(rect.left + rect.width, rect.bottom + rect.height);
	const sf::Vector2f bottomRight(rect.left + rect.width, rect.bottom);

	const std::vector<sf::Vector2f> vertices = { bottomLeft, topLeft, topRight, bottomRight };
	AddLevelCollider(vertices);
}

void diji::CollisionSingleton::UpdateCollider(const Collider* object, const Rectf& collider)
{
	if (m_Colliders.contains(object))
		m_Colliders[object] = collider;
}

std::vector<const diji::Collider*> diji::CollisionSingleton::IsColliding(const Collider* object)
{
	std::vector<const Collider*> collidingObjects;
	for (auto& [ColliderPtr, Rectf] : m_Colliders)
	{
		if (ColliderPtr == object)
			continue;
		
		if (AreRectsColliding(m_Colliders[object], Rectf))
			collidingObjects.push_back(ColliderPtr);
	}
	return collidingObjects;
}

std::vector<const diji::Collider*> diji::CollisionSingleton::IsColliding(const Collider* object, const Rectf& shape)
{
	std::vector<const Collider*> collidingObjects;
	for (auto& [ColliderPtr, Rectf] : m_Colliders)
	{
		if (ColliderPtr == object)
			continue;
		
		if (AreRectsColliding(shape, Rectf))
			collidingObjects.push_back(ColliderPtr);
	}
	return collidingObjects;
}

bool diji::CollisionSingleton::AreColliding(const Collider* source, const Collider* target)
{
	if (!source || !target)
		return false;

	const auto sourceIt = m_Colliders.find(source);
	const auto targetIt = m_Colliders.find(target);

	if (sourceIt == m_Colliders.end() || targetIt == m_Colliders.end())
		return false;

	// Use your rect collision detection function
	return AreRectsColliding(sourceIt->second, targetIt->second);
}

bool diji::CollisionSingleton::AreRectsColliding(const Rectf& rect1, const Rectf& rect2) const
{
	if ((rect1.left + rect1.width) < rect2.left || (rect2.left + rect2.width) < rect1.left)
 		return false;

	if (rect1.bottom > (rect2.bottom + rect2.height) || rect2.bottom > (rect1.bottom + rect1.height))
		return false;

	return true;
}

bool diji::CollisionSingleton::IsCollidingWithWorld(const Rectf& shape) const // todo: collision optimizations?
{
	const auto [left, bottom, width, height] = shape;

	const sf::Vector2f bottomLeft(left, bottom);
	const sf::Vector2f topLeft(left, bottom + height);
	const sf::Vector2f topRight(left + width, bottom + height);
	const sf::Vector2f bottomRight(left + width, bottom);

	return std::ranges::any_of(m_LevelCollider, [&](const auto& box)
	{
		return Raycast(box, bottomLeft, topLeft) ||
			   Raycast(box, topLeft, topRight) ||
			   Raycast(box, topRight, bottomRight) ||
			   Raycast(box, bottomRight, bottomLeft);
	});
}

bool diji::CollisionSingleton::IsCollidingWithWorld(const sf::Vector2f& point1, const sf::Vector2f& point2) const
{
	return std::ranges::any_of(m_LevelCollider, [&](const auto& collisionBox)
	{
		return Raycast(collisionBox, point1, point2);
	});
}

bool diji::CollisionSingleton::IsCollidingWithWorld(const Collider* object) const
{
	// Check if the object exists in the colliders map
	const auto it = m_Colliders.find(object);
	if (it == m_Colliders.end())
		return false;

	const Rectf& colliderRect = it->second;
	return IsCollidingWithWorld(colliderRect);
}

#pragma region utility functions
float diji::CollisionSingleton::Distance(const sf::Vector2f& a, const sf::Vector2f& b) const
{
	return std::hypot(a.x - b.x, a.y - b.y);
}

float diji::CollisionSingleton::DistanceSquared(const sf::Vector2f& a, const sf::Vector2f& b) const
{
	const float distX = a.x - b.x;
	const float distY = a.y - b.y;
	
	return distX * distX + distY * distY;
}

bool diji::CollisionSingleton::Raycast(const std::vector<sf::Vector2f>& vertices, const sf::Vector2f& rayP1, const sf::Vector2f& rayP2) const
{
	return Raycast(vertices.data(), vertices.size(), rayP1, rayP2);
}

bool diji::CollisionSingleton::Raycast(const sf::Vector2f* vertices, const size_t nrVertices, const sf::Vector2f& rayP1, const sf::Vector2f& rayP2) const
{
	if (nrVertices == 0)
		return false;

	Rectf r1, r2{};
	// r1: minimal AABB rect enclosing the ray
	r1.left = std::min(rayP1.x, rayP2.x);
	r1.bottom = std::min(rayP1.y, rayP2.y);
	r1.width = std::max(rayP1.x, rayP2.x) - r1.left;
	r1.height = std::max(rayP1.y, rayP2.y) - r1.bottom;

	// Line-line intersections.
	for (size_t idx{ 0 }; idx <= nrVertices; ++idx)
	{
		// Consider line segment between 2 consecutive vertices
		// (modulo to allow closed polygon, last - first vertices)
		const sf::Vector2f q1 = vertices[(idx + 0) % nrVertices];
		const sf::Vector2f q2 = vertices[(idx + 1) % nrVertices];

		// r2: minimal AABB rect enclosing the 2 vertices
		r2.left = std::min(q1.x, q2.x);
		r2.bottom = std::min(q1.y, q2.y);
		r2.width = std::max(q1.x, q2.x) - r2.left;
		r2.height = std::max(q1.y, q2.y) - r2.bottom;

		if (AreRectsColliding(r1, r2))
		{
			float lambda1{};
			float lambda2{};
			if (IntersectLineSegments(rayP1, rayP2, q1, q2, lambda1, lambda2))
			{
				if (lambda1 > 0 && lambda1 <= 1 && lambda2 > 0 && lambda2 <= 1)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool diji::CollisionSingleton::IntersectLineSegments(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& q1, const sf::Vector2f& q2, float& outLambda1, float& outLambda2, const double epsilon) const
{
	bool intersecting{ false };

	const sf::Vector2f p1p2 = CreateVector(p1, p2);
	const sf::Vector2f q1q2 = CreateVector(q1, q2);
	//const sf::Vector2f q1q2{ q1, q2 };

	// Cross product to determine if parallel
	const float denominator = CrossProduct(p1p2, q1q2);

	// Don't divide by zero
	if (std::abs(denominator) > epsilon)
	{
		intersecting = true;

		//const sf::Vector2f p1q1{ p1, q1 };
		const sf::Vector2f p1q1 = CreateVector(p1, q1);

		const float num1 = CrossProduct(p1q1, q1q2);
		const float num2 = CrossProduct(p1q1, p1p2);
		outLambda1 = num1 / denominator;
		outLambda2 = num2 / denominator;
	}
	else // are parallel
	{
		// Connect start points
		//const sf::Vector2f p1q1{ p1, q1 };
		const sf::Vector2f p1q1 = CreateVector(p1, q1);


		// Cross product to determine if segments and the line connecting their start points are parallel, 
		// if so, than they are on a line
		// if not, then there is no intersection
		if (std::abs(CrossProduct(p1q1, q1q2)) > epsilon)
		{
			return false;
		}

		// Check the 4 conditions
		outLambda1 = 0;
		outLambda2 = 0;
		if (IsPointOnLineSegment(p1, q1, q2) ||
			IsPointOnLineSegment(p2, q1, q2) ||
			IsPointOnLineSegment(q1, p1, p2) ||
			IsPointOnLineSegment(q2, p1, p2))
		{
			intersecting = true;
		}
	}
	return intersecting;
}

bool diji::CollisionSingleton::IsPointOnLineSegment(const sf::Vector2f& p, const sf::Vector2f& a, const sf::Vector2f& b) const
{
	const sf::Vector2f ap = CreateVector(a, p);
	const sf::Vector2f bp = CreateVector(b, p);

	// If not on same line, return false
	if (abs(CrossProduct(ap, bp)) > 0.001f)
	{
		return false;
	}

	// Both vectors must point in opposite directions if p is between a and b
	if (DotProduct(ap, bp) > 0)
	{
		return false;
	}

	return true;
}
#pragma endregion