#include "BackgroundHandler.h"

#include "../Singletons/GameManager.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/Sprite.h"

void thomasWasLate::BackgroundHandler::Init()
{
    m_BackgroundSprite = GetOwner()->GetComponent<diji::Sprite>();

    GameManager::GetInstance().OnNewLevelLoadedEvent.AddListener(this, &BackgroundHandler::OnNewLevelLoaded);

    m_TileIDToAtlasPos =
    {
        { 0,  {3, 0} }, // Empty tile
        { 1,  {0, 0} }, // Ground tile
        { 2,  {0, 1} }, // Stair tile
        { 3,  {1, 0} }, // TopLeft Up Pipe
        { 4,  {2, 0} }, // TopRight Up Pipe
        { 5,  {1, 1} }, // MiddleLeft Up Pipe
        { 6,  {2, 1} }, // MiddleRight Up Pipe
        { 7,  {0, 2} }, // TopRight Side Pipe
        { 8,  {1, 2} }, // MiddleRight Side Pipe
        { 9,  {2, 2} }, // BottomRight Side Pipe
        { 10, {0, 3} }, // TopLeft Side Pipe
        { 11, {1, 3} }, // MiddleLeft Side Pipe
        { 12, {2, 3} }, // BottomLeft Side Pipe
        // ... etc.
    };

}

void thomasWasLate::BackgroundHandler::Start()
{
    GameManager::GetInstance().LoadLevel();
}

void thomasWasLate::BackgroundHandler::OnNewLevelLoaded() const
{
    auto& gameManager = GameManager::GetInstance();
    constexpr int tileSize = 50;

    m_BackgroundSprite->SetTileSize(tileSize);
    m_BackgroundSprite->SetTileCount(gameManager.GetRows(), gameManager.GetCols());
    m_BackgroundSprite->ResizeVertexArray();

    const std::vector<int>& levelData = gameManager.GetLevelInfo();
    const int cols = gameManager.GetCols();
    const int rows = gameManager.GetRows();

    auto& tempVertexArray = m_BackgroundSprite->GetVertexArray();

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            const int tileID = levelData[y * cols + x];
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
            const float texTop    = static_cast<float>(atlasPos.y * tileSize);
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

// void thomasWasLate::BackgroundHandler::OnNewLevelLoaded() const
// {
//     auto& gameManager = GameManager::GetInstance();
//     constexpr int tileSize = 50;
//
//     m_BackgroundSprite->SetTileSize(tileSize);
//     m_BackgroundSprite->SetTileCount(gameManager.GetRows(), gameManager.GetCols());
//     m_BackgroundSprite->ResizeVertexArray();
//
//     const std::vector<int>& levelData = gameManager.GetLevelInfo();
//     const int cols = gameManager.GetCols();
//     const int rows = gameManager.GetRows();
//
//     auto& tempVertexArray = m_BackgroundSprite->GetVertexArray();
//
//     for (int y = 0; y < rows; ++y)
//     {
//         for (int x = 0; x < cols; ++x)
//         {
//             const int tileID = levelData[y * cols + x];
//
//             const int vertexIndex = (y * cols + x) * 6;
//             sf::Vertex* quad = &tempVertexArray[vertexIndex];
//
//             float left   = static_cast<float>(x * tileSize);
//             float top    = static_cast<float>(y * tileSize);
//             float right  = left + tileSize;
//             float bottom = top + tileSize;
//
//             // Triangle A
//             quad[0].position = { left,  top    };
//             quad[1].position = { right, bottom };
//             quad[2].position = { left,  bottom };
//
//             // Triangle B
//             quad[3].position = { left,  top    };
//             quad[4].position = { right, top    };
//             quad[5].position = { right, bottom };
//
//             // Texture coordinates
//             float texTop    = static_cast<float>(tileID * tileSize);
//             float texBottom = texTop + tileSize;
//
//             // Triangle A texCoords
//             quad[0].texCoords = { 0.f, texTop };
//             quad[1].texCoords = { static_cast<float>(tileSize), texBottom };
//             quad[2].texCoords = { 0.f, texBottom };
//
//             // Triangle B texCoords
//             quad[3].texCoords = { 0.f, texTop };
//             quad[4].texCoords = { static_cast<float>(tileSize), texTop };
//             quad[5].texCoords = { static_cast<float>(tileSize), texBottom };
//         }
//     }
//
//     gameManager.ClearLevelInfo();
// }
