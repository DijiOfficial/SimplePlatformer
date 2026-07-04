#include "GameLoader.h"
#include "GameState.h"
#include "../Components/Backgrounds/BackgroundHandler.h"
#include "../Components/Backgrounds/CustomBackgroundRenderer.h"
#include "../Components/Player/CameraClamping.h"
#include "../Components/Player/PlayerInputManager.h"
#include "../Components/Other/HUD/HudManager.h"
#include "../Components/Player/PlayerCharacter.h"
#include "../Components/Other/HUD/TimerScript.h"
#include "../Components/Player/BroadcastPlayerPosition.h"
#include "../Components/Player/MenuTransition.h"
#include "../Input/CustomCommands.h"
#include "../Singletons/GameManager.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Components/TextureComp.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Interfaces/ISoundSystem.h"
#include "Engine/Components/Render.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/FPSCounter.h"
#include "Engine/Components/ScoreCounter.h"
#include "Engine/Components/Sprite.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Components/TextComp.h"
#include "Engine/Core/Engine.h"
#include "Engine/Singleton/GameStateManager.h"
#include "Engine/Singleton/TimerManager.h"

#include <format>

#include "../Components/Other/SceneLoader.h"
#include "../Components/Player/CheckPlayerTopPixel.h"
#include "Engine/Components/ShapeRender.h"
#include "Engine/Core/Renderer.h"

using namespace diji;

void SceneLoader::GameStartUp()
{
#ifndef NDEBUG
    ServiceLocator::RegisterSoundSystem(std::make_unique<LoggingSoundSystem>(std::make_unique<ISFMLSoundSystem>()));
#else
    ServiceLocator::RegisterSoundSystem(std::make_unique<ISFMLSoundSystem>());
#endif

    //todo: this would be better if it was part of the CreateScene function.
    SceneManager::GetInstance().RegisterScene(static_cast<int>(superMarioBros::superMarioBrosState::Level), Level);
    SceneManager::GetInstance().RegisterScene(static_cast<int>(superMarioBros::superMarioBrosState::StartMenu), StartMenu);
    SceneManager::GetInstance().RegisterScene(static_cast<int>(superMarioBros::superMarioBrosState::LivesDisplayMenu), LivesDisplayMenu);
    SceneManager::GetInstance().RegisterScene(static_cast<int>(superMarioBros::superMarioBrosState::GameOver), GameOverMenu);
    SceneManager::GetInstance().RegisterScene(static_cast<int>(superMarioBros::superMarioBrosState::TransitionToNextLevel), TransitionToNextLevel);
    SceneManager::GetInstance().RegisterScene(static_cast<int>(superMarioBros::superMarioBrosState::LevelEditor), LevelEditor);

    // StartMenu();
    // Level();
    LevelEditor();
}

