#include "GameManager.h"
#include "../Core/GameState.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/TextComp.h"
#include "Engine/Components/TextureComp.h"
#include "Engine/Components/Render.h"
#include "Engine/Core/GameObject.h"
#include "../Components/Other/PointsBehaviour.h"
#include "../Components/Blocks/LuckyBlock.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "../Components/Blocks/BreakableBlock.h"
#include "../Components/Blocks/MultiCoinBlock.h"
#include "../Components/Enemies/GoombaAI.h"
#include "../Components/Blocks/StarBlock.h"
#include "../Components/Other/Flag.h"
#include "../Components/Blocks/HiddenBlocks.h"
#include "../Components/Enemies/KoopaTroopa.h"

#include <format>
#include <fstream>

#include "../Components/Other/CastleFlag.h"
#include "../Components/Player/PlayerCharacter.h"
#include "Engine/Interfaces/ISoundSystem.h"

namespace thomasWasLate
{
    class GoombaAI;
    class PointsBehaviour;
}

void thomasWasLate::GameManager::LoadLevel()
{
    m_CurrentPlayerState = m_LastPlayerState == PlayerHealthState::Small ? PlayerHealthState::Small : PlayerHealthState::Big;
    ReadLevelInfo(LoadInformation());

    CreateWorldCollision();

    OnNewLevelLoadedEvent.Broadcast();
}

void thomasWasLate::GameManager::SwitchToNextScene()
{
    ClearListeners();
    
    if (m_ShouldPlayTransition)
        diji::SceneManager::GetInstance().SetNextSceneToActivate(static_cast<int>(thomasWasLateState::TransitionToNextLevel));
    else
        diji::SceneManager::GetInstance().SetNextSceneToActivate(static_cast<int>(thomasWasLateState::Level));
}

void thomasWasLate::GameManager::SetLevelCleared()
{
    ++m_PlayerInfo.currentLevel;
    m_ShouldPlayTransition = true;

    OnLevelClearedEvent.Broadcast();
    ResetLevel();
}

void thomasWasLate::GameManager::ResetLevel()
{
    m_CurrentPlayerState = PlayerHealthState::Small;
    m_LastPlayerState = PlayerHealthState::Small;
    
    ClearListeners();

    if (m_PlayerInfo.totalLives == 0)
        diji::SceneManager::GetInstance().SetNextSceneToActivate(static_cast<int>(thomasWasLateState::GameOver));
    else
        diji::SceneManager::GetInstance().SetNextSceneToActivate(static_cast<int>(thomasWasLateState::LivesDisplayMenu));
    m_TotalFireballsInLevel = 0;
}

void thomasWasLate::GameManager::SwitchCurrentPlayerState()
{
    if (m_CurrentPlayerState == PlayerHealthState::Small)
        m_CurrentPlayerState = PlayerHealthState::Big;
    else
        m_CurrentPlayerState = PlayerHealthState::Small;
}

