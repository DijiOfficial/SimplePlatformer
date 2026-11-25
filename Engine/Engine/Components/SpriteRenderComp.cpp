#include "SpriteRenderComp.h"
#include "Transform.h"
#include "../Core/Renderer.h"
#include "../Core/GameObject.h"
#include "../Singleton/ResourceManager.h"
#include "../Singleton/Helpers.h"

#include <algorithm>
#include <cmath>

diji::SpriteRenderComponent::SpriteRenderComponent(GameObject* ownerPtr, const std::string& texturePath)
    : Render{ ownerPtr }
    , m_Sprite{ ResourceManager::GetInstance().LoadTexture(texturePath).Get() }
{
    const auto size = m_Sprite.getTexture().getSize();
    m_TotalAnimationFrames = static_cast<int>(static_cast<float>(size.x) / static_cast<float>(size.y));
    m_FrameSize = { static_cast<int>(size.y), static_cast<int>(size.y) };

    m_Sprite.setOrigin(sf::Vector2f{ static_cast<float>(m_FrameSize.x) * 0.5f, static_cast<float>(m_FrameSize.y) * 0.5f });
}

diji::SpriteRenderComponent::SpriteRenderComponent(GameObject* ownerPtr, const std::string& texturePath, const sf::Vector2i& frameSize, const int totalAnimationFrames, const float frameDurationSec)
    : Render{ ownerPtr }
    , m_Sprite{ ResourceManager::GetInstance().LoadTexture(texturePath).Get() }
    , m_FrameSize{ frameSize }
    , m_TotalAnimationFrames{ totalAnimationFrames <= 0 ? 1 : totalAnimationFrames }
    , m_FrameDuration{ frameDurationSec <= 0.0f ? 0.1f : frameDurationSec }
{
    m_Sprite.setOrigin(sf::Vector2f{ static_cast<float>(m_FrameSize.x) * 0.5f, static_cast<float>(m_FrameSize.y) * 0.5f });
}

void diji::SpriteRenderComponent::Init()
{
    m_TransformCompPtr = GetOwner()->GetRootComponent();
}

void diji::SpriteRenderComponent::Start()
{
    if (m_SkipStart) return;
    
    m_Sprite.setScale(m_TransformCompPtr->GetWorldScale2D());
    m_Sprite.setTextureRect(sf::IntRect{ sf::Vector2i{ 0, 0 }, sf::Vector2i{ m_FrameSize.x, m_FrameSize.y } });
}

void diji::SpriteRenderComponent::LateUpdate()
{
    const sf::Vector2f worldPosition = m_TransformCompPtr->GetWorldPosition();
    if (Helpers::AreVectorsEqual(m_Sprite.getPosition(), worldPosition) == false)
        m_Sprite.setPosition(worldPosition);

    const sf::Angle worldRotation = m_TransformCompPtr->GetWorldRotation();
    if (Helpers::AreFloatEqual(m_Sprite.getRotation().asDegrees(), worldRotation.asDegrees()) == false)
        m_Sprite.setRotation(worldRotation);

    if (m_IsPlaying == false)
        return;

    if (m_UseGlobalClock && m_IsLooping && m_FrameDuration > 0.0f)
    {
        const double loopDuration = m_FrameDuration * static_cast<double>(m_TotalAnimationFrames);
        const double timeInLoop = std::fmod(m_TimeSingletonInstance.GetGlobalTime(), loopDuration);
        const int frame = std::min(static_cast<int>(timeInLoop / m_FrameDuration), m_TotalAnimationFrames - 1);
        if (frame != m_CurrentFrame)
        {
            m_CurrentFrame = frame;
            UpdateFrame();
        }
        return;
    }

    m_AnimationTimer += m_TimeSingletonInstance.GetDeltaTime();
  
    while (m_AnimationTimer >= m_FrameDuration)
    {
        m_AnimationTimer -= m_FrameDuration;
        ++m_CurrentFrame;
        if (m_CurrentFrame >= m_TotalAnimationFrames)
        {
            if (m_IsLooping)
                m_CurrentFrame = 0;
            else
            {
                m_CurrentFrame = m_TotalAnimationFrames - 1;
                m_IsPlaying = false;
                break;
            }
        }
        
        m_Sprite.setTextureRect(sf::IntRect{ sf::Vector2i{ (m_StartingFrameX + m_CurrentFrame) * m_FrameSize.x, m_StartingFrameY * m_FrameSize.y }, sf::Vector2i{ m_FrameSize.x, m_FrameSize.y } });
    }
}

