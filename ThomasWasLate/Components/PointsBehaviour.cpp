#include "PointsBehaviour.h"

#include "Engine/Components/TextComp.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Singleton/TimerManager.h"
#include "Engine/Components/Transform.h"

void thomasWasLate::PointsBehaviour::Init()
{
    (void)diji::TimerManager::GetInstance().SetTimer([&]()
    {
        diji::SceneManager::GetInstance().SetPendingDestroy(GetOwner());
    }, 1.7f, false);

    m_TransformCompPtr = GetOwner()->GetComponent<diji::Transform>();
}

void thomasWasLate::PointsBehaviour::Update()
{
    m_TransformCompPtr->AddOffset(0, m_Speed * diji::TimeSingleton::GetInstance().GetDeltaTime());
}

void thomasWasLate::PointsBehaviour::SetValue(const int value) const
{
    GetOwner()->GetComponent<diji::TextComp>()->GetText().setString(std::to_string(value));
}

