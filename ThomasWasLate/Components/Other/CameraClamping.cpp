#include "CameraClamping.h"

#include "Engine/Collision/Collider.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/SceneManager.h"

void thomasWasLate::CameraClamping::Init()
{
    m_PlayerTransformCompPtr = GetOwner()->GetComponent<diji::Transform>();
    m_ColliderCompPtr = GetOwner()->GetComponent<diji::Collider>();
    m_CameraPtr = diji::SceneManager::GetInstance().GetMainCamera()->GetComponent<diji::Camera>();
}

void thomasWasLate::CameraClamping::Start()
{
    m_Arena = m_CameraPtr->GetLevelBoundaries();
    m_LastArenaPosX = m_Arena.left;
    m_PlayerHalfWidth = GetOwner()->GetComponent<diji::Collider>()->GetShape()->GetAABB().width * 0.5f;
}

void thomasWasLate::CameraClamping::LateUpdate()
{
    const auto playerPos = m_PlayerTransformCompPtr->GetPosition();
    const float playerX = playerPos.x;
    const float viewWidth = m_CameraPtr->GetCameraView().getSize().x;
    const float viewHalf = viewWidth * 0.5f;

    // compute desired camera-left based on player position and clamp it to previous arena boundaries
    float desiredLeft = playerX - viewHalf;
    const float minLeft = m_Arena.left;
    const float maxLeft = m_Arena.left + m_Arena.width - viewWidth;
    desiredLeft = std::clamp(desiredLeft, minLeft, maxLeft);

    m_LastArenaPosX = std::max(m_LastArenaPosX, desiredLeft);

    // New camera bounding rect (left, top, remaining width, height)
    const float remainingWidth = (m_Arena.left + m_Arena.width) - m_LastArenaPosX;
    const sf::FloatRect newBounds{ m_LastArenaPosX, m_Arena.top, remainingWidth, m_Arena.height };
    m_CameraPtr->SetLevelBoundaries(newBounds);

    // Clamp player X so it stays inside the view
    const float playerMinX = m_LastArenaPosX + m_PlayerHalfWidth;
    const float playerMaxX = m_LastArenaPosX + viewWidth - m_PlayerHalfWidth;
    const float clampedPlayerX = std::clamp(playerX, playerMinX, playerMaxX);

    if (!diji::Helpers::AreFloatEqual(clampedPlayerX, playerX))
    {
        m_PlayerTransformCompPtr->SetPosition(clampedPlayerX, playerPos.y);
        // m_ColliderCompPtr->SetVelocity(sf::Vector2f{0.01f, m_ColliderCompPtr->GetVelocity().y});
    }
}