#pragma region Menus
void SceneLoader::StartMenu()
{
    SceneManager::GetInstance().SetActiveScene(static_cast<int>(superMarioBros::superMarioBrosState::StartMenu));
    const auto& scene = SceneManager::GetInstance().CreateScene(static_cast<int>(superMarioBros::superMarioBrosState::StartMenu));
    GameStateManager::GetInstance().SetNewGameState(static_cast<GameState>(superMarioBros::superMarioBrosState::StartMenu));
    Renderer::GetInstance().SetBackgroundColor(sf::Color(148, 148, 255));

    const auto staticBackground = scene->CreateGameObject("A_StaticBackground");
    staticBackground->SetObjectPosition({ 1920 * 0.5f, 1080 * 0.5f });
    // staticBackground->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.5f, static_cast<float>(window::VIEWPORT.y) * 0.5f });
    staticBackground->AddComponent<TextureComp>("graphics/menuBackground.png");
    staticBackground->AddComponent<Render>(1.0f);
    scene->SetGameObjectAsStaticBackground(staticBackground);

    const auto camera = scene->CreateCameraObject("A_Camera");
    camera->SetObjectPosition({ 0, 0 });
    camera->AddComponent<Camera>(sf::Vector2u{ 1920, 1080 });

    const auto player = scene->CreateGameObject("X_PlayerChar");
    player->SetObjectPosition({ 400, 910 });
    player->AddComponent<SpriteRenderComponent>("graphics/player.png", sf::Vector2i{ 16, 16 }, 1, 0.05f);
    player->GetComponent<SpriteRenderComponent>()->SetScale(3);
    player->GetComponent<SpriteRenderComponent>()->Pause();
    
     // Create the HUD
    const auto playerOneText = scene->CreateGameObject("Z_PlayerOneText");
    playerOneText->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.5f, static_cast<float>(window::VIEWPORT.y) * 0.65f });
    playerOneText->AddComponent<TextComp>("PUSH START BUTTON", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    playerOneText->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    playerOneText->GetComponent<TextComp>()->SetCentered(true);
    playerOneText->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(playerOneText);

    const auto highScore = scene->CreateGameObject("Z_HighScore");
    highScore->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.5f, static_cast<float>(window::VIEWPORT.y) * 0.70f });
    highScore->AddComponent<TextComp>("000000", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    highScore->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    highScore->GetComponent<TextComp>()->SetCentered(true);
    highScore->AddComponent<ScoreCounter>(superMarioBros::GameManager::GetInstance().GetHighScoreFromFile(), true);
    highScore->GetComponent<ScoreCounter>()->SetString("TOP-");
    highScore->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true);
    highScore->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(highScore);

    const auto myName = scene->CreateGameObject("Z_Disclaimer");
    myName->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.5f, static_cast<float>(window::VIEWPORT.y) * 0.80f });
    myName->AddComponent<TextComp>("2025 BURGISSER DYLAN\n", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    myName->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    myName->GetComponent<TextComp>()->SetCentered(true);
    myName->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(myName);
    
    const auto marioName = scene->CreateGameObject("Z_MarioName");
    marioName->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.15f, static_cast<float>(window::VIEWPORT.y) * 0.05f });
    marioName->AddComponent<TextComp>("MARIO", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    marioName->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    marioName->GetComponent<TextComp>()->SetCentered(true);
    marioName->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(marioName);
    
    const auto scoreHUD = scene->CreateGameObject("Z_ScoreHUD");
    scoreHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.15f + 13, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    scoreHUD->AddComponent<TextComp>("000000", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    scoreHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    scoreHUD->GetComponent<TextComp>()->SetCentered(true);
    scoreHUD->AddComponent<ScoreCounter>(0, true);
    scoreHUD->GetComponent<ScoreCounter>()->SetString("");
    scoreHUD->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true);
    scoreHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(scoreHUD);

    const auto coinsCounterHud = scene->CreateGameObject("Z_CoinsCounterHUD");
    coinsCounterHud->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.4f, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    coinsCounterHud->AddComponent<TextComp>("00", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    coinsCounterHud->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    coinsCounterHud->GetComponent<TextComp>()->SetCentered(true);
    coinsCounterHud->AddComponent<ScoreCounter>(0, true);
    coinsCounterHud->GetComponent<ScoreCounter>()->SetString("");
    coinsCounterHud->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true, 2);
    coinsCounterHud->GetComponent<ScoreCounter>()->SetGoalScore(100);
    coinsCounterHud->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(coinsCounterHud);
    
    const auto xMarkHUD = scene->CreateGameObject("Z_xMarkHUD");
    xMarkHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.4f - 38, static_cast<float>(window::VIEWPORT.y) * 0.05f + 32.f });
    xMarkHUD->AddComponent<TextComp>("X", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    xMarkHUD->GetComponent<TextComp>()->GetText().setCharacterSize(18);
    xMarkHUD->GetComponent<TextComp>()->SetCentered(true);
    xMarkHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(xMarkHUD);
    
    const auto coinImageHud = scene->CreateGameObject("Z_CoinsImageHUD");
    coinImageHud->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.4f - 60, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    coinImageHud->AddComponent<SpriteRenderComponent>("graphics/HUDCoins.png", sf::Vector2i{ 24, 24 }, 6, 0.15f);
    scene->SetGameObjectAsCanvasObject(coinImageHud);

    const auto worldNameHUD = scene->CreateGameObject("Z_worldNameHUD");
    worldNameHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.6f + 40, static_cast<float>(window::VIEWPORT.y) * 0.05f });
    worldNameHUD->AddComponent<TextComp>("WORLD", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    worldNameHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    worldNameHUD->GetComponent<TextComp>()->SetCentered(true);
    worldNameHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(worldNameHUD);

    const auto worldCountHUD = scene->CreateGameObject("Z_worldCountHUD");
    worldCountHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.6f + 40, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    worldCountHUD->AddComponent<TextComp>("1-1", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    worldCountHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    worldCountHUD->GetComponent<TextComp>()->SetCentered(true);
    worldCountHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(worldCountHUD);

    const auto timerName = scene->CreateGameObject("Z_timerName");
    timerName->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.85f, static_cast<float>(window::VIEWPORT.y) * 0.05f });
    timerName->AddComponent<TextComp>("TIME", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    timerName->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    timerName->GetComponent<TextComp>()->SetCentered(true);
    timerName->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(timerName);
    
    const auto fpsCounter = scene->CreateGameObject("Z_FPSCounter");
    fpsCounter->AddComponent<TextComp>("0 FPS", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    fpsCounter->GetComponent<TextComp>()->GetText().setCharacterSize(10);
    fpsCounter->AddComponent<FPSCounter>();
    fpsCounter->SetObjectPosition(sf::Vector2f{ static_cast<float>(window::VIEWPORT.x - 100), 40 });
    fpsCounter->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(fpsCounter);


#pragma region Commands
    auto& input = InputManager::GetInstance();

    input.BindCommand<superMarioBros::StartGame>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::Enter, nullptr);
    input.BindCommand<superMarioBros::StartGame>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::Back, nullptr);
    input.BindCommand<superMarioBros::StartGame>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::Start, nullptr);
#pragma endregion
}

