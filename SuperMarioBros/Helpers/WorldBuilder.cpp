#include "WorldBuilder.h"
#include "Engine/Collision/CollisionShape.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Components/TextComp.h"
#include "Engine/Components/TextureComp.h"
#include "Engine/Components/Render.h"
#include "Engine/Interfaces/ISoundSystem.h"
#include "Engine/Components/ShapeRender.h"
#include "../Components/Blocks/SharedBehaviour/PowerUpBlock.h"
#include "../Components/Blocks/UniqueBehaviour/BreakableBlock.h"
#include "../Components/Blocks/UniqueBehaviour/MultiCoinBlock.h"
#include "../Components/Enemies/GoombaAI.h"
#include "../Components/Blocks/SharedBehaviour/StarBlock.h"
#include "../Components/Other/LevelObjects/Flag.h"
#include "../Components/Blocks/HiddenBlock/OneUpBlock.h"
#include "../Components/Enemies/KoopaTroopa.h"
#include "../Components/Enemies/PiranhaPlant.h"
#include "../Components/Other/LevelObjects/CastleFlag.h"
#include "../Components/Other/LevelObjects/CheckPoint.h"
#include "../Components/Other/Coins/StaticCoin.h"
#include "../Components/Blocks/BaseBlock.h"
#include "../Singletons/GameManager.h"

#include <functional>
#include <string>
#include <memory>

