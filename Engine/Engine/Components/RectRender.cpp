#include "RectRender.h"

#include "TextureComp.h"
#include "../Core/Renderer.h"
#include "Transform.h"
#include "../Core/GameObject.h"
#include "../Collision/Collider.h"

diji::RectRender::RectRender(GameObject* ownerPtr, const bool isDebug)
    : Render(ownerPtr)
    , m_IsDebug{ isDebug }
    , m_NeedsUpdate{ isDebug }
{
}

void diji::RectRender::Init()
{
    m_TransformCompPtr = GetOwner()->GetComponent<Transform>();
}

void diji::RectRender::RenderFrame() const
{
    if (m_IsDebug)
        GetOwner()->GetComponent<Render>()->RenderFrame();
    
    if (not m_Render)
        return;

    m_RectangleShape.setPosition(m_TransformCompPtr->GetPosition());

    Renderer::GetInstance().DrawRect(m_RectangleShape, m_OutlineColor, m_FillColor, m_LineWidth);
}

void diji::RectRender::Update()
{
    if (m_NeedsUpdate)
        SetDrawCollision();
}

void diji::RectRender::SetDrawCollision()
{
    m_NeedsUpdate = false;
    
    const auto collider = GetOwner()->GetComponent<Collider>();
    if (!collider)
        return;

    const auto& rect = collider->GetCollisionBox();

    m_RectangleShape.setSize({ rect.width, rect.height });
    m_RectangleShape.setOrigin(-collider->GetOffset());

    m_FillColor = sf::Color::Transparent;
    m_OutlineColor = sf::Color::Green;
    m_LineWidth = 1.f;

    m_Render = true;
}

