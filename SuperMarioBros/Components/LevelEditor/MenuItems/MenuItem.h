#pragma once
#include <any>

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
        
        struct MenuInfo
        {
            bool ShouldLockControls;
            eMenuType MenuType;
            std::any Data;
        };
        
        [[nodiscard]] virtual MenuInfo ActivateMenu() = 0;
        virtual void CloseMenu() = 0;
    };
}
