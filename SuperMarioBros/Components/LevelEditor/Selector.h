#pragma once
#include "Engine/Components/Component.h"

namespace diji
{
    class Transform;
    class SpriteRenderComponent;
}

namespace superMarioBros
{
    class BackgroundHandler;

    class Selector final : public diji::Component
    {
    public:
        explicit Selector(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~Selector() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override;
            
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void SetFramePosition(int frameX, int frameY);
        void ActivateBackgroundTexture() const;
        void DeactivateBackgroundTexture() const;

        void TryPlaceItem();
        void TryHoldPlaceItem();
        void SetBackgroundHandlerRef(BackgroundHandler* backgroundHandler) { m_BackgroundHandlerRef = backgroundHandler; }
        void UpdatePreviewItems();
        void EnablePreview() const;
        void DisablePreview() const;

    private:
        sf::Vector2i m_CurrentFramePos{ -1, -1 };
        diji::Transform* m_TransformCompPtr = nullptr;
        diji::GameObject* m_TextureGO = nullptr;
        diji::SpriteRenderComponent* m_SpriteRenderComp = nullptr;
        diji::SpriteRenderComponent* m_TemplateSpriteRenderComp = nullptr;
        BackgroundHandler* m_BackgroundHandlerRef = nullptr;
        std::unique_ptr<diji::GameObject> m_ItemTemplateUPtr = nullptr;
        bool m_IsHolding = false;
        int m_StartingRow = 0;
        int m_StartingCol = 0;
        sf::Vector2f m_StartingPos{ 0.f, 0.f };

        void CreateBackgroundTexture();

        struct GridPos
        {
            int row;
            int col;

            bool operator==(const GridPos&) const = default;
        };
        struct GridPosHasher
        {
            size_t operator()(const GridPos& pos) const noexcept
            {
                const size_t h1 = std::hash<int>{}(pos.row);
                const size_t h2 = std::hash<int>{}(pos.col);
                return h1 ^ (h2 << 1);
            }
        };
        std::unordered_map<GridPos, diji::GameObject*, GridPosHasher> m_PreviewItemsMap;
        std::unordered_map<GridPos, std::unordered_set<const diji::GameObject*>, GridPosHasher> m_PlacedItemsMap;
        bool HandleSpecialItems(char item, const GridPos& gridPos);
        void CreateItemAtGridPos(char itemChar, const GridPos& gridPos);
        void CreateAllSpecialBlocks();
        void TryDeletePlacedItem(const GridPos& gridPos);
        void HandleCreatingSpecialBlocks(const char itemChar, const GridPos& gridPos);

        struct Vector2iHash
        {
            std::size_t operator()(const sf::Vector2i& v) const noexcept
            {
                const std::size_t h1 = std::hash<int>()(v.x);
                const std::size_t h2 = std::hash<int>()(v.y);
                return h1 ^ (h2 << 1);
            }
        };
        std::unordered_map<sf::Vector2i, char, Vector2iHash> m_GridToCharMap =
        {
            { {0, 0}, '0' },
            { {0, 1}, '0' },
            { {0, 2}, '7' }, // Side Pipe Top Right
            { {0, 3}, 'a' }, // Side Pipe Top Left
            { {0, 4}, 'e' }, // Lucky Block Coin
            { {0, 5}, '1' }, // Floor
            { {0, 6}, '2' }, // Stair
            { {0, 7}, 'd' }, // Breakable Block
            { {0, 8}, 'q' }, // Static Coin
            { {1, 0}, '3' }, // Pipe Top Left
            { {1, 1}, '5' }, // Pipe Middle Left
            { {1, 2}, '8' }, // Side Pipe Middle Right
            { {1, 3}, 'b' }, // Side Pipe Middle Left
            { {1, 4}, 'x' }, // Lucky Block Power Up
            { {1, 5}, 'w' }, // Ground Green
            { {1, 6}, 'G' }, // Stair Green
            { {1, 7}, 'H' }, // Breakable Block Green
            { {1, 8}, '!' }, // Mushroom / Power Up
            { {2, 0}, '4' }, // Pipe Top Right
            { {2, 1}, '6' }, // Pipe Middle Right
            { {2, 2}, '9' }, // Side Pipe Bottom Right
            { {2, 3}, 'c' }, // Side Pipe Bottom Left
            { {2, 4}, '0' },  
            { {2, 5}, '0' },
            { {2, 6}, '0' },
            { {2, 7}, '0' },
            { {2, 8}, '"' }, // 1up Mushroom
            { {3, 0}, 'k' }, // Flag
            { {3, 1}, 'j' }, // Pole
            { {3, 2}, '0' },
            { {3, 3}, '0' },
            { {3, 4}, '0' },
            { {3, 5}, '0' },
            { {3, 6}, '0' },
            { {3, 7}, '0' },
            { {3, 8}, '$' }, // Star
        };

        std::unordered_map<char, sf::Vector2i> m_CharToGridMap =
        {
            { 'x', {1, 4} }, // Lucky Block Power Up
            { 'f', {0, 8} }, // Static Coin
            { 'z', {0, 8} }, // Static Coin
            { 'i', {3, 8} }, // Star
            { 'u', {3, 8} }, // Star
            { 'v', {1, 8} }, // Mushroom
            { 'I', {1, 8} }, // Mushroom
        };
    };
}
