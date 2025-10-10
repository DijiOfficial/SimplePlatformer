#include "SpriteRenderComp.h"
#include "Transform.h"
#include "../Core/Renderer.h"
#include "../Core/GameObject.h"
#include "../Singleton/ResourceManager.h"

diji::SpriteRenderComponent::SpriteRenderComponent(GameObject* ownerPtr, const std::string& texturePath)
    : Render{ ownerPtr }
{
    m_Texture = ResourceManager::GetInstance().LoadTexture(texturePath);

    m_Sprite.setTexture(m_Texture);

    const auto& size = m_Texture.getSize();
    m_TotalAnimationFrames = static_cast<int>(static_cast<float>(size.x) / static_cast<float>(size.y));
    m_FrameSize = { static_cast<int>(size.y), static_cast<int>(size.y) };

    m_Sprite.setOrigin(static_cast<float>(m_FrameSize.x) * 0.5f, static_cast<float>(m_FrameSize.y) * 0.5f);
}

diji::SpriteRenderComponent::SpriteRenderComponent(GameObject* ownerPtr, const std::string& texturePath, const sf::Vector2i& frameSize, int totalAnimationFrames, float frameDurationSec)
    : Render{ ownerPtr }
    , m_FrameSize{ frameSize }
    , m_TotalAnimationFrames{ totalAnimationFrames <= 0 ? 1 : totalAnimationFrames }
    , m_FrameDuration{ frameDurationSec <= 0.f ? 0.1f : frameDurationSec }
{
    m_Texture = ResourceManager::GetInstance().LoadTexture(texturePath);
    m_Sprite.setTexture(m_Texture);

    m_Sprite.setOrigin(static_cast<float>(m_FrameSize.x) * 0.5f, static_cast<float>(m_FrameSize.y) * 0.5f);
}

void diji::SpriteRenderComponent::Init()
{
    m_TransformCompPtr = GetOwner()->GetComponent<Transform>();
}

void diji::SpriteRenderComponent::Start()
{
    m_Sprite.setScale(sf::Vector2f{ m_Scale, m_Scale });

    m_Sprite.setTextureRect({ 0, 0, m_FrameSize.x, m_FrameSize.y });
}

void diji::SpriteRenderComponent::LateUpdate()
{
    const sf::Vector2f pos = [this]()
    {
        if (m_TransformCompPtr)
            return m_TransformCompPtr->GetPosition();
        
        return sf::Vector2f{ 0, 0 };
    }();
    m_Sprite.setPosition(pos);
    
    if (!m_IsPlaying) return;
    
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
        
        m_Sprite.setTextureRect({ (m_StartingFrameX + m_CurrentFrame) * m_FrameSize.x, m_StartingFrameY * m_FrameSize.y, m_FrameSize.x, m_FrameSize.y });
    }
}

void diji::SpriteRenderComponent::RenderFrame() const
{
    if (not m_Render)
        return;

    Renderer::GetInstance().RenderSprite(m_Sprite);
}

void diji::SpriteRenderComponent::SetFrameSize(const sf::Vector2i& size)
{
    m_FrameSize = size;

    m_Sprite.setOrigin(static_cast<float>(m_FrameSize.x) * 0.5f, static_cast<float>(m_FrameSize.y) * 0.5f);
}

void diji::SpriteRenderComponent::SetFrameSizeX(const int x)
{
    m_FrameSize.x = x;

    m_Sprite.setOrigin(static_cast<float>(m_FrameSize.x) * 0.5f, static_cast<float>(m_FrameSize.y) * 0.5f);
}

void diji::SpriteRenderComponent::SetFrameSizeY(const int y)
{
    m_FrameSize.y = y;

    m_Sprite.setOrigin(static_cast<float>(m_FrameSize.x) * 0.5f, static_cast<float>(m_FrameSize.y) * 0.5f);
}

void diji::SpriteRenderComponent::SetTotalAnimationFrames(const int count)
{
    m_TotalAnimationFrames = count <= 0 ? 1 : count;
}

void diji::SpriteRenderComponent::SetCurrentAnimationFrame(const int frame)
{
    m_CurrentFrame = std::clamp(frame, 0, m_TotalAnimationFrames - 1); 
    m_Sprite.setTextureRect({ m_StartingFrameX + m_CurrentFrame * m_FrameSize.x, m_StartingFrameY, m_FrameSize.x, m_FrameSize.y });
}

void diji::SpriteRenderComponent::SetScale(const float scale)
{
    Render::SetScale(scale);

    m_Sprite.setScale(sf::Vector2f{ m_Scale, m_Scale });
}

void diji::SpriteRenderComponent::InvertSprite()
{
    m_Scale = -m_Scale;

    m_Sprite.setScale(sf::Vector2f{ m_Scale, std::abs(m_Scale) });
}

void diji::SpriteRenderComponent::SetSpriteLookingLeft()
{
    m_Scale = -std::abs(m_Scale);

    m_Sprite.setScale(sf::Vector2f{ m_Scale, std::abs(m_Scale) });
}

void diji::SpriteRenderComponent::SetSpriteLookingRight()
{
    m_Scale = std::abs(m_Scale);

    m_Sprite.setScale(sf::Vector2f{ m_Scale, m_Scale });
}

void diji::SpriteRenderComponent::SetTotalAnimationTime(const float time)
{ 
    if (m_TotalAnimationFrames > 0)
        m_FrameDuration = time / static_cast<float>(m_TotalAnimationFrames); 
}

void diji::SpriteRenderComponent::UpdateFrame()
{
    m_Sprite.setTextureRect({ (m_StartingFrameX + m_CurrentFrame) * m_FrameSize.x, m_StartingFrameY * m_FrameSize.y, m_FrameSize.x, m_FrameSize.y });
}
