#include "BackgroundHandler.h"

#include "../Singletons/GameManager.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/Sprite.h"

void thomasWasLate::BackgroundHandler::Init()
{
    m_BackgroundSprite = GetOwner()->GetComponent<diji::Sprite>();

    GameManager::GetInstance().OnNewLevelLoadedEvent.AddListener(this, &BackgroundHandler::OnNewLevelLoaded);
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
            quad[0].position = { left,  top    };
            quad[1].position = { right, bottom };
            quad[2].position = { left,  bottom };

            // Triangle B
            quad[3].position = { left,  top    };
            quad[4].position = { right, top    };
            quad[5].position = { right, bottom };

            // Texture coordinates
            float texTop    = static_cast<float>(tileID * tileSize);
            float texBottom = texTop + tileSize;

            // Triangle A texCoords
            quad[0].texCoords = { 0.f, texTop };
            quad[1].texCoords = { static_cast<float>(tileSize), texBottom };
            quad[2].texCoords = { 0.f, texBottom };

            // Triangle B texCoords
            quad[3].texCoords = { 0.f, texTop };
            quad[4].texCoords = { static_cast<float>(tileSize), texTop };
            quad[5].texCoords = { static_cast<float>(tileSize), texBottom };
        }
    }

    gameManager.ClearLevelInfo();
}
