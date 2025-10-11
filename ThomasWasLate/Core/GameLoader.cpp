#include "GameLoader.h"

#include "GameState.h"
#include "../Components/BackgroundHandler.h"
#include "../Components/CustomBackgroundRenderer.h"
#include "../Components/GoombaAI.h"
#include "../Components/PlayerCharacter.h"
#include "../Components/TimerScript.h"
#include "../Input/CustomCommands.h"
#include "../Singletons/GameManager.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/TextureComp.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Interfaces/ISoundSystem.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/Render.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/FPSCounter.h"
#include "Engine/Components/ScoreCounter.h"
#include "Engine/Components/ShapeRender.h"
#include "Engine/Components/Sprite.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Components/TextComp.h"
#include "Engine/Core/Engine.h"
#include "Engine/Singleton/GameStateManager.h"

using namespace diji;

void SceneLoader::GameStartUp()
{
#ifndef NDEBUG
    ServiceLocator::RegisterSoundSystem(std::make_unique<LoggingSoundSystem>(std::make_unique<SFMLISoundSystem>()));
#else
    ServiceLocator::RegisterSoundSystem(std::make_unique<SFMLISoundSystem>());
#endif

    //todo: this would be better if it was part of the CreateScene function.
    SceneManager::GetInstance().RegisterScene(static_cast<int>(thomasWasLate::thomasWasLateState::Level), Level);
    Level();
}

void SceneLoader::StartMenu(){}

