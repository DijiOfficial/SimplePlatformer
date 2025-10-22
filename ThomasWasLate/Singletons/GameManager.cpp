#include "GameManager.h"
#include "../Core/GameState.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/TextComp.h"
#include "Engine/Components/Render.h"
#include "Engine/Core/GameObject.h"
#include "../Components/Other/PointsBehaviour.h"
#include "../Components/Blocks/LuckyBlock.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "../Components/Blocks/BreakableBlock.h"
#include "../Components/Blocks/MultiCoinBlock.h"
#include "../Components/Enemies/GoombaAI.h"

#include <format>
#include <fstream>

#include "../Components/Blocks/StarBlock.h"
#include "../Components/PowerUps/StartPower.h"

namespace thomasWasLate
{
    class GoombaAI;
    class PointsBehaviour;
}

void thomasWasLate::GameManager::LoadLevel()
{
    m_CurrentPlayerState = PlayerHealthState::Small;
    ReadLevelInfo(LoadInformation());

    CreateWorldCollision();

    OnNewLevelLoadedEvent.Broadcast();
}

void thomasWasLate::GameManager::SetLevelCleared()
{
    ++m_CurrentLevel;

    ResetLevel();
}

void thomasWasLate::GameManager::ResetLevel()
{
    // I don't think I need to clear them anymore?
    OnNewLevelLoadedEvent.ClearAllListeners();
    OnPlayerSwitchedEvent.ClearAllListeners();
    OnScoreAddedEvent.ClearAllListeners();
    OnCoinCollectedEvent.ClearAllListeners();
    
    diji::SceneManager::GetInstance().SetNextSceneToActivate(static_cast<int>(thomasWasLateState::Level));
    m_TotalFireballsInLevel = 0;
}

void thomasWasLate::GameManager::SwitchCurrentPlayerState()
{
    if (m_CurrentPlayerState == PlayerHealthState::Small)
        m_CurrentPlayerState = PlayerHealthState::Big;
    else
        m_CurrentPlayerState = PlayerHealthState::Small;
}