void SceneLoader::LivesDisplayMenu()
{
    SceneManager::GetInstance().SetActiveScene(static_cast<int>(superMarioBros::superMarioBrosState::LivesDisplayMenu));
    const auto& scene = SceneManager::GetInstance().CreateScene(static_cast<int>(superMarioBros::superMarioBrosState::LivesDisplayMenu));
    GameStateManager::GetInstance().SetNewGameState(static_cast<GameState>(superMarioBros::superMarioBrosState::LivesDisplayMenu));
    Renderer::GetInstance().SetBackgroundColor(sf::Color::Black);

    const auto sceneChanger = scene->CreateCameraObject("A_sceneChanger");
    const auto& loader = sceneChanger->AddComponent<superMarioBros::SceneLoader>(2.6f);
    loader->SetCallback([]
    {
        superMarioBros::GameManager::GetInstance().SwitchToNextScene();
    });
    
    const auto camera = scene->CreateCameraObject("A_Camera");
    camera->SetObjectPosition({ 0, 0 });
    camera->AddComponent<Camera>(sf::Vector2f{ 1920.f, 1080.f });
#pragma region HUD
     // Create the HUD    
    const auto marioName = scene->CreateGameObject("Z_MarioName");
    marioName->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.15f, static_cast<float>(window::VIEWPORT.y) * 0.05f });
    marioName->AddComponent<TextComp>("MARIO", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    marioName->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    marioName->GetComponent<TextComp>()->SetCentered(true);
    marioName->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(marioName);
    
    const auto scoreHUD = scene->CreateGameObject("Z_ScoreHUD");
    scoreHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.15f + 13, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    scoreHUD->AddComponent<TextComp>("000000", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    scoreHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    scoreHUD->GetComponent<TextComp>()->SetCentered(true);
    const int score = superMarioBros::GameManager::GetInstance().GetPlayerInfo().totalScore;
    scoreHUD->AddComponent<ScoreCounter>(score, true);
    scoreHUD->GetComponent<ScoreCounter>()->SetString("");
    scoreHUD->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true);
    scoreHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(scoreHUD);

    const auto coinsCounterHud = scene->CreateGameObject("Z_CoinsCounterHUD");
    coinsCounterHud->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.4f, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    coinsCounterHud->AddComponent<TextComp>("00", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    coinsCounterHud->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    coinsCounterHud->GetComponent<TextComp>()->SetCentered(true);
    const int coins = superMarioBros::GameManager::GetInstance().GetPlayerInfo().totalCoins;
    coinsCounterHud->AddComponent<ScoreCounter>(coins, true);
    coinsCounterHud->GetComponent<ScoreCounter>()->SetString("");
    coinsCounterHud->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true, 2);
    coinsCounterHud->GetComponent<ScoreCounter>()->SetGoalScore(100);
    coinsCounterHud->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(coinsCounterHud);
    
    const auto xMarkHUD = scene->CreateGameObject("Z_xMarkHUD");
    xMarkHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.4f - 38, static_cast<float>(window::VIEWPORT.y) * 0.05f + 32.f });
    xMarkHUD->AddComponent<TextComp>("X", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    xMarkHUD->GetComponent<TextComp>()->GetText().setCharacterSize(18);
    xMarkHUD->GetComponent<TextComp>()->SetCentered(true);
    xMarkHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(xMarkHUD);
    
    const auto coinImageHud = scene->CreateGameObject("Z_CoinsImageHUD");
    coinImageHud->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.4f - 60, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    coinImageHud->AddComponent<SpriteRenderComponent>("graphics/HUDCoins.png", sf::Vector2i{ 24, 24 }, 6, 0.15f);
    scene->SetGameObjectAsCanvasObject(coinImageHud);

    const auto worldNameHUD = scene->CreateGameObject("Z_worldNameHUD");
    worldNameHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.6f + 40, static_cast<float>(window::VIEWPORT.y) * 0.05f });
    worldNameHUD->AddComponent<TextComp>("WORLD", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    worldNameHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    worldNameHUD->GetComponent<TextComp>()->SetCentered(true);
    worldNameHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(worldNameHUD);

    const auto worldCountHUD = scene->CreateGameObject("Z_worldCountHUD");
    worldCountHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.6f + 40, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    const std::string levelStr = std::format("1-{}", superMarioBros::GameManager::GetInstance().GetPlayerInfo().currentLevel);
    worldCountHUD->AddComponent<TextComp>(levelStr, "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    worldCountHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    worldCountHUD->GetComponent<TextComp>()->SetCentered(true);
    worldCountHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(worldCountHUD);

    const auto worldCountCentered = scene->CreateGameObject("Z_worldCountCentered");
    worldCountCentered->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.5f, static_cast<float>(window::VIEWPORT.y) * 0.4f });
    const std::string levelStrCenter = std::format("WORLD 1-{}", superMarioBros::GameManager::GetInstance().GetPlayerInfo().currentLevel);
    worldCountCentered->AddComponent<TextComp>(levelStrCenter, "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    worldCountCentered->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    worldCountCentered->GetComponent<TextComp>()->SetCentered(true);
    worldCountCentered->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(worldCountCentered);

    const auto livesDisplay = scene->CreateGameObject("Z_livesDisplay");
    livesDisplay->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.49f, static_cast<float>(window::VIEWPORT.y) * 0.5f });
    const std::string livesStr = std::format("   x  {}", superMarioBros::GameManager::GetInstance().GetPlayerInfo().totalLives);
    livesDisplay->AddComponent<TextComp>(livesStr, "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    livesDisplay->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    livesDisplay->GetComponent<TextComp>()->SetCentered(true);
    livesDisplay->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(livesDisplay);

    const auto player = scene->CreateGameObject("X_PlayerChar");
    player->SetObjectPosition({ 965.f, static_cast<float>(window::VIEWPORT.y) * 0.5f });
    player->AddComponent<SpriteRenderComponent>("graphics/player.png", sf::Vector2i{ 16, 16 }, 1, 0.05f);
    player->GetComponent<SpriteRenderComponent>()->SetScale(3);
    player->GetComponent<SpriteRenderComponent>()->Pause();

    const auto timerName = scene->CreateGameObject("Z_timerName");
    timerName->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.85f, static_cast<float>(window::VIEWPORT.y) * 0.05f });
    timerName->AddComponent<TextComp>("TIME", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    timerName->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    timerName->GetComponent<TextComp>()->SetCentered(true);
    timerName->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(timerName);

    const auto fpsCounter = scene->CreateGameObject("Z_FPSCounter");
    fpsCounter->AddComponent<TextComp>("0 FPS", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    fpsCounter->GetComponent<TextComp>()->GetText().setCharacterSize(10);
    fpsCounter->AddComponent<FPSCounter>();
    fpsCounter->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x - 100), 40 });
    fpsCounter->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(fpsCounter);
#pragma endregion
}

