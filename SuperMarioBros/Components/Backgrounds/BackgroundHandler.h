#pragma once
#include "Engine/Components/Component.h"

#include <SFML/System/Vector2.hpp>

namespace diji
{
    class Sprite;
}

namespace superMarioBros
{
    class BackgroundHandler final : public diji::Component
    {
    public:
        explicit BackgroundHandler(diji::GameObject* ownerPtr) : Component(ownerPtr) {}
        explicit BackgroundHandler(diji::GameObject* ownerPtr, const bool isBonusAreaBackground) : Component(ownerPtr), m_IsBonusAreaBackground{ isBonusAreaBackground } {}
        ~BackgroundHandler() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override;
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void DisableLevelLoadingOnStart() { m_ShouldLoadFirstLevel = false; }
        void ApplyTile(int x, int y, int cols, char tileID);
        void TempReload(const int cols, const std::vector<char>& levelData);
        void LoadBonusAreaTiles() const;

    private:
        diji::Sprite* m_BackgroundSprite = nullptr;
        std::unordered_map<char, sf::Vector2i> m_TileIDToAtlasPos;

        bool m_ShouldLoadFirstLevel = true;
        bool m_IsBonusAreaBackground = false;
        void OnNewLevelLoaded() const;
        void FillVertexArray(const std::vector<char>& levelData, int rows, int cols, int colOffset) const;
    };
}