void thomasWasLate::GameManager::SpawnPointsText(const sf::Vector2f& position, const std::string& score)
{
    sf::Vector2f screenPos = diji::SceneManager::GetInstance().GetScreenPosition(position);
    // screenPos.y += static_cast<float>(window::VIEWPORT.y) * 0.5f;
    auto pointsText = std::make_unique<diji::GameObject>();
    pointsText->AddComponents<diji::Transform>(screenPos);
    pointsText->AddComponents<diji::TextComp>(score, "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    pointsText->GetComponent<diji::TextComp>()->GetText().setCharacterSize(18);
    pointsText->AddComponents<diji::Render>();
    pointsText->AddComponents<PointsBehaviour>();

    diji::SceneManager::GetInstance().AddGameObjectToCanvas("ZZ_pointsText", std::move(pointsText), screenPos);
}

std::string thomasWasLate::GameManager::LoadInformation()
{
    switch (m_CurrentLevel) // if you're going to read from a file put this information in the fucking file
    {
    case 1:
        m_StartPosition.x = 100;
        m_StartPosition.y = 100;
        break;
    case 2:
        m_StartPosition.x = 100;
        m_StartPosition.y = 3600;
        break;
    case 3:
        m_StartPosition.x = 1250;
        m_StartPosition.y = 0;
        break;
    case 4:
        m_StartPosition.x = 50;
        m_StartPosition.y = 200;
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
    m_LevelInfo = std::vector<char>();

    while (std::getline(file, line))
    {
        int colCount = 0;

        for (const char c : line)
        {
            m_LevelInfo.push_back(c);
            ++colCount;
        }

        if (m_Rows == 0)
            m_Cols = colCount;

        ++m_Rows;
    }

    
    file.close();
}

// todo: rename, it also creates enemeis
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
            const char tile = m_LevelInfo[idx];

            if (std::string("0edxyfghi").find(tile) == std::string::npos)
            {
                const int startC = col;
                while (col < m_Cols && m_LevelInfo[row * m_Cols + col] != '0') // == tile? will work but colliders like pipes will become separate
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
                const auto collider = tempBound->GetComponent<diji::Collider>();
                collider->SetStatic(true);
                // tempBound->AddComponents<diji::ShapeRender>();

                collider->SetTag("ground");

                (void)diji::SceneManager::GetInstance().SpawnGameObject("WorldCollider", std::move(tempBound), center);
            }
            else if (tile == 'e' || tile == 'x' || tile == 'y')
            {
                const float left = static_cast<float>(col) * kTileSize;
                const float bottom = static_cast<float>(row) * kTileSize;
                // constexpr float width  = kTileSize;
                // constexpr float height = kTileSize;
                // sf::Vector2f center{ left + width * 0.5f, bottom + height * 0.5f };
                sf::Vector2f center{ left + 25.f, bottom + + 25.f };

                auto luckyBlock = std::make_unique<diji::GameObject>();
                luckyBlock->AddComponents<diji::Transform>(500, 200);
                luckyBlock->AddComponents<diji::SpriteRenderComponent>("graphics/luckyBlock.png", sf::Vector2i{ 50,50 }, 3, 0.25f);
                luckyBlock->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
                const auto collider = luckyBlock->GetComponent<diji::Collider>();
                collider->SetTag("luckyBlock");
                collider->SetAffectedByGravity(false);
                collider->SetGenerateHitEvents(true);
                collider->SetIsMoveable(false);
                luckyBlock->AddComponents<LuckyBlock>();
                if (tile == 'x')
                    luckyBlock->GetComponent<LuckyBlock>()->SetAsPowerUpBlock();

                (void)diji::SceneManager::GetInstance().SpawnGameObject("E_luckyBlock", std::move(luckyBlock), center);

                ++col;
            }
            // else if (tile == 'd')
            // {
            //     int startC = col;
            //     while (startC < m_Cols && m_LevelInfo[row * m_Cols + startC] == 'd')
            //         ++startC;
            //
            //     const int len = startC - col;
            //     const float left = static_cast<float>(col) * kTileSize;
            //     const float top  = static_cast<float>(row) * kTileSize;
            //     const float width  = static_cast<float>(len) * kTileSize;
            //     constexpr float height = kTileSize * 0.5f;
            //     sf::Vector2f bigCenter{ left + width * 0.5f, top + height * 0.5f };
            //
            //     // Create the big top collider
            //     auto longColliderObj = std::make_unique<diji::GameObject>();
            //     longColliderObj->AddComponents<diji::Transform>(bigCenter);
            //     longColliderObj->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ width, height });
            //     longColliderObj->AddComponents<diji::ShapeRender>();
            //     diji::Collider* longCol = longColliderObj->GetComponent<diji::Collider>();
            //     longCol->SetStatic(true);
            //     longCol->SetTag("ground");
            //     diji::SceneManager::GetInstance().SpawnGameObject("WorldCollider", std::move(longColliderObj), bigCenter);
            //
            //     // Now create each breakable block in this run and mark them to ignore the long collider
            //     for (int i = 0; i < len; ++i)
            //     {
            //         const float blockLeft = static_cast<float>(col + i) * kTileSize;
            //         const float blockTop  = static_cast<float>(row) * kTileSize;
            //         constexpr float blockSize = kTileSize;
            //         sf::Vector2f blockCenter{ blockLeft + blockSize * 0.5f, blockTop + blockSize * 0.5f };
            //
            //         auto breakableBlock = std::make_unique<diji::GameObject>();
            //         breakableBlock->AddComponents<diji::Transform>(blockCenter);
            //         breakableBlock->AddComponents<diji::TextureComp>("graphics/breakableBlock.png");
            //         breakableBlock->AddComponents<diji::Render>();
            //         breakableBlock->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ blockSize, blockSize });
            //         breakableBlock->AddComponents<diji::ShapeRender>(true);
            //         const auto collider = breakableBlock->GetComponent<diji::Collider>();
            //         collider->SetTag("breakBlock");
            //         collider->SetAffectedByGravity(false);
            //         collider->SetGenerateHitEvents(true);
            //         collider->SetIsMoveable(false);
            //
            //         collider->IgnoreCollider(longCol);
            //         longCol->IgnoreCollider(collider);
            //
            //         breakableBlock->AddComponents<BreakableBlock>();
            //
            //         diji::SceneManager::GetInstance().SpawnGameObject("E_breakableBlock", std::move(breakableBlock), blockCenter);
            //     }
            //
            //     col = startC;
            // }

            else if (tile == 'd')
            {
                const float left = static_cast<float>(col) * kTileSize;
                const float bottom = static_cast<float>(row) * kTileSize;
                // constexpr float width  = kTileSize;
                // constexpr float height = kTileSize;
                // sf::Vector2f center{ left + width * 0.5f, bottom + height * 0.5f };
                sf::Vector2f center{ left + 25.f, bottom + + 25.f };
            
                auto breakableBlock = std::make_unique<diji::GameObject>();
                breakableBlock->AddComponents<diji::Transform>(600, 300);
                breakableBlock->AddComponents<diji::SpriteRenderComponent>("graphics/breakableBlock.png", sf::Vector2i{ 50, 50 }, 1, 0.0f);
                breakableBlock->GetComponent<diji::SpriteRenderComponent>()->SetLooping(false);
                breakableBlock->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
                const auto collider = breakableBlock->GetComponent<diji::Collider>();
                collider->SetTag("breakBlock");
                collider->SetAffectedByGravity(false);
                collider->SetGenerateHitEvents(true);
                collider->SetIsMoveable(false);
                breakableBlock->AddComponents<BreakableBlock>();
            
                (void)diji::SceneManager::GetInstance().SpawnGameObject("E_breakableBlock", std::move(breakableBlock), center);
                
                ++col;
            }
            else if (tile == 'f' || tile == 'i')
            {
                const float left = static_cast<float>(col) * kTileSize;
                const float bottom = static_cast<float>(row) * kTileSize;
                sf::Vector2f center{ left + 25.f, bottom + 25.f };
                
                auto multiCoinBlock = std::make_unique<diji::GameObject>();
                multiCoinBlock->AddComponents<diji::Transform>(600, 300);
                multiCoinBlock->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
                multiCoinBlock->AddComponents<diji::SpriteRenderComponent>("graphics/breakableBlock.png", sf::Vector2i{ 50, 50 }, 1, 0.0f);
                multiCoinBlock->GetComponent<diji::SpriteRenderComponent>()->SetLooping(false);
                const auto collider = multiCoinBlock->GetComponent<diji::Collider>();
                collider->SetTag("breakBlock");
                collider->SetAffectedByGravity(false);
                collider->SetGenerateHitEvents(true);
                collider->SetIsMoveable(false);

                if (tile == 'i')
                    multiCoinBlock->AddComponents<StarBlock>();
                else
                    multiCoinBlock->AddComponents<MultiCoinBlock>();

                (void)diji::SceneManager::GetInstance().SpawnGameObject("E_MultiCoinBlock", std::move(multiCoinBlock), center);
                ++col;
            }
            else if (tile == 'g' || tile == 'h')
            {
                const float left = static_cast<float>(col) * kTileSize;
                const float bottom = static_cast<float>(row) * kTileSize;
                sf::Vector2f center{ left + 25.f, bottom + + 25.f };

                auto goomba = std::make_unique<diji::GameObject>();
                goomba->AddComponents<diji::Transform>(2000, 0);
                goomba->AddComponents<diji::SpriteRenderComponent>("graphics/goomba.png", sf::Vector2i{ 50,50 }, 2, 0.15f);
                goomba->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
                const auto collider = goomba->GetComponent<diji::Collider>();
                collider->SetRestitution(0.f);
                collider->SetMass(0.89f);
                collider->SetStaticFriction(0.25f);
                collider->SetKineticFriction(0.15f);
                collider->SetMaxVelocity(sf::Vector2f{ 400.f, 800.f });
                collider->SetGenerateHitEvents(true);
                collider->SetTag("enemy");
                goomba->AddComponents<GoombaAI>();
                goomba->GetComponent<GoombaAI>()->SetActivationMilestone(col - 20);
                goomba->SetActive(false);

                if (tile == 'h')
                {
                    center.x += 25.f;
                    goomba->GetComponent<GoombaAI>()->SetActivationMilestone(col - 21);
                }

                (void)diji::SceneManager::GetInstance().SpawnGameObject("E_MultiCoinBlock", std::move(goomba), center);

                ++col;
            }
            else
            {
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
