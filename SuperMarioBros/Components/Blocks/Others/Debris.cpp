#include "Debris.h"

#include "Engine/Collision/Collider.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/Transform.h"

void superMarioBros::Debris::Init()
{
    m_TransformCompPtr = GetOwner()->GetComponent<diji::Transform>();

    sf::Vector2f impulse;
    switch (m_Direction)
    {
    case 0:
        impulse = sf::Vector2f(-250.f, -1200.f);
        break;
    case 1:
        impulse = sf::Vector2f(250.f, -1200.f);
        break;
    case 2:
        impulse = sf::Vector2f(-250.f, -800.f);
        break;
    case 3:
        impulse = sf::Vector2f(250.f, -800.f);
        break;
    default:
        throw std::invalid_argument("Invalid Direction");
    }

    GetOwner()->GetComponent<diji::Collider>()->ApplyImpulse(impulse);
}

void superMarioBros::Debris::Update()
{
    if (m_TransformCompPtr->GetPosition().y > 600.f)
        Destroy();
}
