#include "CustomBackgroundRenderer.h"
#include "Engine/Components/TextureComp.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/Renderer.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/SceneManager.h"

void superMarioBros::CustomBackgroundRenderer::Init()
{
    constexpr float scale = 3.1f;
    m_OwnerPtr = GetOwner();
    m_OwnerPtr->SetObjectScale2D(sf::Vector2f{ scale, scale });

    m_TextureCompPtr = m_OwnerPtr->GetComponent<diji::TextureComp>();
    m_TextureCompPtr->SetOriginToCenter();
    m_TransformCompPtr = m_OwnerPtr->GetRootComponent();

    m_PlayerTransformPtr = diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar")->GetRootComponent();
    m_SFMLTexture = m_TextureCompPtr->GetTexture();
    m_Width = static_cast<float>(m_SFMLTexture.getSize().x) * scale;
    m_WidthCenter = m_Width * 0.5f;
}

void superMarioBros::CustomBackgroundRenderer::RenderFrame() const
{
    // not very efficient but works for now
    const float posY = m_TransformCompPtr->GetWorldPosition().y;
    const int chunk = static_cast<int>(m_PlayerTransformPtr->GetWorldPosition().x - m_WidthCenter) / static_cast<int>(m_Width);

    const float posX = m_Width * chunk + m_WidthCenter;
    const float posX2 = m_Width * (chunk + 1) + m_WidthCenter;
    const sf::Vector2f& scale = m_OwnerPtr->GetObjectScale2D();
    // Render center tile
    diji::Renderer::GetInstance().RenderTexture(
        m_SFMLTexture,
        m_TextureCompPtr->GetOrigin(),
        m_OwnerPtr->GetObjectRotation(),
        posX, posY,
        scale.x,
        scale.y);
    
    diji::Renderer::GetInstance().RenderTexture(
        m_SFMLTexture,
        m_TextureCompPtr->GetOrigin(),
        m_OwnerPtr->GetObjectRotation(),
        posX2, posY,
        scale.x,
        scale.y);

}
