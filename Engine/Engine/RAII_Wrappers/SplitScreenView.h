#pragma once
#include <SFML/Graphics/View.hpp>

namespace diji
{
    class Transform;

    class SplitScreenView final
    {
    public:
        explicit SplitScreenView(const sf::FloatRect& viewport);
        ~SplitScreenView() noexcept = default;

        SplitScreenView(const SplitScreenView& other) noexcept = default;
        SplitScreenView(SplitScreenView&& other) noexcept = default;
        SplitScreenView& operator=(const SplitScreenView& other) noexcept = default;
        SplitScreenView& operator=(SplitScreenView&& other) noexcept = default;

        void Update();

        void SetTargetTransform(const Transform* transform) { m_TargetTransform = transform; }
        void ClearTargetTransform() { m_TargetTransform = nullptr; m_IsFollowing = false; }

        void SetOffset(const sf::Vector2f& offset) { m_Offset = offset; }
        void ClearOffset() { m_Offset = sf::Vector2f(0.0f, 0.0f); }
        
        void SetView(const sf::View& view) { m_View = view; }
        [[nodiscard]] const sf::View& GetView() const { return m_View; }
        
        void SetIsFollowing(const bool isFollowing) { m_IsFollowing = isFollowing; }
        [[nodiscard]] bool GetIsFollowing() const { return m_IsFollowing; }

        void SetCenter(const sf::Vector2f& center) { m_View.setCenter(center); }
        
    private:
        sf::View m_View;
        sf::Vector2f m_Offset;

        const Transform* m_TargetTransform = nullptr;
        bool m_IsFollowing = false;
    };
}