void SceneLoader::GameOverMenu()
{
    // ReSharper disable once StringLiteralTypo
    ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_gameover.wav", false);
    
    SceneManager::GetInstance().SetActiveScene(static_cast<int>(superMarioBros::superMarioBrosState::GameOver));
    const auto& scene = SceneManager::GetInstance().CreateScene(static_cast<int>(superMarioBros::superMarioBrosState::GameOver));
    GameStateManager::GetInstance().SetNewGameState(static_cast<GameState>(superMarioBros::superMarioBrosState::GameOver));
    Renderer::GetInstance().SetBackgroundColor(sf::Color::Black);

    superMarioBros::GameManager::GetInstance().SaveHighScoreToFile();

    const auto sceneChanger = scene->CreateCameraObject("A_sceneChanger");
    const auto& loader = sceneChanger->AddComponent<superMarioBros::SceneLoader>(7.0f);
    loader->SetCallback([]
    {
        superMarioBros::GameManager::GetInstance().ResetPlayerInfo();
        SceneManager::GetInstance().SetNextSceneToActivate(static_cast<int>(superMarioBros::superMarioBrosState::StartMenu));
    });
    
    const auto camera = scene->CreateCameraObject("A_Camera");
    camera->SetObjectPosition({ 0, 0 });
    camera->AddComponent<Camera>(sf::Vector2f{ 1920.f, 1080.f });

     // Create the HUD    
    const auto marioName = scene->CreateGameObject("Z_MarioName");
    marioName->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.15f, static_cast<float>(window::VIEWPORT.y) * 0.05f });
    marioName->AddComponent<TextComp>("MARIO", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    marioName->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    marioName->GetComponent<TextComp>()->SetCentered(true);
    marioName->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(marioName);
    
    const auto scoreHUD = scene->CreateGameObject("Z_ScoreHUD");
    scoreHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.15f + 13, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    scoreHUD->AddComponent<TextComp>("000000", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    scoreHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    scoreHUD->GetComponent<TextComp>()->SetCentered(true);
    const int score = superMarioBros::GameManager::GetInstance().GetPlayerInfo().totalScore;
    scoreHUD->AddComponent<ScoreCounter>(score, true);
    scoreHUD->GetComponent<ScoreCounter>()->SetString("");
    scoreHUD->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true);
    scoreHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(scoreHUD);

    const auto coinsCounterHud = scene->CreateGameObject("Z_CoinsCounterHUD");
    coinsCounterHud->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.4f, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    coinsCounterHud->AddComponent<TextComp>("00", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    coinsCounterHud->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    coinsCounterHud->GetComponent<TextComp>()->SetCentered(true);
    const int coins = superMarioBros::GameManager::GetInstance().GetPlayerInfo().totalCoins;
    coinsCounterHud->AddComponent<ScoreCounter>(coins, true);
    coinsCounterHud->GetComponent<ScoreCounter>()->SetString("");
    coinsCounterHud->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true, 2);
    coinsCounterHud->GetComponent<ScoreCounter>()->SetGoalScore(100);
    coinsCounterHud->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(coinsCounterHud);
    
    const auto xMarkHUD = scene->CreateGameObject("Z_xMarkHUD");
    xMarkHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.4f - 38, static_cast<float>(window::VIEWPORT.y) * 0.05f + 32.f });
    xMarkHUD->AddComponent<TextComp>("X", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    xMarkHUD->GetComponent<TextComp>()->GetText().setCharacterSize(18);
    xMarkHUD->GetComponent<TextComp>()->SetCentered(true);
    xMarkHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(xMarkHUD);
    
    const auto coinImageHud = scene->CreateGameObject("Z_CoinsImageHUD");
    coinImageHud->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.4f - 60, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    coinImageHud->AddComponent<SpriteRenderComponent>("graphics/HUDCoins.png", sf::Vector2i{ 24, 24 }, 6, 0.15f);
    scene->SetGameObjectAsCanvasObject(coinImageHud);

    const auto worldNameHUD = scene->CreateGameObject("Z_worldNameHUD");
    worldNameHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.6f + 40, static_cast<float>(window::VIEWPORT.y) * 0.05f });
    worldNameHUD->AddComponent<TextComp>("WORLD", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    worldNameHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    worldNameHUD->GetComponent<TextComp>()->SetCentered(true);
    worldNameHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(worldNameHUD);

    const auto worldCountHUD = scene->CreateGameObject("Z_worldCountHUD");
    worldCountHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.6f + 40, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    const std::string levelStr = std::format("1-{}", superMarioBros::GameManager::GetInstance().GetPlayerInfo().currentLevel);
    worldCountHUD->AddComponent<TextComp>(levelStr, "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    worldCountHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    worldCountHUD->GetComponent<TextComp>()->SetCentered(true);
    worldCountHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(worldCountHUD);

    const auto gameOver = scene->CreateGameObject("Z_gameOver");
    gameOver->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.5f, static_cast<float>(window::VIEWPORT.y) * 0.5f });
    gameOver->AddComponent<TextComp>("GAME OVER", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    gameOver->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    gameOver->GetComponent<TextComp>()->SetCentered(true);
    gameOver->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(gameOver);

    const auto timerName = scene->CreateGameObject("Z_timerName");
    timerName->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.85f, static_cast<float>(window::VIEWPORT.y) * 0.05f });
    timerName->AddComponent<TextComp>("TIME", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    timerName->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    timerName->GetComponent<TextComp>()->SetCentered(true);
    timerName->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(timerName);

    const auto fpsCounter = scene->CreateGameObject("Z_FPSCounter");
    fpsCounter->AddComponent<TextComp>("0 FPS", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    fpsCounter->GetComponent<TextComp>()->GetText().setCharacterSize(10);
    fpsCounter->AddComponent<FPSCounter>();
    fpsCounter->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x - 100), 40 });
    fpsCounter->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(fpsCounter);
}