void thomasWasLate::GameManager::SavePlayerState()
{
    switch (diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar")->GetComponent<PlayerCharacter>()->GetPowerUpState())
    {
    case 0:
        m_LastPlayerState = PlayerHealthState::Small;
        break;
    case 1:
        m_LastPlayerState = PlayerHealthState::Big;
        break;
    case 2:
        m_LastPlayerState = PlayerHealthState::Fire;
        break;
    default:
        m_LastPlayerState = PlayerHealthState::Small;
        break;
    }
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

void thomasWasLate::GameManager::AddLife()
{
    ++m_PlayerInfo.totalLives;
    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_1-up.wav", false);
}

void thomasWasLate::GameManager::ResetPlayerInfo()
{
    m_PlayerInfo.totalLives = 3;
    m_PlayerInfo.totalCoins = 0;
    m_PlayerInfo.totalScore = 0;
    m_PlayerInfo.currentLevel = 1;
}

std::string thomasWasLate::GameManager::LoadInformation()
{
    if (m_ShouldPlayTransition)
    {
        m_ShouldPlayTransition = false;
        m_StartPosition.x = 100;
        m_StartPosition.y = 100;
        
        return "../ThomasWasLate/Resources/levels/transitionLevel.txt";
    }
    
    switch (m_PlayerInfo.currentLevel) // if you're going to read from a file put this information in the fucking file
    {
    case 1:
        m_StartPosition.x = 100;
        m_StartPosition.y = 400;
        break;
    case 2:
        m_StartPosition.x = 100;
        m_StartPosition.y = 0;
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

    return std::format("../ThomasWasLate/Resources/levels/level{}.txt", m_PlayerInfo.currentLevel);
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

// todo: rename, it also creates enemies
void thomasWasLate::GameManager::CreateWorldCollision()
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

            if (std::string("0edxyfghijklmnoABCDEF").find(tile) == std::string::npos)
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
                sf::Vector2f center{ left + 25.f, bottom + 25.f };

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

                AddEnemyCollider(collider);

                if (tile == 'h')
                {
                    center.x += 25.f;
                    goomba->GetComponent<GoombaAI>()->SetActivationMilestone(col - 21);
                }

                (void)diji::SceneManager::GetInstance().SpawnGameObject("E_Goomba", std::move(goomba), center);

                ++col;
            }
            else if (tile == 'k')
            {
                const float left = static_cast<float>(col) * kTileSize;
                const float bottom = static_cast<float>(row) * kTileSize;
                sf::Vector2f center{ left + 25.f, bottom + 25.f };
            
                auto pole = std::make_unique<diji::GameObject>();
                pole->AddComponents<diji::Transform>(600, 300);
                pole->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 24,  16 * 50 });
                const auto collider = pole->GetComponent<diji::Collider>();
                collider->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);
                collider->SetTag("flagPole");
                collider->SetAffectedByGravity(false);
                collider->SetStatic(true);
            
                (void)diji::SceneManager::GetInstance().SpawnGameObject("E_endPole", std::move(pole), center);

                // create the flag
                auto flag = std::make_unique<diji::GameObject>();
                flag->AddComponents<diji::Transform>(600, 300);
                flag->AddComponents<diji::TextureComp>("graphics/flag.png");
                flag->AddComponents<diji::Render>();
                flag->AddComponents<Flag>();

                (void)diji::SceneManager::GetInstance().SpawnGameObject("E_flag", std::move(flag), center - sf::Vector2f{ 25, 100 });

                // create the poleEnd
                auto poleTop = std::make_unique<diji::GameObject>();
                poleTop->AddComponents<diji::Transform>(600, 300);
                poleTop->AddComponents<diji::TextureComp>("graphics/poleTop.png");
                poleTop->AddComponents<diji::Render>();

                (void)diji::SceneManager::GetInstance().SpawnGameObject("E_poleTop", std::move(poleTop), center - sf::Vector2f{ 0, 75.f });
                
                ++col;
            }
            else if (tile == 'l' || tile == 'm')
            {
                const float left = static_cast<float>(col) * kTileSize;
                const float bottom = static_cast<float>(row) * kTileSize;
                sf::Vector2f center{ left + 25.f, bottom + 25.f - 100.f };
            
                auto castle = std::make_unique<diji::GameObject>();
                castle->AddComponents<diji::Transform>(600, 300);
                castle->AddComponents<diji::TextureComp>("graphics/smallCastle.png");
                castle->AddComponents<diji::Render>();
            
                (void)diji::SceneManager::GetInstance().SpawnGameObject("E_castle", std::move(castle), center);

                auto castleFlag = std::make_unique<diji::GameObject>();
                castleFlag->AddComponents<diji::Transform>(11000, 250);
                castleFlag->AddComponents<diji::TextureComp>("graphics/castleFlag.png");
                castleFlag->AddComponents<diji::Render>();
                castleFlag->AddComponents<CastleFlag>();

                (void)diji::SceneManager::GetInstance().SpawnGameObject("C_castleFlag", std::move(castleFlag), center + sf::Vector2f{ 0.f, -75.f });
                
                ++col;
            }
            else if (tile == 'n')
            {
                const float left = static_cast<float>(col) * kTileSize;
                const float bottom = static_cast<float>(row) * kTileSize;
                sf::Vector2f center{ left + 25.f, bottom + 25.f };
            
                auto oneUpBlock = std::make_unique<diji::GameObject>();
                oneUpBlock->AddComponents<diji::Transform>(600, 300);
                oneUpBlock->AddComponents<diji::SpriteRenderComponent>("graphics/breakableBlock.png", sf::Vector2i{ 50,50 }, 1, 0.035f);
                oneUpBlock->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
                oneUpBlock->GetComponent<diji::Collider>()->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);
                oneUpBlock->GetComponent<diji::Collider>()->SetTag("HiddenBlock");
                oneUpBlock->GetComponent<diji::Collider>()->SetAffectedByGravity(false);
                oneUpBlock->GetComponent<diji::Collider>()->SetIsMoveable(false);
                oneUpBlock->AddComponents<HiddenBlocks>();
            
                (void)diji::SceneManager::GetInstance().SpawnGameObject("E_oneUpBlock", std::move(oneUpBlock), center);
                
                ++col;
            }
            else if (tile == 'o')
            {
                const float left = static_cast<float>(col) * kTileSize;
                const float bottom = static_cast<float>(row) * kTileSize;
                sf::Vector2f center{ left + 25.f, bottom + 25.f };

                auto koopa = std::make_unique<diji::GameObject>();
                koopa->AddComponents<diji::Transform>(2200, 200);
                koopa->AddComponents<diji::SpriteRenderComponent>("graphics/koopaTroopa.png", sf::Vector2i{ 50,75 }, 2, 0.15f);
                koopa->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 75 });
                const auto koopaCollider = koopa->GetComponent<diji::Collider>();
                koopaCollider->SetRestitution(0.f);
                koopaCollider->SetMass(0.89f);
                koopaCollider->SetStaticFriction(0.25f);
                koopaCollider->SetKineticFriction(0.15f);
                koopaCollider->SetMaxVelocity(sf::Vector2f{ 400.f, 800.f });
                koopaCollider->SetGenerateHitEvents(true);
                koopaCollider->SetTag("koopa");
                koopa->AddComponents<KoopaTroopa>();
                koopa->GetComponent<KoopaTroopa>()->SetActivationMilestone(col - 20);
                koopa->SetActive(false);
                
                AddEnemyCollider(koopaCollider);
        
                (void)diji::SceneManager::GetInstance().SpawnGameObject("E_Koopa", std::move(koopa), center);
                
                ++col;
            }
            else if (tile == 'A' || tile == 'B' || tile == 'C' || tile == 'D' || tile == 'E' || tile == 'F')
            {
                const float left = static_cast<float>(col) * kTileSize;
                const float bottom = static_cast<float>(row) * kTileSize;
                sf::Vector2f center{ left + 25.f, bottom + 25.f };
                
                int x = 0, y = 0;
                switch (tile)
                {
                case 'D': x = 0; y = 2; break;
                case 'E': x = 1; y = 2; break;
                case 'F': x = 2; y = 2; break;
                case 'A': x = 0; y = 3; break;
                case 'B': x = 1; y = 3; break;
                case 'C': x = 2; y = 3; break;
                default: break;
                }

                auto foregroundTexture = std::make_unique<diji::GameObject>();
                foregroundTexture->AddComponents<diji::Transform>(600, 300);
                foregroundTexture->AddComponents<diji::SpriteRenderComponent>("graphics/tiles_sheet.png", sf::Vector2i{50, 50}, 1, 0.05f);
                foregroundTexture->GetComponent<diji::SpriteRenderComponent>()->SetFrameSize(sf::Vector2i{50, 50});
                foregroundTexture->GetComponent<diji::SpriteRenderComponent>()->SetStartingFrame(x, y);
                foregroundTexture->GetComponent<diji::SpriteRenderComponent>()->SetTotalAnimationFrames(1);
                foregroundTexture->GetComponent<diji::SpriteRenderComponent>()->SetFrameDuration(0.01f);
                foregroundTexture->GetComponent<diji::SpriteRenderComponent>()->SetLooping(false);
                foregroundTexture->GetComponent<diji::SpriteRenderComponent>()->Pause();
                foregroundTexture->GetComponent<diji::SpriteRenderComponent>()->SetCurrentAnimationFrame(0);
                foregroundTexture->GetComponent<diji::SpriteRenderComponent>()->UpdateFrame();
                foregroundTexture->GetComponent<diji::SpriteRenderComponent>()->SkipStart();

                (void)diji::SceneManager::GetInstance().SpawnGameObject("ZZ_foregroundTexture", std::move(foregroundTexture), center);
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

void thomasWasLate::GameManager::ClearListeners()
{
    OnNewLevelLoadedEvent.ClearAllListeners();
    OnPlayerSwitchedEvent.ClearAllListeners();
    OnScoreAddedEvent.ClearAllListeners();
    OnCoinCollectedEvent.ClearAllListeners();
    OnLevelClearedEvent.ClearAllListeners();
}
