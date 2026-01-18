#include "PointsBehaviour.h"

#include "Engine/Components/TextComp.h"
#include "Engine/Singleton/TimerManager.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/TimeSingleton.h"

void superMarioBros::PointsBehaviour::Init()
{
    if (!m_ShouldIgnoreLifetime)
        Destroy(1.7f);
    
    m_TransformCompPtr = GetOwner()->GetRootComponent();
}

void superMarioBros::PointsBehaviour::Update()
{

    GetOwner()->AddObjectWorldOffset(sf::Vector2f{ 0, m_Speed * diji::TimeSingleton::GetInstance().GetDeltaTime() });

    const sf::Vector2f pos = m_TransformCompPtr->GetWorldPosition();
    if (pos.y <= m_MaxHeight)
        m_TransformCompPtr->SetWorldPosition(sf::Vector2f{ pos.x, m_MaxHeight });
}

void superMarioBros::PointsBehaviour::SetValue(const int value) const
{
    GetOwner()->GetComponent<diji::TextComp>()->GetText().setString(std::to_string(value));
}

