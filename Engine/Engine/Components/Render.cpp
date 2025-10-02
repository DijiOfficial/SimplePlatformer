#include "Render.h"

#include "../Core/GameObject.h"
#include "../Core/Renderer.h"
#include "Transform.h"
#include "TextureComp.h"
#include "TextComp.h"
#include "Sprite.h"

diji::Render::Render(GameObject* ownerPtr, const int scale) 
    : Render(ownerPtr)
{
    m_TextureCompPtr = nullptr;
    m_TransformCompPtr = nullptr;
    m_TextCompPtr = nullptr;
    m_SpriteCompPtr = nullptr;
    m_Scale = scale;
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

    m_TransformCompPtr = ownerPtr->GetComponent<Transform>();
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

    // every object should have a transform so this is redundant
    const sf::Vector2f pos = [this]()
    {
        if (m_TransformCompPtr)
            return m_TransformCompPtr->GetPosition();
        
        return sf::Vector2f{ 0, 0 };
    }();

    // todo: for future reference it would be better to pass the textureComp as a parameter at this point.
    // todo: it would be better if every object had a render component and that render comp was different than a component. You could then regardless of the component call a virtual Render function on it, making the use of custom renderer much rarer
    if (m_TextureCompPtr)
        Renderer::GetInstance().RenderTexture(m_SFMLTexture, m_TextureCompPtr->GetOrigin(), m_TextureCompPtr->GetRotationAngle(), pos.x, pos.y, m_TextureCompPtr->GetScaleX(), m_TextureCompPtr->GetScaleY());
    else if (m_TextCompPtr)
        Renderer::GetInstance().RenderText(m_TextCompPtr->GetText(), pos.x, pos.y, 1.f, m_TextCompPtr->GetIsCentered());
    else if (m_SpriteCompPtr)
        m_SpriteCompPtr->Render();
}

void diji::Render::UpdateTexture(sf::Texture& texture)
{
    m_SFMLTexture = texture;
}
