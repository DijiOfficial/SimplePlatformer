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
            { {1, 4}, 'x' },
            { {1, 5}, 'w' },
            { {1, 6}, 'G' },
            { {1, 7}, 'H' },
            { {1, 8}, '!' },
            { {2, 0}, '4' },
            { {2, 1}, '6' },
            { {2, 2}, '9' },
            { {2, 3}, 'c' },
            { {2, 4}, '0' },
            { {2, 5}, '0' },
            { {2, 6}, '0' },
            { {2, 7}, '0' },
            { {2, 8}, '"' },
            { {3, 0}, 'k' },
            { {3, 1}, 'j' },
            { {3, 2}, '0' },
            { {3, 3}, '0' },
            { {3, 4}, '0' },
            { {3, 5}, '0' },
            { {3, 6}, '0' },
            { {3, 7}, '0' },
            { {3, 8}, '$' },
        }; 
    };
}
