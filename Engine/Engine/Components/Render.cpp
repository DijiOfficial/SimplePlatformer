#include "Render.h"

#include "../Core/GameObject.h"
#include "../Core/Renderer.h"
#include "Transform.h"
#include "TextureComp.h"
#include "TextComp.h"
#include "Sprite.h"
#include "../Singleton/ResourceManager.h"

diji::Render::Render(GameObject* ownerPtr, const float scale) 
    : Render(ownerPtr)
{
    m_TextureCompPtr = nullptr;
    m_TransformCompPtr = nullptr;
    m_TextCompPtr = nullptr;
    m_SpriteCompPtr = nullptr;
    ownerPtr->SetObjectScale2D(sf::Vector2f{ scale, scale });
}

diji::Render::Render(GameObject* ownerPtr)
    : Component(ownerPtr)
{
    m_TextureCompPtr = nullptr;
    m_TransformCompPtr = nullptr;
    m_TextCompPtr = nullptr;
    m_SpriteCompPtr = nullptr;
}

void diji::Render::Init()
{
    const auto& ownerPtr = GetOwner();

    m_TransformCompPtr = ownerPtr->GetRootComponent();
    m_TextureCompPtr = ownerPtr->GetComponent<TextureComp>();
    m_TextCompPtr = ownerPtr->GetComponent<TextComp>();
    m_SpriteCompPtr = ownerPtr->GetComponent<Sprite>();
    
    if (m_TextureCompPtr)
        m_SFMLTexture = m_TextureCompPtr->GetTexture();
    else if (m_SpriteCompPtr)
        m_SFMLTexture = m_SpriteCompPtr->GetTexture();
}

void diji::Render::RenderFrame() const
{
    if (not m_Render)
        return;

    const sf::Vector2f pos = m_TransformCompPtr->GetWorldPosition();
    
    // todo: for future reference it would be better to pass the textureComp as a parameter at this point.
    // todo: it would be better if every object had a render component and that render comp was different than a component. You could then regardless of the component call a virtual Render function on it, making the use of custom renderer much rarer
    if (m_TextureCompPtr) // todo: optimize this later, TextureComp doesn't need to know about the scale and rotation, it's based on the owner's transform now. Also optimize the call to the owner.
        Renderer::GetInstance().RenderTexture(m_SFMLTexture, m_TextureCompPtr->GetOrigin(), GetOwner()->GetObjectRotation(), pos.x, pos.y, m_TextureCompPtr->GetScaleX(), m_TextureCompPtr->GetScaleY());
    else if (m_TextCompPtr)
        Renderer::GetInstance().RenderText(m_TextCompPtr->GetText(), pos.x, pos.y, 1.f, m_TextCompPtr->GetIsCentered());
    else if (m_SpriteCompPtr)
        m_SpriteCompPtr->Render();
}

void diji::Render::UpdateTexture(sf::Texture& texture)
{
    m_SFMLTexture = texture;
}

void diji::Render::LoadShaderFromFile(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    m_ShaderPtr = &ResourceManager::GetInstance().LoadShader(vertexShaderPath, fragmentShaderPath);
}

sf::Vector2f diji::Render::GetScaledSize() const
{
    if (m_TextureCompPtr) return {};

    const auto size = m_TextureCompPtr->GetSize();
    return { static_cast<float>(size.x) * m_TextureCompPtr->GetScaleX(), static_cast<float>(size.y) * m_TextureCompPtr->GetScaleY()};
}

void diji::Render::SetScale(const float scale)
{
    GetOwner()->SetObjectScale2D(sf::Vector2f{ scale, scale });
}

sf::FloatRect diji::Render::GetBoundingBox() const
{
    if (!m_TextureCompPtr && !m_SpriteCompPtr && !m_TextCompPtr)
        return sf::Rect{ sf::Vector2f{ 0.f, 0.f }, sf::Vector2f{ 0.f, 0.f } };

    const sf::Vector2f pos = m_TransformCompPtr ? m_TransformCompPtr->GetWorldPosition() : sf::Vector2f{0.f, 0.f};
    sf::Vector2f size{ 0.f, 0.f };

    if (m_TextureCompPtr)
    {
        const sf::Vector2u texSize = m_TextureCompPtr->GetTexture().getSize();
        size.x = static_cast<float>(texSize.x) * m_TextureCompPtr->GetScaleX();
        size.y = static_cast<float>(texSize.y) * m_TextureCompPtr->GetScaleY();
    }
    else if (m_SpriteCompPtr)
    {
        // const auto bounds = m_SpriteCompPtr->GetGlobalBounds(); // sf::Rect<float>
        size.x = 0;
        size.y = 0;
    }
    else if (m_TextCompPtr)
    {
        const auto bounds = m_TextCompPtr->GetText().getLocalBounds();
        size.x = bounds.size.x;
        size.y = bounds.size.y;
    }

    return { pos, size };
}

