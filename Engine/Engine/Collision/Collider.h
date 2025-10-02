#pragma once
#include "../Components/Component.h"
#include "CollisionShape.h"

#include <memory>
#include <stdexcept>

namespace diji 
{
    class Transform;
    class CollisionShape;
    
    class Collider final : public Component
    {
    public:
        Collider() = delete;
        
        template <typename... Args>
        explicit Collider(GameObject* ownerPtr, const CollisionShape::ShapeType type, Args&&... args)
            : Component(ownerPtr), m_Type{ type }
        {
            switch (type)
            {
            case CollisionShape::ShapeType::CIRCLE:
                m_Shape = std::make_unique<Circle>(std::forward<Args>(args)...);
                break;
            case CollisionShape::ShapeType::RECT:
                m_Shape = std::make_unique<Rect>(std::forward<Args>(args)...);
                break;
            case CollisionShape::ShapeType::TRIANGLE:
                m_Shape = std::make_unique<Triangle>(std::forward<Args>(args)...);
                break;
            default:
                throw std::invalid_argument("Unsupported shape type");
            }
        }
        
        ~Collider() noexcept override = default;

        void Init() override {}
        void OnEnable() override {}
        void Start() override;

        void FixedUpdate() override {}
        void Update() override;
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override;

        // void UpdateColliderFromTexture();

        // void SetTag (const std::string& tag) { m_Tag = tag; }
        // [[nodiscard]] const std::string& GetTag() const { return m_Tag; }
        // [[nodiscard]] Rectf GetCollisionBox() const { return m_CollisionBox; }
        // [[nodiscard]] const GameObject* GetParent() const { return GetOwner(); }
        // [[nodiscard]] Rectf GetLastState() const { return m_CollisionBox; }
        // [[nodiscard]] sf::Vector2f GetOffset() const { return m_Offset; }
        // [[nodiscard]] sf::Vector2f GetPosition() const;
        // [[nodiscard]] sf::Vector2f GetCenter() const;

    private:
        Transform* m_TransformCompPtr = nullptr;
        CollisionShape::ShapeType m_Type;
        std::unique_ptr<CollisionShape> m_Shape;
        // sf::Vector2f m_Offset{ 0, 0 };
        // std::string m_Tag = "Untagged";
        // Rectf m_CollisionBox;
        // Rectf m_LastState { .left = 0, .bottom = 0, .width = 0, .height = 0 };
        // bool m_IsOffsetSet = false;
        // bool m_IsCollisionSet = true;
    };
}

