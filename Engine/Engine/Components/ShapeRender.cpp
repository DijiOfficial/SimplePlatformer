#include "ShapeRender.h"

#include "TextureComp.h"
#include "../Core/Renderer.h"
#include "Transform.h"
#include "../Core/GameObject.h"
#include "../Collision/Collider.h"

diji::ShapeRender::ShapeRender(GameObject* ownerPtr, const bool isDebug)
    : Render(ownerPtr)
    , m_IsDebug{ isDebug }
{
}

void diji::ShapeRender::Init()
{
    m_TransformCompPtr = GetOwner()->GetComponent<Transform>();
    m_ColliderPtr = GetOwner()->GetComponent<Collider>();
}

void diji::ShapeRender::RenderFrame() const
{
    if (m_IsDebug)
    {
        GetOwner()->GetComponent<Render>()->RenderFrame();
        // if (const auto& renderComp = GetOwner()->GetComponent<Render>()) // todo: use a bool instead
        // {
        //     // Only call RenderFrame if it's not a ShapeRender
        //     if (dynamic_cast<const ShapeRender*>(renderComp) == nullptr)
        //         renderComp->RenderFrame();
        // }
    }
    
    if (not m_Render)
        return;

    std::visit([](const auto& shape)
    {
        Renderer::GetInstance().DrawShape(shape);
    }, m_Shape);
}

void diji::ShapeRender::Update()
{
    if (m_NeedsUpdate)
        SetDrawCollision();

    if (m_ColliderPtr)
    {
        auto& shape = m_ColliderPtr->GetShape()->GetShape();
        if (const auto* rect = dynamic_cast<const sf::RectangleShape*>(&shape))
        {
            m_Shape = sf::RectangleShape(*rect);
        }
        else if (const auto* circ = dynamic_cast<const sf::CircleShape*>(&shape))
        {
            m_Shape = sf::CircleShape(*circ);
        }
        else if (const auto* conv = dynamic_cast<const sf::ConvexShape*>(&shape))
        {
            m_Shape = sf::ConvexShape(*conv);
        }

        ApplyStyle();
    }

    std::visit([this](auto& shape)
    {
        shape.setPosition(m_TransformCompPtr->GetPosition());
    }, m_Shape);
}

void diji::ShapeRender::SetDrawCollision()
{
    m_NeedsUpdate = false;
    
    const auto collider = GetOwner()->GetComponent<Collider>();
    if (!collider)
        return;

    const auto& shape = collider->GetShape()->GetShape();
    switch (collider->GetShapeType())
    {
    case CollisionShape::ShapeType::CIRCLE:
        m_Shape = *dynamic_cast<const sf::CircleShape*>(&shape); //todo: fix this
        break;
    case CollisionShape::ShapeType::RECT:
        m_Shape = *dynamic_cast<const sf::RectangleShape*>(&shape);
        break;
    case CollisionShape::ShapeType::TRIANGLE:
        m_Shape = *dynamic_cast<const sf::ConvexShape*>(&shape);
        break;
    default:
        throw std::invalid_argument("Unknown shape type in ShapeRender::SetDrawCollision");
    }

    std::visit([this](auto& newShape)
    {
        newShape.setOrigin(sf::Vector2f{ 0, 0 });
    }, m_Shape);
    
    m_FillColor = sf::Color::Transparent; // todo: redundant?
    m_OutlineColor = sf::Color::Green;
    m_LineWidth = 1.f;

    m_Render = true;
    
    ApplyStyle(); // or this redundant?
}

void diji::ShapeRender::ApplyStyle()
{
    std::visit([this](auto& shape)
    {
        shape.setFillColor(m_FillColor);
        shape.setOutlineColor(m_OutlineColor);
        shape.setOutlineThickness(m_LineWidth);
    }, m_Shape);
}