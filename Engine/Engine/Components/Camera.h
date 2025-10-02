#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/View.hpp>

#include "../Components/Component.h"

namespace diji
{
    class Transform;
    class GameObject;

    class Camera final : public Component
    {
    public:
        explicit Camera(GameObject* ownerPtr, float width, float height);
        explicit Camera(GameObject* ownerPtr, sf::Vector2f size);
        explicit Camera(GameObject* ownerPtr, sf::Vector2u size);
        ~Camera() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override;
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}
        
        void SetLevelBoundaries(const sf::FloatRect& levelBoundaries) { m_LevelBoundaries = levelBoundaries; }
        void UnlockCamera() { m_IsLocked = false; }
        void LockCamera() { m_IsLocked = true; }
        void SetOffsetCamera(const sf::Vector2f& offset) { m_CameraOffset = offset; }
        void AddOffsetCamera(const sf::Vector2f& offset) { m_CameraOffset += offset; }
        void ResetOffset() { m_CameraOffset = sf::Vector2f{ 0, 0 }; }
        void SetFollow(const GameObject* target);
        void SetFollowSelf();
        void SetAsMainView() const;
        void SetClampingMode(const bool isClamped) { m_IsClamped = isClamped; }
        void ClearFollow();
        
        sf::Vector2f GetCameraOffset() const { return m_CameraOffset; }
        bool GetIsCameraLocked() const { return m_IsLocked; }
        sf::Vector2i GetMouseWorldPosition(const sf::Vector2i& pos) const;
        const sf::View& GetCameraView() const { return m_CameraView; }
        
    private:
        Transform* m_TransformCompPtr = nullptr;
        sf::View m_CameraView;
        sf::FloatRect m_LevelBoundaries;
        sf::Vector2f m_CameraOffset;
        float m_Width;
        float m_Height;
        bool m_IsLocked;
        bool m_IsClamped = true;

        void Clamp(sf::Vector2f& pos) const;
    };
}