void SceneLoader::TransitionToNextLevel()
{
    ServiceLocator::GetSoundSystem().AddSoundRequest("sound/Transition.mp3", false);

    SceneManager::GetInstance().SetActiveScene(static_cast<int>(superMarioBros::superMarioBrosState::TransitionToNextLevel));
    const auto& scene = SceneManager::GetInstance().CreateScene(static_cast<int>(superMarioBros::superMarioBrosState::TransitionToNextLevel));
    GameStateManager::GetInstance().SetNewGameState(static_cast<GameState>(superMarioBros::superMarioBrosState::TransitionToNextLevel));
    Renderer::GetInstance().SetBackgroundColor(sf::Color(92, 148, 252));

    const auto sceneChanger = scene->CreateCameraObject("A_sceneChanger");
    const auto& loader = sceneChanger->AddComponent<superMarioBros::SceneLoader>(5.6f);
    loader->SetCallback([]
    {
        superMarioBros::GameManager::GetInstance().SwitchToNextScene();
    });
  
    const sf::FloatRect arena{ sf::Vector2f{ 0, -(115 * 4.5) }, sf::Vector2f{ 1920.f, 1080.f } };
    const auto camera = scene->CreateCameraObject("A_Camera");
    camera->SetObjectPosition({ 0, 0 });
    camera->AddComponent<Camera>(sf::Vector2f{ 1920.f, 1080.f });
    camera->GetComponent<Camera>()->SetLevelBoundaries(arena);

    const auto player = scene->CreateGameObject("X_PlayerChar");
    const auto staticBackground = scene->CreateGameObject("A_StaticBackground");
    staticBackground->SetObjectPosition({ 0, 178 });
    staticBackground->AddComponent<TextureComp>("graphics/background.png");
    staticBackground->AddComponent<superMarioBros::CustomBackgroundRenderer>(player->GetRootComponent());

    const auto background = scene->CreateGameObject("B_Background");
    background->SetObjectPosition({ 0, 0 });
    background->AddComponent<Sprite>("graphics/tiles_sheet.png");
    background->AddComponent<Render>();
    background->AddComponent<superMarioBros::BackgroundHandler>();
    
    player->SetObjectPosition({ 200, 475 });
    player->AddComponent<SpriteRenderComponent>("graphics/player.png", sf::Vector2i{ 16, 16 }, 3, 0.05f);
    player->GetComponent<SpriteRenderComponent>()->SetScale(3);
    player->AddComponent<Collider>(CollisionShape::ShapeType::RECT, sf::Vector2f{ 48, 48 });
    const auto collider = player->GetComponent<Collider>();
    collider->SetCollisionResponse(Collider::CollisionResponse::Ignore);
    collider->SetColliderActive(false);
    collider->SetAffectedByGravity(false);
    player->AddComponent<superMarioBros::PlayerCharacter>();
    player->GetComponent<superMarioBros::PlayerCharacter>()->PausePlayer();
    player->AddComponent<superMarioBros::MenuTransition>();
    
#pragma region HUD
    // Create the HUD
    const auto marioName = scene->CreateGameObject("Z_MarioName");
    marioName->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.15f, static_cast<float>(window::VIEWPORT.y) * 0.05f });
    marioName->AddComponent<TextComp>("MARIO", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    marioName->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    marioName->GetComponent<TextComp>()->SetCentered(true);
    marioName->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(marioName);
    
    const auto scoreHUD = scene->CreateGameObject("Z_ScoreHUD");
    scoreHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.15f + 13, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    scoreHUD->AddComponent<TextComp>("000000", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    scoreHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    scoreHUD->GetComponent<TextComp>()->SetCentered(true);
    const int score = superMarioBros::GameManager::GetInstance().GetPlayerInfo().totalScore;
    scoreHUD->AddComponent<ScoreCounter>(score, true);
    scoreHUD->GetComponent<ScoreCounter>()->SetString("");
    scoreHUD->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true);
    scoreHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(scoreHUD);

    const auto coinsCounterHud = scene->CreateGameObject("Z_CoinsCounterHUD");
    coinsCounterHud->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.4f, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    coinsCounterHud->AddComponent<TextComp>("00", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    coinsCounterHud->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    coinsCounterHud->GetComponent<TextComp>()->SetCentered(true);
    const int coins = superMarioBros::GameManager::GetInstance().GetPlayerInfo().totalCoins;
    coinsCounterHud->AddComponent<ScoreCounter>(coins, true);
    coinsCounterHud->GetComponent<ScoreCounter>()->SetString("");
    coinsCounterHud->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true, 2);
    coinsCounterHud->GetComponent<ScoreCounter>()->SetGoalScore(100);
    coinsCounterHud->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(coinsCounterHud);
    
    const auto xMarkHUD = scene->CreateGameObject("Z_xMarkHUD");
    xMarkHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.4f - 38, static_cast<float>(window::VIEWPORT.y) * 0.05f + 32.f });
    xMarkHUD->AddComponent<TextComp>("X", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    xMarkHUD->GetComponent<TextComp>()->GetText().setCharacterSize(18);
    xMarkHUD->GetComponent<TextComp>()->SetCentered(true);
    xMarkHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(xMarkHUD);
    
    const auto coinImageHud = scene->CreateGameObject("Z_CoinsImageHUD");
    coinImageHud->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.4f - 60, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    coinImageHud->AddComponent<SpriteRenderComponent>("graphics/HUDCoins.png", sf::Vector2i{ 24, 24 }, 6, 0.15f);
    scene->SetGameObjectAsCanvasObject(coinImageHud);

    const auto worldNameHUD = scene->CreateGameObject("Z_worldNameHUD");
    worldNameHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.6f + 40, static_cast<float>(window::VIEWPORT.y) * 0.05f });
    worldNameHUD->AddComponent<TextComp>("WORLD", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    worldNameHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    worldNameHUD->GetComponent<TextComp>()->SetCentered(true);
    worldNameHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(worldNameHUD);

    const auto worldCountHUD = scene->CreateGameObject("Z_worldCountHUD");
    worldCountHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.6f + 40, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    const std::string levelStr = std::format("1-{}", superMarioBros::GameManager::GetInstance().GetPlayerInfo().currentLevel);
    worldCountHUD->AddComponent<TextComp>(levelStr, "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    worldCountHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    worldCountHUD->GetComponent<TextComp>()->SetCentered(true);
    worldCountHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(worldCountHUD);

    const auto timerName = scene->CreateGameObject("Z_timerName");
    timerName->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.85f, static_cast<float>(window::VIEWPORT.y) * 0.05f });
    timerName->AddComponent<TextComp>("TIME", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    timerName->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    timerName->GetComponent<TextComp>()->SetCentered(true);
    timerName->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(timerName);

    const auto fpsCounter = scene->CreateGameObject("Z_FPSCounter");
    fpsCounter->AddComponent<TextComp>("0 FPS", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    fpsCounter->GetComponent<TextComp>()->GetText().setCharacterSize(10);
    fpsCounter->AddComponent<FPSCounter>();
    fpsCounter->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x - 100), 40 });
    fpsCounter->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(fpsCounter);
