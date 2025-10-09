#pragma once
#include "Engine/Components/Component.h"

#include <SFML/System/Vector2.hpp>

namespace diji
{
    class Sprite;
}

namespace thomasWasLate
{
    class BackgroundHandler final : public diji::Component
    {
    public:
        explicit BackgroundHandler(diji::GameObject* ownerPtr) : Component(ownerPtr) {}
        ~BackgroundHandler() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override;
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

    private:
        diji::Sprite* m_BackgroundSprite = nullptr;
        std::unordered_map<int, sf::Vector2i> m_TileIDToAtlasPos;

        void OnNewLevelLoaded() const;
    };
}
