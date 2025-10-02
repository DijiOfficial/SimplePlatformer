#include "Sprite.h"

#include <future>
#include <thread>

#include "../Core/Renderer.h"
#include "../Singleton/RandNumber.h"
#include "../Singleton/ResourceManager.h"

diji::Sprite::Sprite(GameObject* ownerPtr)
    : Component(ownerPtr)
{
}

diji::Sprite::Sprite(GameObject* ownerPtr, std::string filename)
    : Component(ownerPtr)
    , m_FilePath{ std::move(filename) }
{
}

diji::Sprite::Sprite(GameObject* ownerPtr, std::string filename, const int tileSize, const int tileCount)
    : Component(ownerPtr)
    , m_FilePath{ std::move(filename) }
    , m_TileSize{ tileSize }
    , m_TileCountX{ tileCount }
    , m_TileCountY{ tileCount }
{
}

diji::Sprite::Sprite(GameObject* ownerPtr, std::string filename, const int tileSize, const int tileCount, const int maxWidth, const int maxHeight)
    : Component(ownerPtr)
    , m_FilePath{ std::move(filename) }
    , m_TileSize{ tileSize }
    , m_TileCountX{ tileCount }
    , m_TileCountY{ tileCount }
    , m_MaxWidth{ maxWidth }
    , m_MaxHeight{ maxHeight }
{
}

void diji::Sprite::Init()
{
    if (not m_FilePath.empty())
        SetTexture(m_FilePath);

    m_VertexArray.setPrimitiveType(sf::PrimitiveType::Triangles);

    if (m_TileSize == 0 || (m_TileCountY + m_TileCountX) == 0 || m_MaxWidth == 0 || m_MaxHeight == 0)
        return;

    const int tilesX = m_MaxWidth / m_TileSize;
    const int tilesY = m_MaxHeight / m_TileSize;
    const int numVertices = tilesX * tilesY * TRIANGLE_VERTICES;
    m_VertexArray.resize(numVertices);
}

void diji::Sprite::SetTexture(const std::string& filename)
{
    m_SFMLTexture = ResourceManager::GetInstance().LoadTexture(filename);
}

void diji::Sprite::Render() const
{
    Renderer::GetInstance().RenderVertexArray(m_SFMLTexture, m_VertexArray); 
}

void diji::Sprite::GenerateMap()
{
    const int tilesX = m_MaxWidth / m_TileSize;
    const int tilesY = m_MaxHeight / m_TileSize;

    const auto& wall = m_WallSpritePosition;
    const sf::Vector2f wallTopLeft = sf::Vector2f{ wall.left, wall.top };
    const sf::Vector2f wallBottomRight = sf::Vector2f{ wall.left + wall.width, wall.height };

    // Helper lambdas
    auto SetVertices = [](sf::Vertex* quad, float left, float top, float right, float bottom)
    {
        // Triangle A (bottom-left triangle)
        quad[0].position = { left,  top    };
        quad[1].position = { right, bottom };
        quad[2].position = { left,  bottom };

        // Triangle B (top-right triangle)
        quad[3].position = { left,  top    };
        quad[4].position = { right, top    };
        quad[5].position = { right, bottom };
    };

    auto SetTexCoords = [](sf::Vertex* quad, const sf::Vector2f& texTopLeft, const sf::Vector2f& texBottomRight)
    {
        // Triangle A (bottom-left triangle)
        quad[0].texCoords = { texTopLeft.x,      texTopLeft.y };
        quad[1].texCoords = { texBottomRight.x,  texBottomRight.y };
        quad[2].texCoords = { texTopLeft.x,      texBottomRight.y };

        // Triangle B (top-right triangle)
        quad[3].texCoords = { texTopLeft.x,      texTopLeft.y };
        quad[4].texCoords = { texBottomRight.x,  texTopLeft.y };
        quad[5].texCoords = { texBottomRight.x,  texBottomRight.y };
    };

    // threading the map generation
    const int threadCount = static_cast<int>(std::thread::hardware_concurrency());
    const int rowsPerThread = tilesY / threadCount + 1;
    std::vector<std::future<void>> tasks;

    for (int t = 0; t < threadCount; ++t)
    {
        const int startY = t * rowsPerThread;
        const int endY = std::min(tilesY, startY + rowsPerThread);

        tasks.emplace_back(std::async(std::launch::async, [startY, endY, tilesX, SetVertices, tilesY, SetTexCoords, wallTopLeft, wallBottomRight, wall, this]()
        {
            for (int y = startY; y < endY; ++y)
            {
                for (int x = 0; x < tilesX; ++x)
                {
                    const std::size_t tileIndex = static_cast<std::size_t>(y) * static_cast<std::size_t>(tilesX) + x;
                    sf::Vertex* quad = &m_VertexArray[tileIndex * TRIANGLE_VERTICES];

                    // Get the positions of the vertices for the current tile and set them
                    const float left   = static_cast<float>(x * m_TileSize);
                    const float top    = static_cast<float>(y * m_TileSize);
                    const float right  = static_cast<float>(m_TileSize) + left;
                    const float bottom = static_cast<float>(m_TileSize) + top;

                    SetVertices(quad, left, top, right, bottom);

                    // if the tile is on the border, use the wall texture
                    if (x == 0 || x == tilesX - 1 || y == 0 || y == tilesY - 1)
                    {
                        SetTexCoords(quad, wallTopLeft, wallBottomRight);
                    }
                    else
                    {
                        // else generate a random terrain tile
                        const int randX = RandNumber::GetRandomRangeInt(0, m_TileCountX - 1);
                        const int maxTerrainY = static_cast<int>(wall.top / static_cast<float>(m_TileSize)) - 1;
                        const int randY = RandNumber::GetRandomRangeInt(0, maxTerrainY);
                        
                        sf::Vector2f terrainTopLeft  = { static_cast<float>(randX * m_TileSize), static_cast<float>(randY * m_TileSize) };
                        sf::Vector2f terrainBottomRight = terrainTopLeft + sf::Vector2f{ static_cast<float>(m_TileSize), static_cast<float>(m_TileSize) };
                        SetTexCoords(quad, terrainTopLeft, terrainBottomRight);
                    }
                }
            }
        }));
    }

    for (auto& f : tasks)
        f.get(); // wait for all threads to finish
}