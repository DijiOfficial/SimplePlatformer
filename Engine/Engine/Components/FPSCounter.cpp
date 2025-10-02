#include "FPSCounter.h"
#include "../Components/TextComp.h"
#include "../Singleton/TimeSingleton.h"
#include "../Core/GameObject.h"
#include <format>

diji::FPSCounter::FPSCounter(GameObject* ownerPtr) 
    : Component(ownerPtr)
    , m_TextComponentPtr{ nullptr }
    , m_FrameCount{ 0 }
    , m_Fps{ 0 }
    , m_ElapsedTime{ 0 } 
{ 
}

void diji::FPSCounter::Init()
{
    m_TextComponentPtr = GetOwner()->GetComponent<TextComp>();
}

void diji::FPSCounter::Update()
{
    const double tempFps = m_Fps;
    m_ElapsedTime += TimeSingleton::GetInstance().GetDeltaTime();
    ++m_FrameCount;

    if (m_ElapsedTime >= REFRESH_RATE)
    {
        m_Fps = static_cast<float>(m_FrameCount / m_ElapsedTime);
        m_FrameCount = 0;
        m_ElapsedTime = 0;
    }

    if (std::fabs(m_Fps - tempFps) < 1e-9)
        m_TextComponentPtr->GetText().setString(std::format("{:.1f} FPS", m_Fps));
}
