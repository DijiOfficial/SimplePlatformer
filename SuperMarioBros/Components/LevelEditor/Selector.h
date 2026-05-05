#pragma once
#include "Engine/Components/Component.h"
#include "Engine/Singleton/TimerManager.h"

namespace diji
{
    class Transform;
}

namespace superMarioBros
{
    class Selector final : public diji::Component
    {
    public:
        explicit Selector(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~Selector() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
            
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void Move(const sf::Vector2f& direction, bool isStart);
        void SetMenuDate(std::vector<int> data) { m_MenuPositions = std::move(data); }
        
    private:
        diji::Transform* m_TransformCompPtr = nullptr;
        diji::TimerManager::TimerHandle m_MoveTimerHandle;
        sf::Vector2f m_ArenaBoundsHorizontal{ 25.f, 12000.f };
        sf::Vector2f m_ArenaBoundsVertical{ -25.f, 475.f };
        
        const float TILE_SIZE = 50.0f;
        const float INITIAL_DELAY_BEFORE_MOVE = 0.25f;
        const float DELAY_BETWEEN_MOVE = 0.03f;
        const float MENU_Y_POSITION = -25.f;
        bool m_IsInMenu = false;
        int m_CurrentMenuIndex = 0;

        std::vector<int> m_MenuPositions;

        [[nodiscard]] bool CanMove(const sf::Vector2f& dir);
        [[nodiscard]] bool CheckForMenuMovementX(const sf::Vector2f& dir);
        [[nodiscard]] bool CheckForMenuMovementY(const sf::Vector2f& dir);
        [[nodiscard]] int ClosestMenuItemPosition(int x);
        void EnterMenu();
    };
}
