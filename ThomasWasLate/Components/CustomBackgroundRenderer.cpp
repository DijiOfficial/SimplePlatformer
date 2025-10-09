#include "CustomBackgroundRenderer.h"
#include "Engine/Components/TextureComp.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/Renderer.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/SceneManager.h"

void thomasWasLate::CustomBackgroundRenderer::Init()
{
    m_Scale = 5.f;
    m_TextureCompPtr = GetOwner()->GetComponent<diji::TextureComp>();
    m_TextureCompPtr->SetScale(m_Scale);
    m_TextureCompPtr->SetOriginToCenter();
    m_TransformCompPtr = GetOwner()->GetComponent<diji::Transform>();

    m_PlayerTransformPtr = diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar")->GetComponent<diji::Transform>();
    m_SFMLTexture = m_TextureCompPtr->GetTexture();
    m_Width = static_cast<float>(m_SFMLTexture.getSize().x) * m_Scale;
}
void thomasWasLate::CustomBackgroundRenderer::RenderFrame() const
{
    // not very efficient but works for now
    const float posY = m_TransformCompPtr->GetPosition().y;
    const int chunk = static_cast<int>(m_PlayerTransformPtr->GetPosition().x / m_Width) % static_cast<int>(m_Width);

    const float posX = m_Width * chunk;
    const float posX2 = m_Width * (chunk + 1);
    // Render center tile
    diji::Renderer::GetInstance().RenderTexture(
        m_SFMLTexture,
        m_TextureCompPtr->GetOrigin(),
        m_TextureCompPtr->GetRotationAngle(),
        posX, posY,
        m_Scale,
        m_TextureCompPtr->GetScaleY());
    
    diji::Renderer::GetInstance().RenderTexture(
        m_SFMLTexture,
        m_TextureCompPtr->GetOrigin(),
        m_TextureCompPtr->GetRotationAngle(),
        posX2, posY,
        m_Scale,
        m_TextureCompPtr->GetScaleY());

}
