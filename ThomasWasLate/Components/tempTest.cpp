#include "tempTest.h"

#include "Engine/Core/GameObject.h"
#include "Engine/Collision/Collider.h"

void thomasWasLate::tempTest::Init()
{
    m_ColliderPtr = GetOwner()->GetComponent<diji::Collider>();
    m_ColliderPtr->GetShape()->SetRotation(-90.f);
}

void thomasWasLate::tempTest::FixedUpdate()
{
    m_Angle += 45.f * diji::TimeSingleton::GetInstance().GetFixedUpdateDeltaTime();
    m_ColliderPtr->GetShape()->SetRotation(m_Angle);
}
