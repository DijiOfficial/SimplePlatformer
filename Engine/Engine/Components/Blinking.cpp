#include "Blinking.h"
#include "Render.h"
#include "../Core/GameObject.h"
#include "../Singleton/TimeSingleton.h"

void diji::Blinking::Init()
{
    m_RenderCompPtr = GetOwner()->GetComponent<Render>();
}

void diji::Blinking::Update()
{
    m_ElapsedTime -= TimeSingleton::GetInstance().GetDeltaTime();
    if (m_ElapsedTime >= 0)
        return;
    
    m_ElapsedTime = m_RenderCompPtr->IsRendering() ? m_OnScreenTime : m_OffScreenTime;
    m_RenderCompPtr->ToggleRendering();
}

void diji::Blinking::InitializeRenderComp()
{
    Init();
}