void SceneLoader::Level()
{
    SceneManager::GetInstance().SetActiveScene(static_cast<int>(thomasWasLate::thomasWasLateState::Level));
    const auto& scene = SceneManager::GetInstance().CreateScene(static_cast<int>(thomasWasLate::thomasWasLateState::Level));
    GameStateManager::GetInstance().SetNewGameState(static_cast<GameState>(thomasWasLate::thomasWasLateState::Level));

    // static_cast<float>(window::VIEWPORT.x) * -0.5f + 25.f, static_cast<float>(window::VIEWPORT.y) * -0.5f + 25.f
    const auto staticBackground = scene->CreateGameObject("A_StaticBackground");
    // staticBackground->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.5f, static_cast<float>(window::VIEWPORT.y) * 0.5f);
    staticBackground->AddComponents<Transform>(0, 0);
    staticBackground->AddComponents<TextureComp>("graphics/background.png");
    staticBackground->AddComponents<thomasWasLate::CustomBackgroundRenderer>();
    // scene->SetGameObjectAsStaticBackground(staticBackground);

    const auto background = scene->CreateGameObject("B_Background");
    background->AddComponents<Transform>(0, 0);
    background->AddComponents<Sprite>("graphics/tiles_sheet.png");
    background->AddComponents<Render>();
    background->AddComponents<thomasWasLate::BackgroundHandler>();

    const sf::FloatRect arena{ 0,-(115 * 4.5),12000.f, 1080.f };
    const auto camera = scene->CreateCameraObject("A_Camera");
    camera->AddComponents<Transform>(0, 0);
    camera->AddComponents<Camera>(window::VIEWPORT);
    // camera->AddComponents<Camera>(window::VIEWPORT);
    camera->GetComponent<Camera>()->SetLevelBoundaries(arena);
    // camera->GetComponent<Camera>()->SetClampingMode(false);

    const auto player = scene->CreateGameObject("X_PlayerChar");
    player->AddComponents<Transform>(200, 0);
    // player->AddComponents<TextureComp>("graphics/player.png");
    // player->AddComponents<Render>(3);
    player->AddComponents<SpriteRenderComponent>("graphics/player.png", sf::Vector2i{ 16, 16 }, 3, 0.05f);
    player->GetComponent<SpriteRenderComponent>()->SetScale(3);
    // player->GetComponent<SpriteRenderComponent>()->SetFrameSize(sf::Vector2i{ 16, 16 });
    player->AddComponents<Collider>(CollisionShape::ShapeType::RECT, sf::Vector2f{ 48, 48 });
    player->GetComponent<Collider>()->SetRestitution(0.f);
    player->GetComponent<Collider>()->SetMass(0.89f);
    player->GetComponent<Collider>()->SetFriction(0.25f);
    player->GetComponent<Collider>()->SetGenerateHitEvents(true);
    player->GetComponent<Collider>()->SetMaxVelocity(sf::Vector2f{ 400.f, 800.f });
    player->GetComponent<Collider>()->SetTag("player");
    // player->GetComponent<Collider>()->SetAffectedByGravity(false);
    player->AddComponents<thomasWasLate::PlayerCharacter>(0.45f);
    // player->AddComponents<ShapeRender>(true);

    SceneManager::GetInstance().GetPhysicsWorld()->SetGravity(sf::Vector2f{ 0, 980 * 3.f });

    const auto goombaTest = scene->CreateGameObject("Y_GoombaTest");
    goombaTest->AddComponents<Transform>(2000, 0);
    goombaTest->AddComponents<SpriteRenderComponent>("graphics/goomba.png", sf::Vector2i{ 50,50 }, 2, 0.15f);
    goombaTest->AddComponents<Collider>(CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
    goombaTest->GetComponent<Collider>()->SetRestitution(0.f);
    goombaTest->GetComponent<Collider>()->SetMass(0.89f);
    goombaTest->GetComponent<Collider>()->SetFriction(0.25f);
    goombaTest->GetComponent<Collider>()->SetMaxVelocity(sf::Vector2f{ 400.f, 800.f });
    goombaTest->GetComponent<Collider>()->SetGenerateHitEvents(true);
    goombaTest->GetComponent<Collider>()->SetTag("enemy");
    goombaTest->AddComponents<thomasWasLate::GoombaAI>();

    const auto goombaTest2 = scene->CreateGameObject("Y_Goomba2Test");
    goombaTest2->AddComponents<Transform>(1800, 0);
    goombaTest2->AddComponents<SpriteRenderComponent>("graphics/goomba.png", sf::Vector2i{ 50,50 }, 2, 0.15f);
    goombaTest2->AddComponents<Collider>(CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
    goombaTest2->GetComponent<Collider>()->SetRestitution(0.f);
    goombaTest2->GetComponent<Collider>()->SetMass(0.89f);
    goombaTest2->GetComponent<Collider>()->SetFriction(0.25f);
    goombaTest2->GetComponent<Collider>()->SetMaxVelocity(sf::Vector2f{ 400.f, 800.f });
    goombaTest2->GetComponent<Collider>()->SetGenerateHitEvents(true);
    goombaTest2->GetComponent<Collider>()->SetTag("enemy");
    goombaTest2->AddComponents<thomasWasLate::GoombaAI>();

    const auto goombaTest3 = scene->CreateGameObject("Y_Goomba32Test");
    goombaTest3->AddComponents<Transform>(2300, 0);
    goombaTest3->AddComponents<SpriteRenderComponent>("graphics/goomba.png", sf::Vector2i{ 50,50 }, 2, 0.15f);
    goombaTest3->AddComponents<Collider>(CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
    goombaTest3->GetComponent<Collider>()->SetRestitution(0.f);
    goombaTest3->GetComponent<Collider>()->SetMass(0.89f);
    goombaTest3->GetComponent<Collider>()->SetFriction(0.25f);
    goombaTest3->GetComponent<Collider>()->SetMaxVelocity(sf::Vector2f{ 400.f, 800.f });
    goombaTest3->GetComponent<Collider>()->SetGenerateHitEvents(true);
    goombaTest3->GetComponent<Collider>()->SetTag("enemy");
    goombaTest3->AddComponents<thomasWasLate::GoombaAI>();

    // Create the HUD
    const auto marioName = scene->CreateGameObject("Z_MarioName");
    marioName->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.15f, static_cast<float>(window::VIEWPORT.y) * 0.05f);
    marioName->AddComponents<TextComp>("MARIO", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    marioName->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    marioName->GetComponent<TextComp>()->SetCentered(true);
    marioName->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(marioName);
    
    const auto scoreHUD = scene->CreateGameObject("Z_ScoreHUD");
    scoreHUD->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.15f + 13, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f);
    scoreHUD->AddComponents<TextComp>("000000", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    scoreHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    scoreHUD->GetComponent<TextComp>()->SetCentered(true);
    scoreHUD->AddComponents<ScoreCounter>(0, true);
    scoreHUD->GetComponent<ScoreCounter>()->SetString("");
    scoreHUD->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true);
    scoreHUD->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(scoreHUD);

    const auto coinsCounterHud = scene->CreateGameObject("Z_CoinsCounterHUD");
    coinsCounterHud->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.4f, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f);
    coinsCounterHud->AddComponents<TextComp>("00", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    coinsCounterHud->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    coinsCounterHud->GetComponent<TextComp>()->SetCentered(true);
    coinsCounterHud->AddComponents<ScoreCounter>(0, true);
    coinsCounterHud->GetComponent<ScoreCounter>()->SetString("");
    coinsCounterHud->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true, 2);
    coinsCounterHud->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(coinsCounterHud);
    
    const auto xMarkHUD = scene->CreateGameObject("Z_xMarkHUD");
    xMarkHUD->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.4f - 38, static_cast<float>(window::VIEWPORT.y) * 0.05f + 32.f);
    xMarkHUD->AddComponents<TextComp>("X", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    xMarkHUD->GetComponent<TextComp>()->GetText().setCharacterSize(18);
    xMarkHUD->GetComponent<TextComp>()->SetCentered(true);
    xMarkHUD->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(xMarkHUD);
    
    const auto coinImageHud = scene->CreateGameObject("Z_CoinsImageHUD");
    coinImageHud->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.4f - 60, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f);
    coinImageHud->AddComponents<SpriteRenderComponent>("graphics/HUDCoins.png", sf::Vector2i{ 24, 24 }, 6, 0.15f);
    scene->SetGameObjectAsCanvasObject(coinImageHud);

    const auto worldNameHUD = scene->CreateGameObject("Z_worldNameHUD");
    worldNameHUD->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.6f + 40, static_cast<float>(window::VIEWPORT.y) * 0.05f);
    worldNameHUD->AddComponents<TextComp>("WORLD", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    worldNameHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    worldNameHUD->GetComponent<TextComp>()->SetCentered(true);
    worldNameHUD->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(worldNameHUD);

    const auto worldCountHUD = scene->CreateGameObject("Z_worldCountHUD");
    worldCountHUD->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.6f + 40, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f);
    worldCountHUD->AddComponents<TextComp>("1-1", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    worldCountHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    worldCountHUD->GetComponent<TextComp>()->SetCentered(true);
    worldCountHUD->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(worldCountHUD);

    const auto timerName = scene->CreateGameObject("Z_timerName");
    timerName->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.85f, static_cast<float>(window::VIEWPORT.y) * 0.05f);
    timerName->AddComponents<TextComp>("TIME", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    timerName->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    timerName->GetComponent<TextComp>()->SetCentered(true);
    timerName->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(timerName);
    
    const auto timerHUD = scene->CreateGameObject("Z_timerHUD");
    timerHUD->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.85f + 12, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f);
    timerHUD->AddComponents<TextComp>("300", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    timerHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    timerHUD->GetComponent<TextComp>()->SetCentered(true);
    timerHUD->AddComponents<ScoreCounter>(300, true);
    timerHUD->GetComponent<ScoreCounter>()->SetString("");
    timerHUD->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true, 3);
    timerHUD->AddComponents<thomasWasLate::TimerScript>();
    timerHUD->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(timerHUD);

    const auto fpsCounter = scene->CreateGameObject("Z_FPSCounter");
    fpsCounter->AddComponents<TextComp>("0 FPS", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    fpsCounter->GetComponent<TextComp>()->GetText().setCharacterSize(10);
    fpsCounter->AddComponents<FPSCounter>();
    fpsCounter->AddComponents<Transform>(window::VIEWPORT.x - 100, 40);
    fpsCounter->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(fpsCounter);

#pragma region Commands
    auto& input = InputManager::GetInstance();

    input.BindCommand<thomasWasLate::MoveCharacter>(PlayerIdx::KEYBOARD, KeyState::HELD, sf::Keyboard::Key::D, player, sf::Vector2f{ 1.f, 0.f }, true);
    input.BindCommand<thomasWasLate::MoveCharacter>(PlayerIdx::KEYBOARD, KeyState::HELD, sf::Keyboard::Key::A, player, sf::Vector2f{ -1.f, 0.f }, true);
    input.BindCommand<thomasWasLate::MoveCharacter>(PlayerIdx::KEYBOARD, KeyState::HELD, sf::Keyboard::Key::Right, player, sf::Vector2f{ 1.f, 0.f }, true);
    input.BindCommand<thomasWasLate::MoveCharacter>(PlayerIdx::KEYBOARD, KeyState::HELD, sf::Keyboard::Key::Left, player, sf::Vector2f{ -1.f, 0.f }, true);
    input.BindCommand<thomasWasLate::MoveCharacter>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Key::D, player, sf::Vector2f{ 1.f, 0.f }, false);
    input.BindCommand<thomasWasLate::MoveCharacter>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Key::A, player, sf::Vector2f{ -1.f, 0.f }, false);
    input.BindCommand<thomasWasLate::MoveCharacter>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Key::Right, player, sf::Vector2f{ 1.f, 0.f }, false);
    input.BindCommand<thomasWasLate::MoveCharacter>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Key::Left, player, sf::Vector2f{ -1.f, 0.f }, false);
    
    input.BindCommand<thomasWasLate::CharacterJump>(PlayerIdx::KEYBOARD, KeyState::HELD, sf::Keyboard::Key::W, player, true);
    input.BindCommand<thomasWasLate::CharacterJump>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Key::W, player, false);
    input.BindCommand<thomasWasLate::CharacterJump>(PlayerIdx::KEYBOARD, KeyState::HELD, sf::Keyboard::Key::Space, player, true);
    input.BindCommand<thomasWasLate::CharacterJump>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Key::Space, player, false);
    input.BindCommand<thomasWasLate::CharacterJump>(PlayerIdx::KEYBOARD, KeyState::HELD, sf::Keyboard::Key::Up, player, true);
    input.BindCommand<thomasWasLate::CharacterJump>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Key::Up, player, false);
    
    input.BindCommand<thomasWasLate::Sprint>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Key::LShift, player, true);
    input.BindCommand<thomasWasLate::Sprint>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Key::LShift, player, false);
    // input.BindCommand<thomasWasLate::TempAddImpulse>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Key::Space, testBall);
    
#pragma endregion

#pragma region Events

    player->GetComponent<thomasWasLate::PlayerCharacter>()->OnPointsScoredEvent.AddListener(scoreHUD->GetComponent<ScoreCounter>(), &ScoreCounter::IncreaseScore);
#pragma endregion
}