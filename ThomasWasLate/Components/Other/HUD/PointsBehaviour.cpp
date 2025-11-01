#include "PointsBehaviour.h"

#include "Engine/Components/TextComp.h"
#include "Engine/Singleton/TimerManager.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/GameObject.h"

void thomasWasLate::PointsBehaviour::Init()
{
    if (!m_ShouldIgnoreLifetime)
        Destroy(1.7f);
    
    m_TransformCompPtr = GetOwner()->GetComponent<diji::Transform>();
}

void thomasWasLate::PointsBehaviour::Update()
{
    m_TransformCompPtr->AddOffset(0, m_Speed * diji::TimeSingleton::GetInstance().GetDeltaTime());

    const sf::Vector2f pos = m_TransformCompPtr->GetPosition();
    if (pos.y <= m_MaxHeight)
        m_TransformCompPtr->SetPosition(pos.x, m_MaxHeight);
}

void thomasWasLate::PointsBehaviour::SetValue(const int value) const
{
    GetOwner()->GetComponent<diji::TextComp>()->GetText().setString(std::to_string(value));
}