#pragma endregion
}
#pragma endregion

void SceneLoader::Level()
{
    ServiceLocator::GetSoundSystem().AddSoundRequest("sound/LevelMusic.mp3", true);

    SceneManager::GetInstance().SetActiveScene(static_cast<int>(superMarioBros::superMarioBrosState::Level));
    const auto& scene = SceneManager::GetInstance().CreateScene(static_cast<int>(superMarioBros::superMarioBrosState::Level));
    GameStateManager::GetInstance().SetNewGameState(static_cast<GameState>(superMarioBros::superMarioBrosState::Level));
    Renderer::GetInstance().SetBackgroundColor(sf::Color(92, 148, 252));

    const auto player = scene->CreateGameObject("X_PlayerChar");
    const auto staticBackground = scene->CreateGameObject("A_StaticBackground");
    staticBackground->SetObjectPosition({ 0, 178 });
    staticBackground->AddComponent<TextureComp>("graphics/background.png");
    staticBackground->AddComponent<superMarioBros::CustomBackgroundRenderer>(player->GetRootComponent());

    const auto background = scene->CreateGameObject("B_Background");
    background->SetObjectPosition({ 0, 0 });
    background->AddComponent<Sprite>("graphics/tiles_sheet.png");
    background->AddComponent<Render>();
    background->AddComponent<superMarioBros::BackgroundHandler>();

    const sf::FloatRect arena{ sf::Vector2f{ 0, -(115 * 4.5) }, sf::Vector2f{ 12000.f, 1080.f } };
    const auto camera = scene->CreateCameraObject("A_Camera");
    camera->SetObjectPosition({ 0, 0 });
    camera->AddComponent<Camera>(sf::Vector2f{ 1920.f, 1080.f });
    camera->GetComponent<Camera>()->SetLevelBoundaries(arena);

    player->SetObjectPosition({ 200, 0 });
    player->AddComponent<SpriteRenderComponent>("graphics/player.png", sf::Vector2i{ 16, 16 }, 3, 0.05f);
    player->GetComponent<SpriteRenderComponent>()->SetScale(3);
    player->AddComponent<Collider>(CollisionShape::ShapeType::RECT, sf::Vector2f{ 48, 48 });
    player->GetComponent<Collider>()->SetRestitution(0.f);
    player->GetComponent<Collider>()->SetMass(0.89f);
    player->GetComponent<Collider>()->SetStaticFriction(0.25f);
    player->GetComponent<Collider>()->SetKineticFriction(0.15f);
    player->GetComponent<Collider>()->SetGenerateHitEvents(true);
    player->GetComponent<Collider>()->SetIsOnlyApplyingGroundFriction(true);
    player->GetComponent<Collider>()->SetMaxVelocity(sf::Vector2f{ 800.f, 1000.f });
    player->GetComponent<Collider>()->SetTag("player");
    player->AddComponent<superMarioBros::PlayerCharacter>();
    player->AddComponent<superMarioBros::PlayerInputManager>(0.5f);
    player->AddComponent<superMarioBros::CameraClamping>();
    player->AddComponent<superMarioBros::BroadcastPlayerPosition>();
    player->AddComponent<superMarioBros::CheckPlayerTopPixel>();
    player->AddComponent<ShapeRender>(true);

    SceneManager::GetInstance().GetPhysicsWorld()->SetGravity(sf::Vector2f{ 0, 980 * 3.f });

    // const auto testObject = scene->CreateGameObject("Y_testObject");
    // // auto testObject = std::make_unique<diji::GameObject>();
    // testObject->SetObjectPosition({ 6000, 400 });
    // testObject->AddComponent<SpriteRenderComponent>("graphics/piranhaPlant.png", sf::Vector2i{ 50, 75 }, 2, 0.135f);
    // testObject->AddComponent<Collider>(CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 75 });
    // const auto collider = testObject->GetComponent<Collider>();
    // collider->SetIsMoveable(false);
    // collider->SetTag("plant");
    // collider->SetAffectedByGravity(false);
    // collider->SetCollisionResponse(Collider::CollisionResponse::Overlap);
    // testObject->AddComponent<superMarioBros::PiranhaPlant>();
 
    
    // const auto testObject = scene->CreateGameObject("Y_testObject");
    // testObject->SetObjectPosition({ 11000, 250 });
    // testObject->AddComponent<SpriteRenderComponent>("graphics/explosion.png", sf::Vector2i{ 50, 50 }, 3, 0.135f);
    // testObject->AddComponent<AutoDestroy>(0.405f);

    // auto breakableBlock = scene->CreateGameObject("Y_testObject");
    // // auto breakableBlock = std::make_unique<diji::GameObject>();
    // breakableBlock->SetObjectPosition({600, 300);
    // breakableBlock->AddComponent<diji::SpriteRenderComponent>("graphics/breakableBlock.png", sf::Vector2i{ 50, 50 }, 1, 0.0f);
    // breakableBlock->GetComponent<diji::SpriteRenderComponent>()->SetLooping(false);
    // breakableBlock->AddComponent<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
    // const auto collider = breakableBlock->GetComponent<diji::Collider>();
    // collider->SetTag("breakBlock");
    // collider->SetAffectedByGravity(false);
    // collider->SetGenerateHitEvents(true);
    // collider->SetIsMoveable(false);
    // breakableBlock->AddComponent<superMarioBros::BreakableBlock>();

#pragma region HUD
    // Create the HUD
    const auto marioName = scene->CreateGameObject("Z_MarioName");
    marioName->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.15f, static_cast<float>(window::VIEWPORT.y) * 0.05f });
    marioName->AddComponent<TextComp>("MARIO", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    marioName->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    marioName->GetComponent<TextComp>()->SetCentered(true);
    marioName->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(marioName);
    
    const auto scoreHUD = scene->CreateGameObject("Z_ScoreHUD");
    scoreHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.15f + 13, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    scoreHUD->AddComponent<TextComp>("000000", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    scoreHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    scoreHUD->GetComponent<TextComp>()->SetCentered(true);
    const int score = superMarioBros::GameManager::GetInstance().GetPlayerInfo().totalScore;
    scoreHUD->AddComponent<ScoreCounter>(0, true);
    scoreHUD->GetComponent<ScoreCounter>()->SetString("");
    scoreHUD->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true);
    scoreHUD->GetComponent<ScoreCounter>()->SetScore(score);
    scoreHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(scoreHUD);

    const auto coinsCounterHud = scene->CreateGameObject("Z_CoinsCounterHUD");
    coinsCounterHud->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.4f, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    coinsCounterHud->AddComponent<TextComp>("00", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    coinsCounterHud->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    coinsCounterHud->GetComponent<TextComp>()->SetCentered(true);
    const int coins = superMarioBros::GameManager::GetInstance().GetPlayerInfo().totalCoins;
    coinsCounterHud->AddComponent<ScoreCounter>(coins, true);
    coinsCounterHud->GetComponent<ScoreCounter>()->SetString("");
    coinsCounterHud->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true, 2);
    coinsCounterHud->GetComponent<ScoreCounter>()->SetGoalScore(100);
    coinsCounterHud->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(coinsCounterHud);
    
    const auto xMarkHUD = scene->CreateGameObject("Z_xMarkHUD");
    xMarkHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.4f - 38, static_cast<float>(window::VIEWPORT.y) * 0.05f + 32.f });
    xMarkHUD->AddComponent<TextComp>("X", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    xMarkHUD->GetComponent<TextComp>()->GetText().setCharacterSize(18);
    xMarkHUD->GetComponent<TextComp>()->SetCentered(true);
    xMarkHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(xMarkHUD);
    
    const auto coinImageHud = scene->CreateGameObject("Z_CoinsImageHUD");
    coinImageHud->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.4f - 60, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    coinImageHud->AddComponent<SpriteRenderComponent>("graphics/HUDCoins.png", sf::Vector2i{ 24, 24 }, 6, 0.15f);
    scene->SetGameObjectAsCanvasObject(coinImageHud);

    const auto worldNameHUD = scene->CreateGameObject("Z_worldNameHUD");
    worldNameHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.6f + 40, static_cast<float>(window::VIEWPORT.y) * 0.05f });
    worldNameHUD->AddComponent<TextComp>("WORLD", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    worldNameHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    worldNameHUD->GetComponent<TextComp>()->SetCentered(true);
    worldNameHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(worldNameHUD);

    const auto worldCountHUD = scene->CreateGameObject("Z_worldCountHUD");
    worldCountHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.6f + 40, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    const std::string levelStr = std::format("1-{}", superMarioBros::GameManager::GetInstance().GetPlayerInfo().currentLevel);
    worldCountHUD->AddComponent<TextComp>(levelStr, "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    worldCountHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    worldCountHUD->GetComponent<TextComp>()->SetCentered(true);
    worldCountHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(worldCountHUD);

    const auto timerName = scene->CreateGameObject("Z_timerName");
    timerName->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.85f, static_cast<float>(window::VIEWPORT.y) * 0.05f });
    timerName->AddComponent<TextComp>("TIME", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    timerName->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    timerName->GetComponent<TextComp>()->SetCentered(true);
    timerName->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(timerName);
    
    const auto timerHUD = scene->CreateGameObject("Z_timerHUD");
    timerHUD->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.85f + 12, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f });
    timerHUD->AddComponent<TextComp>("400", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    timerHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    timerHUD->GetComponent<TextComp>()->SetCentered(true);
    timerHUD->AddComponent<ScoreCounter>(400, true);
    timerHUD->GetComponent<ScoreCounter>()->SetString("");
    timerHUD->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true, 3);
    timerHUD->GetComponent<ScoreCounter>()->SetGoalScore(0);
    timerHUD->AddComponent<superMarioBros::TimerScript>();
    timerHUD->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(timerHUD);

    const auto HUDManager = scene->CreateGameObject("Z_HUDManager");
    HUDManager->SetObjectPosition({ 0, 0 });
    HUDManager->AddComponent<superMarioBros::HudManager>
    (
        scoreHUD->GetComponent<ScoreCounter>(),
        coinsCounterHud->GetComponent<ScoreCounter>(),
        timerHUD->GetComponent<ScoreCounter>()
    );

    const auto fpsCounter = scene->CreateGameObject("Z_FPSCounter");
    fpsCounter->AddComponent<TextComp>("0 FPS", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    fpsCounter->GetComponent<TextComp>()->GetText().setCharacterSize(10);
    fpsCounter->AddComponent<FPSCounter>();
    fpsCounter->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x - 100), 40 });
    fpsCounter->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(fpsCounter);