void superMarioBros::WorldBuilder::CreateWorld(const std::vector<char>& levelInfo, const int& rows, const int& cols)
{
    constexpr float kTileSize = 50.0f;
    const std::string specialTiles = "0edxyfghijklmnopqrstuvABCDEF"; //todo: consider inverting this system

    using Handler = std::function<int(int row, int col, char tile)>;
    std::unordered_map<char, Handler> handlers;

    // 1) Create Ground
    handlers['#'] = [&](const int row, const int col, char) -> int
    {
        const int startC = col;
        int c = col;
        while (c < cols && levelInfo[row * cols + c] != '0')
            ++c;
        const int len = c - startC;
        if (len <= 0) return 1;

        const float left = static_cast<float>(startC) * kTileSize;
        const float bottom = static_cast<float>(row) * kTileSize;
        const float width  = static_cast<float>(len) * kTileSize;
        constexpr float height = kTileSize;
        const sf::Vector2f center = { left + width * 0.5f, bottom + height * 0.5f };

        auto go = std::make_unique<diji::GameObject>();
        go->SetObjectPosition(center);
        go->AddComponent<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ static_cast<float>(len) * kTileSize, kTileSize });

        const auto collider = go->GetComponent<diji::Collider>();
        collider->SetStatic(true);
        collider->SetTag("ground");
        go->AddComponent<diji::ShapeRender>();

        diji::SceneManager::GetInstance().SpawnGameObject("WorldCollider", std::move(go), center);
        return len;
    };

    // 2) Lucky block: e,x,y
    auto spawnLuckyBlock = [&](const int row, const int col, const char tile) -> int
    {
        const float left = static_cast<float>(col) * kTileSize;
        const float bottom = static_cast<float>(row) * kTileSize;
        const sf::Vector2f center = { left + 25.f, bottom + 25.f };

        auto go = std::make_unique<diji::GameObject>();
        go->SetObjectPosition(center);
        go->AddComponent<diji::SpriteRenderComponent>("graphics/luckyBlock.png", sf::Vector2i{50,50}, 6, 0.135f);
        go->GetComponent<diji::SpriteRenderComponent>()->SetStartingFrameX(1);
        go->AddComponent<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{50,50});

        const auto collider = go->GetComponent<diji::Collider>();
        collider->SetTag("luckyBlock");
        collider->SetAffectedByGravity(false);
        collider->SetIsMoveable(false);

        if (tile == 'e') go->AddComponent<BaseBlock>(BaseBlock::ItemSpawnType::Coin);
        else if (tile == 'y') go->AddComponent<PowerUpBlock>(BaseBlock::ItemSpawnType::StarPowerUp);
        else if (tile == 'x') go->AddComponent<PowerUpBlock>(BaseBlock::ItemSpawnType::PowerUp);

        diji::SceneManager::GetInstance().SpawnGameObject("E_luckyBlock", std::move(go), center);
        return 1;
    };
    
    handlers['e'] = spawnLuckyBlock;
    handlers['x'] = spawnLuckyBlock;
    handlers['y'] = spawnLuckyBlock;

    // 3) Breakable / special blocks: d,f,i,n,t,u,v
    handlers['d'] = handlers['f'] = handlers['i'] = handlers['n'] =
    handlers['t'] = handlers['u'] = handlers['v'] =
    [&](const int row, const int col, const char tile) -> int
    {
        const float left = static_cast<float>(col) * kTileSize;
        const float bottom = static_cast<float>(row) * kTileSize;
        const sf::Vector2f center = { left + 25.f, bottom + 25.f };

        auto go = std::make_unique<diji::GameObject>();
        go->SetObjectPosition(center);
        go->AddComponent<diji::SpriteRenderComponent>("graphics/breakableBlock.png", sf::Vector2i{50,50}, 1, 0.0f);
        go->AddComponent<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{50,50});

        const auto spr = go->GetComponent<diji::SpriteRenderComponent>();
        spr->SetStartingFrameX(1);
        spr->SetLooping(false);
        spr->SkipStart();
        spr->UpdateFrame();

        const auto collider = go->GetComponent<diji::Collider>();
        collider->SetTag("breakBlock");
        collider->SetAffectedByGravity(false);
        collider->SetIsMoveable(false);

        if (tile == 'd')
            go->AddComponent<BreakableBlock>(BaseBlock::ItemSpawnType::None);
        else if (tile == 'i')
            go->AddComponent<StarBlock>(BaseBlock::ItemSpawnType::StarPowerUp);
        else if (tile == 'f')
            go->AddComponent<MultiCoinBlock>(BaseBlock::ItemSpawnType::Coin);
        else if (tile == 'n')
        {
            collider->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);
            collider->SetTag("HiddenBlock");
            go->AddComponent<OneUpBlock>(BaseBlock::ItemSpawnType::OneUpMushroom);
        }
        else if (tile == 't')
        {
            go->AddComponent<HiddenBlock>(BaseBlock::ItemSpawnType::None);
            collider->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);
            collider->SetTag("HiddenBlock");
        }
        else if (tile == 'u')
            go->AddComponent<BreakableBlock>(BaseBlock::ItemSpawnType::StarPowerUp);
        else if (tile == 'v')
            go->AddComponent<PowerUpBlock>(BaseBlock::ItemSpawnType::PowerUp);

        diji::SceneManager::GetInstance().SpawnGameObject("E_breakableBlock", std::move(go), center);
        return 1;
    };

    // 4) Goomba: g/h
    auto spawnGoomba = [&](const int row, const int col, const char tile) -> int
    {
        const float left = static_cast<float>(col) * kTileSize;
        const float bottom = static_cast<float>(row) * kTileSize;
        sf::Vector2f center = { left + 25.f, bottom + 25.f };

        auto go = std::make_unique<diji::GameObject>();
        go->SetObjectPosition(center);
        go->AddComponent<diji::SpriteRenderComponent>("graphics/goomba.png", sf::Vector2i{50,50}, 2, 0.15f);
        go->AddComponent<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{50,50});
        go->AddComponent<GoombaAI>();
        go->GetComponent<GoombaAI>()->SetActivationMilestone(col - 20);
        go->SetActive(false);
        
        const auto collider = go->GetComponent<diji::Collider>();
        collider->SetRestitution(0.f);
        collider->SetMass(0.89f);
        collider->SetStaticFriction(0.25f);
        collider->SetKineticFriction(0.15f);
        collider->SetMaxVelocity(sf::Vector2f{400.f, 10000.f});
        collider->SetGenerateHitEvents(true);
        collider->SetTag("enemy");

        GameManager::GetInstance().AddEnemyCollider(collider);

        if (tile == 'h')
        {
            center.x += 25.f;
            go->GetComponent<GoombaAI>()->SetActivationMilestone(col - 21);
        }

        diji::SceneManager::GetInstance().SpawnGameObject("E_Goomba", std::move(go), center);
        return 1;
    };
    
    handlers['g'] = spawnGoomba;
    handlers['h'] = spawnGoomba;

    // 5) Flag pole
    handlers['k'] = [&](const int row, const int col, char) -> int
    {
        const float left = static_cast<float>(col) * kTileSize;
        const float bottom = static_cast<float>(row) * kTileSize;
        const sf::Vector2f center = { left + 25.f, bottom + 25.f };

        // pole collider
        auto pole = std::make_unique<diji::GameObject>();
        pole->SetObjectPosition(center);
        pole->AddComponent<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{24, 16 * 50});

        const auto poleCollider = pole->GetComponent<diji::Collider>();
        poleCollider->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);
        poleCollider->SetTag("flagPole");
        poleCollider->SetAffectedByGravity(false);
        poleCollider->SetStatic(true);
        diji::SceneManager::GetInstance().SpawnGameObject("E_endPole", std::move(pole), center);

        // flag
        auto flag = std::make_unique<diji::GameObject>();
        flag->SetObjectPosition(center);
        flag->AddComponent<diji::TextureComp>("graphics/flag.png");
        flag->AddComponent<diji::Render>();
        flag->AddComponent<Flag>();
        diji::SceneManager::GetInstance().SpawnGameObject("E_flag", std::move(flag), center - sf::Vector2f{25, 100});

        // pole top
        auto poleTop = std::make_unique<diji::GameObject>();
        poleTop->SetObjectPosition(center);
        poleTop->AddComponent<diji::TextureComp>("graphics/poleTop.png");
        poleTop->AddComponent<diji::Render>();
        diji::SceneManager::GetInstance().SpawnGameObject("E_poleTop", std::move(poleTop), center - sf::Vector2f{0, 75.f});

        return 1;
    };

    // 6) Castle l/m
    handlers['l'] = handlers['m'] = [&](const int row, const int col, char ) -> int
    {
        const float left = static_cast<float>(col) * kTileSize;
        const float bottom = static_cast<float>(row) * kTileSize;
        const sf::Vector2f center = { left + 25.f, bottom + 25.f - 100.f };

        auto castle = std::make_unique<diji::GameObject>();
        castle->SetObjectPosition(center);
        castle->AddComponent<diji::TextureComp>("graphics/smallCastle.png");
        castle->AddComponent<diji::Render>();
        diji::SceneManager::GetInstance().SpawnGameObject("D_castle", std::move(castle), center);

        auto castleFlag = std::make_unique<diji::GameObject>();
        castleFlag->SetObjectPosition(center + sf::Vector2f{0.f, -75.f});
        castleFlag->AddComponent<diji::TextureComp>("graphics/castleFlag.png");
        castleFlag->AddComponent<diji::Render>();
        castleFlag->AddComponent<CastleFlag>();
        diji::SceneManager::GetInstance().SpawnGameObject("C_castleFlag", std::move(castleFlag), center + sf::Vector2f{0.f, -75.f});

        return 1;
    };

    // 7) Koopa o/p
    handlers['o'] = handlers['p'] = [&](const int row, const int col, const char tile) -> int
    {
        const float left = static_cast<float>(col) * kTileSize;
        const float bottom = static_cast<float>(row) * kTileSize;
        sf::Vector2f center = { left + 25.f, bottom + 25.f };

        auto go = std::make_unique<diji::GameObject>();
        go->SetObjectPosition(center);
        go->AddComponent<diji::SpriteRenderComponent>("graphics/koopaTroopa.png", sf::Vector2i{50,75}, 2, 0.15f);
        go->AddComponent<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{50,75});
        go->AddComponent<KoopaTroopa>();
        go->GetComponent<KoopaTroopa>()->SetActivationMilestone(col - 20);
        go->SetActive(false);
        
        const auto colComp = go->GetComponent<diji::Collider>();
        colComp->SetRestitution(0.f);
        colComp->SetMass(0.89f);
        colComp->SetStaticFriction(0.25f);
        colComp->SetKineticFriction(0.15f);
        colComp->SetMaxVelocity(sf::Vector2f{400.f, 800.f});
        colComp->SetGenerateHitEvents(true);
        colComp->SetTag("koopa");

        if (tile == 'p')
        {
            center.x += 25.f;
            go->GetComponent<KoopaTroopa>()->SetActivationMilestone(col - 21);
        }

        GameManager::GetInstance().AddEnemyCollider(colComp);
        diji::SceneManager::GetInstance().SpawnGameObject("E_Koopa", std::move(go), center);
        return 1;
    };

    // 8) Foreground tiles
    handlers['A'] = handlers['B'] = handlers['C'] =
    handlers['D'] = handlers['E'] = handlers['F'] =
    [&](const int row, const int col, const char tile) -> int
    {
        const float left = static_cast<float>(col) * kTileSize;
        const float bottom = static_cast<float>(row) * kTileSize;
        const sf::Vector2f center = { left + 25.f, bottom + 25.f };

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

        auto go = std::make_unique<diji::GameObject>();
        go->SetObjectPosition(center);
        go->AddComponent<diji::SpriteRenderComponent>("graphics/tiles_sheet.png", sf::Vector2i{50,50}, 1, 0.05f);

        const auto spr = go->GetComponent<diji::SpriteRenderComponent>();
        spr->SetFrameSize(sf::Vector2i{50,50});
        spr->SetStartingFrame(x, y);
        spr->SetTotalAnimationFrames(1);
        spr->SetFrameDuration(0.01f);
        spr->SetLooping(false);
        spr->Pause();
        spr->SetCurrentAnimationFrame(0);
        spr->UpdateFrame();
        spr->SkipStart();

        diji::SceneManager::GetInstance().SpawnGameObject("ZZ_foregroundTexture", std::move(go), center);
        return 1;
    };

    // 9) Static coin q
    handlers['q'] = [&](const int row, const int col, char) -> int
    {
        const float left = static_cast<float>(col) * kTileSize;
        const float bottom = static_cast<float>(row) * kTileSize;
        const sf::Vector2f center = { left + 25.f, bottom + 25.f };

        auto go = std::make_unique<diji::GameObject>();
        go->SetObjectPosition(center);
        go->AddComponent<diji::SpriteRenderComponent>("graphics/staticCoin.png", sf::Vector2i{50,50}, 6, 0.135f);
        go->AddComponent<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{50,50});
        go->AddComponent<StaticCoin>();

        const auto collider = go->GetComponent<diji::Collider>();
        collider->SetTag("coin");
        collider->SetAffectedByGravity(false);
        collider->SetIsMoveable(false);
        collider->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);

        diji::SceneManager::GetInstance().SpawnGameObject("K_StaticCoin", std::move(go), center);
        return 1;
    };

    // 10) Checkpoint r
    handlers['r'] = [&](const int row, const int col, char) -> int
    {
        const float left = static_cast<float>(col) * kTileSize;
        const float bottom = static_cast<float>(row) * kTileSize;
        const sf::Vector2f center = { left + 25.f, bottom + 25.f };

        auto go = std::make_unique<diji::GameObject>();
        go->SetObjectPosition(center);
        go->AddComponent<CheckPoint>();
        go->GetComponent<CheckPoint>()->SetActivationMilestone(col);

        diji::SceneManager::GetInstance().SpawnGameObject("E_checkPoint", std::move(go), center);
        return 1;
    };

    // 11) Piranha plant s
    handlers['s'] = [&](const int row, const int col, char) -> int
    {
        const float left = static_cast<float>(col) * kTileSize;
        const float bottom = static_cast<float>(row) * kTileSize;
        const sf::Vector2f center = { left + 50.f, bottom + 45.f };

        auto plant = std::make_unique<diji::GameObject>();
        plant->SetObjectPosition(center);
        plant->AddComponent<diji::SpriteRenderComponent>("graphics/piranhaPlant.png", sf::Vector2i{50,75}, 2, 0.135f);
        plant->AddComponent<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{50,75});
        plant->AddComponent<PiranhaPlant>();
        plant->GetComponent<PiranhaPlant>()->SetActivationMilestone(col - 20);
        plant->SetActive(false);
        
        const auto collider = plant->GetComponent<diji::Collider>();
        collider->SetIsMoveable(false);
        collider->SetTag("plant");
        collider->SetAffectedByGravity(false);
        collider->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);

        diji::SceneManager::GetInstance().SpawnGameObject("BA_PiranhaPlant", std::move(plant), center);

        // spawn a ground collider under the plant (mirrors original)
        auto ground = std::make_unique<diji::GameObject>();
        ground->SetObjectPosition(center);
        ground->AddComponent<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{kTileSize, kTileSize});

        const auto groundCollider = ground->GetComponent<diji::Collider>();
        groundCollider->SetStatic(true);
        groundCollider->SetTag("ground");
        diji::SceneManager::GetInstance().SpawnGameObject("WorldCollider", std::move(ground), center - sf::Vector2f{ 25.f, 20.f });

        return 1;
    };

    for (int row = 0; row < rows; ++row)
    {
        int col = 0;
        while (col < cols)
        {
            const int idx = row * cols + col;
            const char tile = levelInfo[idx];

            if (specialTiles.find(tile) == std::string::npos)
            {
                col += handlers['#'](row, col, tile);
                continue;
            }

            auto it = handlers.find(tile);
            if (it != handlers.end())
                col += it->second(row, col, tile);
            else
                ++col;
        }
    }
}