#pragma once
#include <any>

#include "Engine/Components/Component.h"
#include "Engine/Singleton/TimerManager.h"
#include "MenuItems/MenuItem.h"

namespace diji
{
    class Transform;
    class Render;
}

namespace superMarioBros
{
    class MenuItem;

    class SelectorControls final : public diji::Component
    {
    public:
        explicit SelectorControls(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~SelectorControls() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override;
            
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void SetMenuArrow(diji::GameObject* menuArrow) { m_MenuArrowRef = menuArrow; }

        void Move(const sf::Vector2f& direction, bool isStart);
        // todo merge both menu open
        void OpenSaveMenu();
        void OpenBlocksMenu();
        void SelectCurrentMenuItem();
        void SetMenuTransform(std::vector<MenuItem*> data) { m_MenuItems = std::move(data); }
        void SetMenuYPosition(const float pos) { m_MenuYPosition = pos; }
        void ClearOutOfSpecialMenu();
        void SetMenuRenderRatio(const float ratio) { m_MenuRenderRatio = ratio; }
        void TrySetCanvasSelector(const sf::Vector2f& pos) const;
        
    private:
        Selector* m_SelectorRef = nullptr;
        diji::GameObject* m_CanvasSelector = nullptr;
        diji::GameObject* m_MenuArrowRef = nullptr;
        diji::Render* m_RenderCompPtr = nullptr;
        diji::Transform* m_TransformCompPtr = nullptr;
        diji::TimerManager::TimerHandle m_MoveTimerHandle = 0;
        sf::Vector2f m_ArenaBoundsHorizontal{ 25.f, 12000.f };
        sf::Vector2f m_ArenaBoundsVertical{ 25.f, 575.f };

        const float TILE_SIZE = 50.0f;
        const float INITIAL_DELAY_BEFORE_MOVE = 0.25f;
        const float DELAY_BETWEEN_MOVE = 0.03f;
        float m_MenuYPosition = -25.f;
        float m_MenuRenderRatio = 1.0f;
        int m_CurrentMenuIndex = 0;
        bool m_IsInMenu = false;
        bool m_DisableMovement = false;

        struct MenuEntry
        {
            int xPosition{};
            MenuItem* menuItemPtr{};
        };

        std::vector<MenuEntry> m_MenuEntries;
        std::vector<MenuItem*> m_MenuItems;

        template<typename T>
        [[nodiscard]] const T* GetMenuData(const std::any& data)
        {
            return std::any_cast<T>(&data);
        }
        
        [[nodiscard]] bool CanMove(const sf::Vector2f& dir);
        [[nodiscard]] bool CheckForMenuMovementX(const sf::Vector2f& dir);
        [[nodiscard]] bool CheckForMenuMovementY(const sf::Vector2f& dir);
        [[nodiscard]] int ClosestMenuItemPosition(int x);
        void EnterMenu();
        void ResetSpecialMenu();
        void ChooseMenuItem(int index);
    };
}