#pragma endregion
    
#pragma region Commands
    auto& input = InputManager::GetInstance();

    input.BindCommand<superMarioBros::MoveCharacter>(PlayerIdx::KEYBOARD, KeyState::HELD, sf::Keyboard::Scancode::D, player, sf::Vector2f{ 1.f, 0.f }, true);
    input.BindCommand<superMarioBros::MoveCharacter>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::D, player, sf::Vector2f{ 1.f, 0.f }, false);
    input.BindCommand<superMarioBros::MoveCharacter>(PlayerIdx::KEYBOARD, KeyState::HELD, sf::Keyboard::Scancode::Right, player, sf::Vector2f{ 1.f, 0.f }, true);
    input.BindCommand<superMarioBros::MoveCharacter>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::Right, player, sf::Vector2f{ 1.f, 0.f }, false);
    input.BindCommand<superMarioBros::MoveCharacter>(PlayerIdx::PLAYER1, KeyState::HELD, Controller::Button::DPadRight, player, sf::Vector2f{ 1.f, 0.f }, true);
    input.BindCommand<superMarioBros::MoveCharacter>(PlayerIdx::PLAYER1, KeyState::RELEASED, Controller::Button::DPadRight, player, sf::Vector2f{ 1.f, 0.f }, false);
    
    input.BindCommand<superMarioBros::MoveCharacter>(PlayerIdx::KEYBOARD, KeyState::HELD, sf::Keyboard::Scancode::A, player, sf::Vector2f{ -1.f, 0.f }, true);
    input.BindCommand<superMarioBros::MoveCharacter>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::A, player, sf::Vector2f{ -1.f, 0.f }, false);
    input.BindCommand<superMarioBros::MoveCharacter>(PlayerIdx::KEYBOARD, KeyState::HELD, sf::Keyboard::Scancode::Left, player, sf::Vector2f{ -1.f, 0.f }, true);
    input.BindCommand<superMarioBros::MoveCharacter>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::Left, player, sf::Vector2f{ -1.f, 0.f }, false);
    input.BindCommand<superMarioBros::MoveCharacter>(PlayerIdx::PLAYER1, KeyState::HELD, Controller::Button::DPadLeft, player, sf::Vector2f{ -1.f, 0.f }, true);
    input.BindCommand<superMarioBros::MoveCharacter>(PlayerIdx::PLAYER1, KeyState::RELEASED, Controller::Button::DPadLeft, player, sf::Vector2f{ -1.f, 0.f }, false);
    
    input.BindCommand<superMarioBros::CharacterJump>(PlayerIdx::KEYBOARD, KeyState::HELD, sf::Keyboard::Scancode::W, player, true);
    input.BindCommand<superMarioBros::CharacterJump>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::W, player, false);
    input.BindCommand<superMarioBros::CharacterJump>(PlayerIdx::KEYBOARD, KeyState::HELD, sf::Keyboard::Scancode::Space, player, true);
    input.BindCommand<superMarioBros::CharacterJump>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::Space, player, false);
    input.BindCommand<superMarioBros::CharacterJump>(PlayerIdx::KEYBOARD, KeyState::HELD, sf::Keyboard::Scancode::Up, player, true);
    input.BindCommand<superMarioBros::CharacterJump>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::Up, player, false);
    input.BindCommand<superMarioBros::CharacterJump>(PlayerIdx::PLAYER1, KeyState::HELD, Controller::Button::DPadUp, player, true);
    input.BindCommand<superMarioBros::CharacterJump>(PlayerIdx::PLAYER1, KeyState::RELEASED, Controller::Button::DPadUp, player, false);
    input.BindCommand<superMarioBros::CharacterJump>(PlayerIdx::PLAYER1, KeyState::HELD, Controller::Button::A, player, true);
    input.BindCommand<superMarioBros::CharacterJump>(PlayerIdx::PLAYER1, KeyState::RELEASED, Controller::Button::A, player, false);
    
    input.BindCommand<superMarioBros::Sprint>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::LShift, player, true);
    input.BindCommand<superMarioBros::Sprint>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::LShift, player, false);
    input.BindCommand<superMarioBros::Sprint>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::X, player, true);
    input.BindCommand<superMarioBros::Sprint>(PlayerIdx::PLAYER1, KeyState::RELEASED, Controller::Button::X, player, false);
    
    input.BindCommand<superMarioBros::Attack>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::LShift, player);
    input.BindCommand<superMarioBros::Attack>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::C, player);
    input.BindCommand<superMarioBros::Attack>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::X, player);
    input.BindCommand<superMarioBros::Attack>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::B, player);

    input.BindCommand<superMarioBros::Pause>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::Enter, nullptr);
    input.BindCommand<superMarioBros::Pause>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::Escape, nullptr);
    input.BindCommand<superMarioBros::Pause>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::Start, nullptr);
    input.BindCommand<superMarioBros::Pause>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::Back, nullptr);

    input.BindCommand<superMarioBros::Crouch>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::S, player, true);
    input.BindCommand<superMarioBros::Crouch>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::S, player, false);
    input.BindCommand<superMarioBros::Crouch>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::Down, player, true);
    input.BindCommand<superMarioBros::Crouch>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::Down, player, false);
    input.BindCommand<superMarioBros::Crouch>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::DPadDown, player, true);
    input.BindCommand<superMarioBros::Crouch>(PlayerIdx::PLAYER1, KeyState::RELEASED, Controller::Button::DPadDown, player, false);
#pragma endregion

#pragma region Events

    superMarioBros::GameManager::GetInstance().OnScoreAddedEvent.AddListener(scoreHUD->GetComponent<ScoreCounter>(), &ScoreCounter::IncreaseScore);
    superMarioBros::GameManager::GetInstance().OnCoinCollectedEvent.AddListener(coinsCounterHud->GetComponent<ScoreCounter>(), &ScoreCounter::IncreaseScore);
    timerHUD->GetComponent<ScoreCounter>()->OnGivenScoreReachedEvent.AddListener(player->GetComponent<superMarioBros::PlayerCharacter>(), &superMarioBros::PlayerCharacter::KillPlayer);
#pragma endregion
}