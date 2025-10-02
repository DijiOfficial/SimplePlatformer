#include "Transform.h"
#include "../Core/GameObject.h"
#include "../Singleton/Helpers.h"

void diji::Transform::Seek(const float speed)
{
    sf::Vector2f direction =  m_Target->GetPosition() - m_Position;

    if (direction == sf::Vector2f{ 0, 0 })
        return;

    direction =  Helpers::Normalize(direction);
    const sf::Vector2f newPosition = m_Position + direction * speed * m_TimeSingleton.GetDeltaTime();

    SetPosition(newPosition);
}

void diji::Transform::SetTarget(const GameObject* target)
{
    m_Target = target->GetComponent<Transform>();
}
