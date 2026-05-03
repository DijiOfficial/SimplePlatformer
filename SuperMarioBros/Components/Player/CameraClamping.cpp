#include "CameraClamping.h"

#include "Engine/Collision/Collider.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/SceneManager.h"
#include "PlayerCharacter.h"
#include "PlayerInputManager.h"

void superMarioBros::CameraClamping::Init()
{
    m_CameraPtr = diji::SceneManager::GetInstance().GetMainCamera()->GetComponent<diji::Camera>();
    m_PlayerInputManagerCompPtr = GetOwner()->GetComponent<PlayerInputManager>();
    m_ColliderCompPtr = GetOwner()->GetComponent<diji::Collider>();
    m_PlayerTransformCompPtr = GetOwner()->GetRootComponent();
}

void superMarioBros::CameraClamping::Start()
{
    m_Arena = m_CameraPtr->GetLevelBoundaries();
    m_LastArenaPosX = m_Arena.position.x;
    m_PlayerHalfWidth = m_ColliderCompPtr->GetShape()->GetShape().getOrigin().x;
}

void superMarioBros::CameraClamping::LateUpdate()
{
    const auto playerPos = m_PlayerTransformCompPtr->GetWorldPosition();
    const float playerX = playerPos.x;
    const float viewWidth = m_CameraPtr->GetCameraView().getSize().x;
    const float viewHalf = viewWidth * 0.5f;

    // compute desired camera-left based on player position and clamp it to previous arena boundaries
    float desiredLeft = playerX - viewHalf;
    const float minLeft = m_Arena.position.x;
    const float maxLeft = m_Arena.position.x + m_Arena.size.x - viewWidth;
    desiredLeft = std::clamp(desiredLeft, minLeft, maxLeft);

    m_LastArenaPosX = std::max(m_LastArenaPosX, desiredLeft);

    // New camera bounding rect (left, top, remaining width, height)
    const float remainingWidth = (m_Arena.position.x + m_Arena.size.x) - m_LastArenaPosX;
    const sf::FloatRect newBounds{ sf::Vector2f{ m_LastArenaPosX, m_Arena.position.y }, sf::Vector2f{ remainingWidth, m_Arena.size.y } };
    m_CameraPtr->SetLevelBoundaries(newBounds);

    // Clamp player X so it stays inside the 
    const float playerMinX = m_LastArenaPosX + m_PlayerHalfWidth;
    const float playerMaxX = m_LastArenaPosX + viewWidth - m_PlayerHalfWidth;
    const float clampedPlayerX = std::clamp(playerX, playerMinX, playerMaxX);

    if (!diji::Helpers::AreFloatEqual(clampedPlayerX, playerX))
    {
        m_PlayerTransformCompPtr->SetWorldPosition(sf::Vector2f{ clampedPlayerX, playerPos.y });
        m_ColliderCompPtr->SetVelocity(sf::Vector2f{0.00f, m_ColliderCompPtr->GetVelocity().y});
        m_PlayerInputManagerCompPtr->SetAgainstCameraEdge(true);
        return;
    }
    m_PlayerInputManagerCompPtr->SetAgainstCameraEdge(false);
}