void diji::SpriteRenderComponent::RenderFrame() const
{
    if (m_Render == false)
        return;

    if (m_RenderWithShader)
        Renderer::GetInstance().RenderSpriteWithShader(m_Sprite, m_ShaderPtr);
    else
        Renderer::GetInstance().RenderSprite(m_Sprite);
}

void diji::SpriteRenderComponent::SetFrameSize(const sf::Vector2i& size)
{
    m_FrameSize = size;

    m_Sprite.setOrigin(sf::Vector2f{ static_cast<float>(m_FrameSize.x) * 0.5f, static_cast<float>(m_FrameSize.y) * 0.5f });
}

void diji::SpriteRenderComponent::SetFrameSizeX(const int x)
{
    m_FrameSize.x = x;

    m_Sprite.setOrigin(sf::Vector2f{ static_cast<float>(m_FrameSize.x) * 0.5f, static_cast<float>(m_FrameSize.y) * 0.5f });
}

void diji::SpriteRenderComponent::SetFrameSizeY(const int y)
{
    m_FrameSize.y = y;

    m_Sprite.setOrigin(sf::Vector2f{ static_cast<float>(m_FrameSize.x) * 0.5f, static_cast<float>(m_FrameSize.y) * 0.5f });
}

sf::Vector2f diji::SpriteRenderComponent::GetScaledSize() const
{
    const auto& scale = m_TransformCompPtr->GetWorldScale2D();
    return sf::Vector2f{ std::abs(static_cast<float>(m_FrameSize.x) * scale.x), std::abs(static_cast<float>(m_FrameSize.y) * scale.y) };
}

void diji::SpriteRenderComponent::SetTotalAnimationFrames(const int count)
{
    m_TotalAnimationFrames = count <= 0 ? 1 : count;
}

void diji::SpriteRenderComponent::SetCurrentAnimationFrame(const int frame)
{
    m_CurrentFrame = std::clamp(frame, 0, m_TotalAnimationFrames - 1); 
    m_Sprite.setTextureRect(sf::IntRect{ sf::Vector2i{ (m_StartingFrameX + m_CurrentFrame) * m_FrameSize.x, m_StartingFrameY * m_FrameSize.y }, sf::Vector2i{ m_FrameSize.x, m_FrameSize.y } });
}

void diji::SpriteRenderComponent::SetScale(const float scale)
{
    Render::SetScale(scale);

    m_Sprite.setScale(sf::Vector2f{ scale, scale });
}

void diji::SpriteRenderComponent::InvertSprite()
{
    const auto* owner = GetOwner();
    const auto scale = [&]()
    {
        const auto s = owner->GetObjectScale2D();
        return sf::Vector2f{ -s.x, std::abs(s.y) };
    }();
    
    owner->SetObjectScale2D(scale);
    m_Sprite.setScale(scale);
}

void diji::SpriteRenderComponent::SetSpriteLookingLeft()
{
    const auto* owner = GetOwner();
    const auto scale = [&]()
    {
        const auto s = owner->GetObjectScale2D();
        return sf::Vector2f{ -std::abs(s.x), std::abs(s.y) };
    }();

    owner->SetObjectScale2D(scale);
    m_Sprite.setScale(scale);
}

void diji::SpriteRenderComponent::SetSpriteLookingRight()
{
    const auto* owner = GetOwner();
    const auto scale = [&]()
    {
        const auto s = owner->GetObjectScale2D();
        return sf::Vector2f{ std::abs(s.x), std::abs(s.y) };
    }();

    owner->SetObjectScale2D(scale);
    m_Sprite.setScale(scale);
}

void diji::SpriteRenderComponent::SetTotalAnimationTime(const float time)
{ 
    if (m_TotalAnimationFrames > 0)
        m_FrameDuration = time / static_cast<float>(m_TotalAnimationFrames);
}

void diji::SpriteRenderComponent::UpdateFrame()
{
    m_Sprite.setTextureRect(sf::IntRect{ sf::Vector2i{ (m_StartingFrameX + m_CurrentFrame) * m_FrameSize.x, m_StartingFrameY * m_FrameSize.y }, sf::Vector2i{ m_FrameSize.x, m_FrameSize.y } });
}

sf::FloatRect diji::SpriteRenderComponent::GetBoundingBox() const
{
    return static_cast<sf::FloatRect>(m_Sprite.getTextureRect());
}
