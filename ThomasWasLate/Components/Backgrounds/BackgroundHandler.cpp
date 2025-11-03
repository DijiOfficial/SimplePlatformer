#include "BackgroundHandler.h"

#include "../../Singletons/GameManager.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/Sprite.h"

void thomasWasLate::BackgroundHandler::Init()
{
    m_BackgroundSprite = GetOwner()->GetComponent<diji::Sprite>();

    GameManager::GetInstance().OnNewLevelLoadedEvent.AddListener(this, &BackgroundHandler::OnNewLevelLoaded);

    m_TileIDToAtlasPos =
    {
        { '0', {0, 4} }, // Empty tile
        { '1', {0, 0} }, // Ground tile
        { '2', {0, 1} }, // Stair tile
        { '3', {1, 0} }, // TopLeft Up Pipe
        { '4', {2, 0} }, // TopRight Up Pipe
        { '5', {1, 1} }, // MiddleLeft Up Pipe
        { '6', {2, 1} }, // MiddleRight Up Pipe
        { '7', {0, 2} }, // TopRight Side Pipe
        { '8', {1, 2} }, // MiddleRight Side Pipe
        { '9', {2, 2} }, // BottomRight Side Pipe
        { 'a', {0, 3} }, // TopLeft Side Pipe
        { 'b', {1, 3} }, // MiddleLeft Side Pipe
        { 'c', {2, 3} }, // BottomLeft Side Pipe
        { 'd', {0, 4} }, // LuckyBlock (invisible tile because it's rendered by LuckyBlock component)
        { 'x', {0, 4} }, // LuckyBlock with a power up
        { 'y', {0, 4} }, // LuckyBlock with a star power up
        { 'e', {0, 4} }, // BreakableBlock (invisible tile because it's rendered by BreakableBlock component)
        { 'f', {0, 4} }, // MultiCoinBlock (invisible tile because it's rendered by BreakableBlock component)
        { 'g', {0, 4} }, // Goomba (fitting isn't it?) (invisible tile because it's not a background tile)
        { 'h', {0, 4} }, // Goomba but offset by 25 (invisible tile because it's not a background tile)
        { 'i', {0, 4} }, // Star block
        { 'k', {0, 4} }, // pole top
        { 'j', {3, 1} }, // pole body
        { 'l', {0, 4} }, // small castle base
        { 'm', {0, 4} }, // big castle base
        { 'n', {0, 4} }, // Hidden 1up mushroom
        { 'o', {0, 4} }, // Koopa Troopa
        { 'p', {0, 4} }, // Koopa Troopa offset by 25
        { 'q', {0, 4} }, // Static Coin
        { 'r', {0, 4} }, // CheckPoint
        { 's', {1, 0} }, // Piranha Plant (using top left pipe tile as it will replace the pipe tile to spawn the plant)
        { 't', {0, 4} }, // Hidden Block
        { 'u', {0, 4} }, // Brick Star
        { 'v', {0, 4} }, // Brick PowerUp
        { 'D', {0, 2} }, // TopRight Side Pipe (Z priority)
        { 'E', {1, 2} }, // MiddleRight Side Pipe (Z priority)
        { 'F', {2, 2} }, // BottomRight Side Pipe (Z priority)
        { 'A', {0, 3} }, // TopLeft Side Pipe (Z priority)
        { 'B', {1, 3} }, // MiddleLeft Side Pipe (Z priority)
        { 'C', {2, 3} }, // BottomLeft Side Pipe (Z priority)
        // ... etc.
    };

    GameManager::GetInstance().LoadLevel();
}

void thomasWasLate::BackgroundHandler::OnNewLevelLoaded() const
{
    auto& gameManager = GameManager::GetInstance();
    constexpr int tileSize = 50;

    m_BackgroundSprite->SetTileSize(tileSize);
    m_BackgroundSprite->SetTileCount(gameManager.GetRows(), gameManager.GetCols());
    m_BackgroundSprite->ResizeVertexArray();

    const std::vector<char>& levelData = gameManager.GetLevelInfo();
    const int cols = gameManager.GetCols();
    const int rows = gameManager.GetRows();

    auto& tempVertexArray = m_BackgroundSprite->GetVertexArray();

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            const char tileID = levelData[y * cols + x];
            const int vertexIndex = (y * cols + x) * 6;
            sf::Vertex* quad = &tempVertexArray[vertexIndex];

            float left   = static_cast<float>(x * tileSize);
            float top    = static_cast<float>(y * tileSize);
            float right  = left + tileSize;
            float bottom = top + tileSize;

            // Triangle A
            quad[0].position = { left,  top };
            quad[1].position = { right, bottom };
            quad[2].position = { left,  bottom };

            // Triangle B
            quad[3].position = { left,  top };
            quad[4].position = { right, top };
            quad[5].position = { right, bottom };

            // Lookup tile position in texture atlas
            const auto it = m_TileIDToAtlasPos.find(tileID);
            if (it == m_TileIDToAtlasPos.end())
            {
                // Optional: fallback or skip if tile ID not mapped
                continue;
            }

            const sf::Vector2i& atlasPos = it->second;

            const float texLeft   = static_cast<float>(atlasPos.x * tileSize);
            const float texTop    = static_cast<float>(atlasPos.y * tileSize) + 1;
            const float texRight  = texLeft + tileSize;
            const float texBottom = texTop + tileSize;

            // Triangle A texCoords
            quad[0].texCoords = { texLeft, texTop };
            quad[1].texCoords = { texRight, texBottom };
            quad[2].texCoords = { texLeft, texBottom };

            // Triangle B texCoords
            quad[3].texCoords = { texLeft, texTop };
            quad[4].texCoords = { texRight, texTop };
            quad[5].texCoords = { texRight, texBottom };
        }
    }

    gameManager.ClearLevelInfo();
}