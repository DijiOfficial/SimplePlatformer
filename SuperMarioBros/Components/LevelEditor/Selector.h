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
        
        struct Vector2iHash
        {
            std::size_t operator()(const sf::Vector2i& v) const noexcept
            {
                const std::size_t h1 = std::hash<int>()(v.x);
                const std::size_t h2 = std::hash<int>()(v.y);
                return h1 ^ (h2 << 1);
            }
        };
        std::unordered_map<sf::Vector2i, char, Vector2iHash> m_AtlasToPosMap =
        {
            { {0, 0}, '0' },
            { {0, 1}, '0' },
            { {0, 2}, '7' },
            { {0, 3}, 'a' },
            { {0, 4}, 'e' },
            { {0, 5}, '1' },
            { {0, 6}, '2' },
            { {0, 7}, 'd' },
            { {0, 8}, 'q' },
            { {1, 0}, '3' },
            { {1, 1}, '5' },
            { {1, 2}, '8' },
            { {1, 3}, 'b' },
            { {1, 4}, '0' },
            { {1, 5}, 'w' },
            { {1, 6}, 'G' },
            { {1, 7}, 'H' },
            { {1, 8}, '0' },
            { {2, 0}, '4' },
            { {2, 1}, '6' },
            { {2, 2}, '9' },
            { {2, 3}, 'c' },
            { {2, 4}, '0' },
            { {2, 5}, '0' },
            { {2, 6}, '0' },
            { {2, 7}, '0' },
            { {2, 8}, '0' },
            { {3, 0}, 'k' },
            { {3, 1}, 'j' },
            { {3, 2}, '0' },
            { {3, 3}, '0' },
            { {3, 4}, '0' },
            { {3, 5}, '0' },
            { {3, 6}, '0' },
            { {3, 7}, '0' },
            { {3, 8}, '0' },
            // { '0', {0, 4} }, // Empty tile
            // { '1', {0, 0} }, // Ground tile
            // { '2', {0, 1} }, // Stair tile
            // { '3', {1, 0} }, // TopLeft Up Pipe
            // { '4', {2, 0} }, // TopRight Up Pipe
            // { '5', {1, 1} }, // MiddleLeft Up Pipe
            // { '6', {2, 1} }, // MiddleRight Up Pipe
            // { '7', {0, 2} }, // TopRight Side Pipe
            // { '8', {1, 2} }, // MiddleRight Side Pipe
            // { '9', {2, 2} }, // BottomRight Side Pipe
            // { 'a', {0, 3} }, // TopLeft Side Pipe
            // { 'b', {1, 3} }, // MiddleLeft Side Pipe
            // { 'c', {2, 3} }, // BottomLeft Side Pipe
            // { 'd', {0, 4} }, // LuckyBlock (invisible tile because it's rendered by LuckyBlock component)
            // { 'x', {0, 4} }, // LuckyBlock with a power up
            // { 'y', {0, 4} }, // LuckyBlock with a star power up
            // { 'e', {0, 4} }, // BreakableBlock (invisible tile because it's rendered by BreakableBlock component)
            // { 'f', {0, 4} }, // MultiCoinBlock (invisible tile because it's rendered by BreakableBlock component)
            // { 'g', {0, 4} }, // Goomba (fitting isn't it?) (invisible tile because it's not a background tile)
            // { 'h', {0, 4} }, // Goomba but offset by 25 (invisible tile because it's not a background tile)
            // { 'i', {0, 4} }, // Star block
            // { 'k', {0, 4} }, // pole top
            // { 'j', {3, 1} }, // pole body
            // { 'l', {0, 4} }, // small castle base
            // { 'm', {0, 4} }, // big castle base
            // { 'n', {0, 4} }, // Hidden 1up mushroom
            // { 'o', {0, 4} }, // Koopa Troopa
            // { 'p', {0, 4} }, // Koopa Troopa offset by 25
            // { 'q', {0, 4} }, // Static Coin
            // { 'r', {0, 4} }, // CheckPoint
            // { 's', {1, 0} }, // Piranha Plant (using top left pipe tile as it will replace the pipe tile to spawn the plant)
            // { 't', {0, 4} }, // Hidden Block
            // { 'u', {0, 4} }, // Brick Star
            // { 'v', {0, 4} }, // Brick PowerUp
            // { 'D', {0, 2} }, // TopRight Side Pipe (Z priority)
            // { 'E', {1, 2} }, // MiddleRight Side Pipe (Z priority)
            // { 'F', {2, 2} }, // BottomRight Side Pipe (Z priority)
            // { 'A', {0, 3} }, // TopLeft Side Pipe (Z priority)
            // { 'B', {1, 3} }, // MiddleLeft Side Pipe (Z priority)
            // { 'C', {2, 3} }, // BottomLeft Side Pipe (Z priority)
        }; 
    };
}
