#pragma once
#include "Engine/Components/Component.h"
#include "MenuStructs.h"

namespace superMarioBros
{
    class Selector;

    class MenuItem : public diji::Component
    {
    public:
        explicit MenuItem(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~MenuItem() noexcept override = default;

        void Init() override {}
        void OnEnable() override {}
        void Start() override {}
            
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}
        
        void SetMenuArrow(diji::GameObject* menuArrow) { m_MenuArrow = menuArrow; }

        [[nodiscard]] virtual bool ActivateMenu() = 0;
        virtual void CloseMenu() = 0;
        virtual void Move(const sf::Vector2f& direction, bool isStart) = 0;
        [[nodiscard]] virtual bool Select() = 0;
        [[nodiscard]] virtual bool Return() { return false; }
        
    protected:
        diji::GameObject* m_MenuArrow = nullptr;
        const int SELECTOR_SPACING = 20;
    };
}
