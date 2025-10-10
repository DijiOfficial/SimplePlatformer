#include "GoombaAI.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/Transform.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Singleton/TimeSingleton.h"

void thomasWasLate::GoombaAI::Init()
{
    m_TransformCompPtr = GetOwner()->GetComponent<diji::Transform>();
    m_ColliderCompPtr = GetOwner()->GetComponent<diji::Collider>();
}

void thomasWasLate::GoombaAI::Update()
{
    m_TransformCompPtr->AddOffset(-1 * m_Speed * diji::TimeSingleton::GetInstance().GetDeltaTime(), 0.f);
}

