#include "GameManager.h"
#include "../Core/GameState.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Components/Transform.h"

#include <format>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

void thomasWasLate::GameManager::SwitchPlayer()
{
    m_CurrentPlayer = static_cast<bool>(m_CurrentPlayer) ? CurrentPlayer::Thomas : CurrentPlayer::Bob;

    OnPlayerSwitchedEvent.Broadcast();
}

void thomasWasLate::GameManager::LoadLevel()
{
    ReadLevelInfo(LoadInformation());

    CreateWorldCollision();

    OnNewLevelLoadedEvent.Broadcast();
}

void thomasWasLate::GameManager::SetLevelCleared()
{
    ++m_CurrentLevel;

    OnNewLevelLoadedEvent.ClearListeners();
    OnPlayerSwitchedEvent.ClearListeners();
    diji::SceneManager::GetInstance().SetNextSceneToActivate(static_cast<int>(thomasWasLateState::Level));
}

std::string thomasWasLate::GameManager::LoadInformation()
{
    switch (m_CurrentLevel) // if you're going to read from a file put this information in the fucking file
    {
    case 1:
        m_StartPosition.x = 100;
        m_StartPosition.y = 100;
        m_LevelTimeLimit = 30.0f;
        break;
    case 2:
        m_StartPosition.x = 100;
        m_StartPosition.y = 3600;
        m_LevelTimeLimit = 100.0f;
        break;
    case 3:
        m_StartPosition.x = 1250;
        m_StartPosition.y = 0;
        m_LevelTimeLimit = 30.0f;
        break;
    case 4:
        m_StartPosition.x = 50;
        m_StartPosition.y = 200;
        m_LevelTimeLimit = 50.0f;
        break;
    default:
        throw std::runtime_error("Invalid Level.");
    }

    return std::format("../ThomasWasLate/Resources/levels/level{}.txt", m_CurrentLevel);
}

void thomasWasLate::GameManager::ReadLevelInfo(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
        throw std::runtime_error("Could not open level file: " + filepath);

    std::string line;
    m_Rows = 0;
    m_Cols = 0;
    m_LevelInfo = std::vector<int>();

    while (std::getline(file, line))
    {
        int colCount = 0;

        for (const char c : line)
        {
            if (std::isdigit(c))
            {
                int number = c - '0';
                m_LevelInfo.push_back(number);
                ++colCount;
            }
        }

        if (m_Rows == 0)
            m_Cols = colCount;

        ++m_Rows;
    }

    
    file.close();
}

void thomasWasLate::GameManager::CreateWorldCollision() const
{
    constexpr float kTileSize = 50.0f;

    // Clear previously created tagged colliders for tiles 2/3/4
    // m_TileColliders = std::vector<std::unique_ptr<diji::Collider>>();
    
    for (int row = 0; row < m_Rows; ++row)
    {
        int col = 0;
        while (col < m_Cols)
        {
            const int idx = row * m_Cols + col;
            const int tile = m_LevelInfo[idx];

            if (tile == 1)
            {
                const int startC = col;
                while (col < m_Cols && m_LevelInfo[row * m_Cols + col] == 1)
                    ++col;

                const int len = col - startC;

                const float left = static_cast<float>(startC) * kTileSize;
                const float bottom = static_cast<float>(row) * kTileSize;
                const float width  = static_cast<float>(len) * kTileSize;
                constexpr float height = kTileSize;
                
                sf::Vector2f center{ left + width * 0.5f, bottom + height * 0.5f };

                auto tempBound = std::make_unique<diji::GameObject>();
                tempBound->AddComponents<diji::Transform>(center);
                tempBound->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ static_cast<float>(len) * kTileSize, kTileSize });
                tempBound->GetComponent<diji::Collider>()->SetStatic(true);
                // tempBound->AddComponents<diji::ShapeRender>();

                (void)diji::SceneManager::GetInstance().SpawnGameObject("WorldCollider", std::move(tempBound), center);
            }
            else
            {
                // Create individual colliders for special tiles 2/3/4 with tags
                if (tile == 2 || tile == 3 || tile == 4)
                {
                    const float left = static_cast<float>(col) * kTileSize;
                    const float bottom = static_cast<float>(row) * kTileSize;
                    constexpr float width  = kTileSize;
                    constexpr float height = kTileSize;
                
                    sf::Vector2f center{ left + width * 0.5f, bottom + height * 0.5f };

                    // todo: set up overlap event collisions!
                    auto tempBound = std::make_unique<diji::GameObject>();
                    tempBound->AddComponents<diji::Transform>(center);
                    tempBound->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ width, height });
                    const auto collider = tempBound->GetComponent<diji::Collider>();
                    collider->SetStatic(true);
                    collider->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);

                    // tempBound->AddComponents<diji::ShapeRender>();
                   
                    switch (tile)
                    {
                    case 2:
                        collider->SetTag("lava");
                        break;
                    case 3:
                        collider->SetTag("water");
                        break;
                    case 4:
                        collider->SetTag("goal");
                        break;
                    default:
                        break;
                    }
                    
                    diji::SceneManager::GetInstance().SpawnGameObject("WorldCollider", std::move(tempBound), center);
                }
                ++col;
            }
        }

        if (row == m_Rows - 1)
        {
            const float width  = static_cast<float>(m_Cols) * 1.5f * kTileSize;
            const float left = -static_cast<float>(m_Cols) * kTileSize * 0.5f;
            const float bottom = static_cast<float>(row + 2) * kTileSize;
            constexpr float height = kTileSize;
            
            sf::Vector2f center{ left + width * 0.5f, bottom + height * 0.5f };

            auto tempBound = std::make_unique<diji::GameObject>();
            tempBound->AddComponents<diji::Transform>(center);
            tempBound->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ width, height });
            // tempBound->AddComponents<diji::ShapeRender>();
            
            const auto collider = tempBound->GetComponent<diji::Collider>();
            collider->SetStatic(true);
            collider->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);
            collider->SetTag("void");
            
            (void)diji::SceneManager::GetInstance().SpawnGameObject("WorldCollider", std::move(tempBound), center);
        }
    }
}
