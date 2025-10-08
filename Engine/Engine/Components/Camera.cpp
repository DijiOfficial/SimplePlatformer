#include "Camera.h"
#include "../Core/Engine.h"
#include "../Core/GameObject.h"
#include "Transform.h"

diji::Camera::Camera(GameObject* ownerPtr, const float width, const float height)
    : Component(ownerPtr)
    , m_LevelBoundaries{ sf::Vector2f{ 0, 0 }, sf::Vector2f{ width, height } } 
    , m_CameraOffset{ 0, 0 }
    , m_Width{ width }
    , m_Height{ height }
    , m_IsLocked{ false }
{
    m_CameraView = sf::View(sf::Vector2f{ width * 0.5f, height * 0.5f }, sf::Vector2f{ width, height });
}

diji::Camera::Camera(GameObject* ownerPtr, const sf::Vector2f size)
    : Component(ownerPtr)
    , m_LevelBoundaries{ sf::Vector2f{ 0, 0 }, size }
    , m_CameraOffset{ 0, 0 }
    , m_Width{ size.x }
    , m_Height{ size.y }
    , m_IsLocked{ false }
{
    m_CameraView = sf::View(sf::Vector2f{ size.x * 0.5f, size.y * 0.5f }, size);
}

diji::Camera::Camera(GameObject* ownerPtr, const sf::Vector2u size)
    : Camera(ownerPtr, static_cast<sf::Vector2f>(size))
{
    
}

void diji::Camera::Init()
{
    m_TransformCompPtr = GetOwner()->GetComponent<Transform>();
    window::g_window_ptr->setView(m_CameraView);
}

void diji::Camera::LateUpdate()
{
    if (m_IsLocked) return;

    sf::Vector2f cameraPos = m_TransformCompPtr->GetPosition() + m_CameraOffset;

    if (m_IsClamped)
        Clamp(cameraPos);
    
    m_CameraView.setCenter(cameraPos);
    window::g_window_ptr->setView(m_CameraView); // big L from SFML imo
}

void diji::Camera::SetFollow(const GameObject* target)
{
    m_TransformCompPtr = target->GetComponent<Transform>();
}

void diji::Camera::SetFollowSelf()
{
    m_TransformCompPtr = GetOwner()->GetComponent<Transform>();
}

void diji::Camera::SetAsMainView() const
{
    window::g_window_ptr->setView(m_CameraView);
}

void diji::Camera::ClearFollow()
{
    m_TransformCompPtr = GetOwner()->GetComponent<Transform>();
}

sf::Vector2i diji::Camera::GetMouseWorldPosition(const sf::Vector2i& pos) const
{
    return static_cast<sf::Vector2i>(window::g_window_ptr->mapPixelToCoords(pos, m_CameraView));
}

void diji::Camera::Clamp(sf::Vector2f& pos) const
{
    const float halfWidth = m_Width * 0.5f;
    const float halfHeight = m_Height * 0.5f;

    const float levelLeft   = m_LevelBoundaries.left;
    const float levelTop    = m_LevelBoundaries.top;
    const float levelRight  = levelLeft + m_LevelBoundaries.width;
    const float levelBottom = levelTop + m_LevelBoundaries.height;

    float minX = levelLeft + halfWidth;
    float maxX = levelRight - halfWidth;
    float minY = levelTop + halfHeight;
    float maxY = levelBottom - halfHeight;

    if (m_LevelBoundaries.width < m_Width)
        minX = maxX = levelLeft + m_LevelBoundaries.width * 0.5f;
    if (m_LevelBoundaries.height < m_Height)
        minY = maxY = levelTop + m_LevelBoundaries.height * 0.5f;

    pos.x = std::clamp(pos.x, minX, maxX);
    pos.y = std::clamp(pos.y, minY, maxY);
}