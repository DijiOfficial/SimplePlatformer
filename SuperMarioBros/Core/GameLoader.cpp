#include "GameLoader.h"
#include "GameState.h"
#include "../Components/Backgrounds/BackgroundHandler.h"
#include "../Components/Backgrounds/CustomBackgroundRenderer.h"
#include "../Components/Player/CameraClamping.h"
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
#include "Engine/Components/Transform.h"
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

#include "../Components/Player/CheckPlayerTopPixel.h"
#include "Engine/Core/Renderer.h"

using namespace diji;

void SceneLoader::GameStartUp()
{
#ifndef NDEBUG
    ServiceLocator::RegisterSoundSystem(std::make_unique<LoggingSoundSystem>(std::make_unique<SFMLISoundSystem>()));
#else
    ServiceLocator::RegisterSoundSystem(std::make_unique<SFMLISoundSystem>());
#endif

    //todo: this would be better if it was part of the CreateScene function.
    SceneManager::GetInstance().RegisterScene(static_cast<int>(superMarioBros::superMarioBrosState::Level), Level);
    SceneManager::GetInstance().RegisterScene(static_cast<int>(superMarioBros::superMarioBrosState::StartMenu), StartMenu);
    SceneManager::GetInstance().RegisterScene(static_cast<int>(superMarioBros::superMarioBrosState::LivesDisplayMenu), LivesDisplayMenu);
    SceneManager::GetInstance().RegisterScene(static_cast<int>(superMarioBros::superMarioBrosState::GameOver), GameOverMenu);
    SceneManager::GetInstance().RegisterScene(static_cast<int>(superMarioBros::superMarioBrosState::TransitionToNextLevel), TransitionToNextLevel);

    // StartMenu();
    Level();
}

#pragma region Menus
void SceneLoader::StartMenu()
{
    SceneManager::GetInstance().SetActiveScene(static_cast<int>(superMarioBros::superMarioBrosState::StartMenu));
    const auto& scene = SceneManager::GetInstance().CreateScene(static_cast<int>(superMarioBros::superMarioBrosState::StartMenu));
    GameStateManager::GetInstance().SetNewGameState(static_cast<GameState>(superMarioBros::superMarioBrosState::StartMenu));
    Renderer::GetInstance().SetBackgroundColor(sf::Color(148, 148, 255));

    const auto staticBackground = scene->CreateGameObject("A_StaticBackground");
    staticBackground->AddComponents<Transform>(1920 * 0.5f, 1080 * 0.5f);
    // staticBackground->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.5f, static_cast<float>(window::VIEWPORT.y) * 0.5f);
    staticBackground->AddComponents<TextureComp>("graphics/menuBackground.png");
    staticBackground->AddComponents<Render>(1.0f);
    scene->SetGameObjectAsStaticBackground(staticBackground);

    const auto camera = scene->CreateCameraObject("A_Camera");
    camera->AddComponents<Transform>(0, 0);
    camera->AddComponents<Camera>(sf::Vector2u{ 1920, 1080 });

    const auto player = scene->CreateGameObject("X_PlayerChar");
    player->AddComponents<Transform>(400, 910);
    player->AddComponents<SpriteRenderComponent>("graphics/player.png", sf::Vector2i{ 16, 16 }, 1, 0.05f);
    player->GetComponent<SpriteRenderComponent>()->SetScale(3);
    player->GetComponent<SpriteRenderComponent>()->Pause();
    
     // Create the HUD
    const auto playerOneText = scene->CreateGameObject("Z_PlayerOneText");
    playerOneText->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.5f, static_cast<float>(window::VIEWPORT.y) * 0.65f);
    playerOneText->AddComponents<TextComp>("PUSH START BUTTON", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    playerOneText->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    playerOneText->GetComponent<TextComp>()->SetCentered(true);
    playerOneText->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(playerOneText);

    const auto highScore = scene->CreateGameObject("Z_HighScore");
    highScore->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.5f, static_cast<float>(window::VIEWPORT.y) * 0.70f);
    highScore->AddComponents<TextComp>("000000", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    highScore->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    highScore->GetComponent<TextComp>()->SetCentered(true);
    highScore->AddComponents<ScoreCounter>(superMarioBros::GameManager::GetInstance().GetHighScoreFromFile(), true);
    highScore->GetComponent<ScoreCounter>()->SetString("TOP-");
    highScore->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true);
    highScore->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(highScore);

    const auto myName = scene->CreateGameObject("Z_Disclaimer");
    myName->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.5f, static_cast<float>(window::VIEWPORT.y) * 0.80f);
    myName->AddComponents<TextComp>("2025 BURGISSER DYLAN\n", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    myName->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    myName->GetComponent<TextComp>()->SetCentered(true);
    myName->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(myName);
    
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
    coinsCounterHud->GetComponent<ScoreCounter>()->SetGoalScore(100);
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
    
    const auto fpsCounter = scene->CreateGameObject("Z_FPSCounter");
    fpsCounter->AddComponents<TextComp>("0 FPS", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    fpsCounter->GetComponent<TextComp>()->GetText().setCharacterSize(10);
    fpsCounter->AddComponents<FPSCounter>();
    fpsCounter->AddComponents<Transform>(window::VIEWPORT.x - 100, 40);
    fpsCounter->AddComponents<Render>();
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

    (void)TimerManager::GetInstance().SetTimer([]
    {
        superMarioBros::GameManager::GetInstance().SwitchToNextScene();
    }, 2.6f, false);
    
    const auto camera = scene->CreateCameraObject("A_Camera");
    camera->AddComponents<Transform>(0, 0);
    camera->AddComponents<Camera>(sf::Vector2f{ 1920.f, 1080.f });
#pragma region HUD
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
    const int score = superMarioBros::GameManager::GetInstance().GetPlayerInfo().totalScore;
    scoreHUD->AddComponents<ScoreCounter>(score, true);
    scoreHUD->GetComponent<ScoreCounter>()->SetString("");
    scoreHUD->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true);
    scoreHUD->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(scoreHUD);

    const auto coinsCounterHud = scene->CreateGameObject("Z_CoinsCounterHUD");
    coinsCounterHud->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.4f, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f);
    coinsCounterHud->AddComponents<TextComp>("00", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    coinsCounterHud->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    coinsCounterHud->GetComponent<TextComp>()->SetCentered(true);
    const int coins = superMarioBros::GameManager::GetInstance().GetPlayerInfo().totalCoins;
    coinsCounterHud->AddComponents<ScoreCounter>(coins, true);
    coinsCounterHud->GetComponent<ScoreCounter>()->SetString("");
    coinsCounterHud->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true, 2);
    coinsCounterHud->GetComponent<ScoreCounter>()->SetGoalScore(100);
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
    const std::string levelStr = std::format("1-{}", superMarioBros::GameManager::GetInstance().GetPlayerInfo().currentLevel);
    worldCountHUD->AddComponents<TextComp>(levelStr, "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    worldCountHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    worldCountHUD->GetComponent<TextComp>()->SetCentered(true);
    worldCountHUD->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(worldCountHUD);

    const auto worldCountCentered = scene->CreateGameObject("Z_worldCountCentered");
    worldCountCentered->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.5f, static_cast<float>(window::VIEWPORT.y) * 0.4f);
    const std::string levelStrCenter = std::format("WORLD 1-{}", superMarioBros::GameManager::GetInstance().GetPlayerInfo().currentLevel);
    worldCountCentered->AddComponents<TextComp>(levelStrCenter, "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    worldCountCentered->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    worldCountCentered->GetComponent<TextComp>()->SetCentered(true);
    worldCountCentered->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(worldCountCentered);

    const auto livesDisplay = scene->CreateGameObject("Z_livesDisplay");
    livesDisplay->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.49f, static_cast<float>(window::VIEWPORT.y) * 0.5f);
    const std::string livesStr = std::format("   x  {}", superMarioBros::GameManager::GetInstance().GetPlayerInfo().totalLives);
    livesDisplay->AddComponents<TextComp>(livesStr, "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    livesDisplay->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    livesDisplay->GetComponent<TextComp>()->SetCentered(true);
    livesDisplay->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(livesDisplay);

    const auto player = scene->CreateGameObject("X_PlayerChar");
    player->AddComponents<Transform>(965.f, static_cast<float>(window::VIEWPORT.y) * 0.5f);
    player->AddComponents<SpriteRenderComponent>("graphics/player.png", sf::Vector2i{ 16, 16 }, 1, 0.05f);
    player->GetComponent<SpriteRenderComponent>()->SetScale(3);
    player->GetComponent<SpriteRenderComponent>()->Pause();

    const auto timerName = scene->CreateGameObject("Z_timerName");
    timerName->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.85f, static_cast<float>(window::VIEWPORT.y) * 0.05f);
    timerName->AddComponents<TextComp>("TIME", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    timerName->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    timerName->GetComponent<TextComp>()->SetCentered(true);
    timerName->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(timerName);

    const auto fpsCounter = scene->CreateGameObject("Z_FPSCounter");
    fpsCounter->AddComponents<TextComp>("0 FPS", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    fpsCounter->GetComponent<TextComp>()->GetText().setCharacterSize(10);
    fpsCounter->AddComponents<FPSCounter>();
    fpsCounter->AddComponents<Transform>(window::VIEWPORT.x - 100, 40);
    fpsCounter->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(fpsCounter);
#pragma endregion
}

void SceneLoader::GameOverMenu()
{
    ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_gameover.wav", false);
    
    SceneManager::GetInstance().SetActiveScene(static_cast<int>(superMarioBros::superMarioBrosState::GameOver));
    const auto& scene = SceneManager::GetInstance().CreateScene(static_cast<int>(superMarioBros::superMarioBrosState::GameOver));
    GameStateManager::GetInstance().SetNewGameState(static_cast<GameState>(superMarioBros::superMarioBrosState::GameOver));
    Renderer::GetInstance().SetBackgroundColor(sf::Color::Black);

    superMarioBros::GameManager::GetInstance().SaveHighScoreToFile();
    
    (void)TimerManager::GetInstance().SetTimer([]
    {
        superMarioBros::GameManager::GetInstance().ResetPlayerInfo();
        SceneManager::GetInstance().SetNextSceneToActivate(static_cast<int>(superMarioBros::superMarioBrosState::StartMenu));
    }, 7.f, false);
    
    const auto camera = scene->CreateCameraObject("A_Camera");
    camera->AddComponents<Transform>(0, 0);
    camera->AddComponents<Camera>(sf::Vector2f{ 1920.f, 1080.f });

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
    const int score = superMarioBros::GameManager::GetInstance().GetPlayerInfo().totalScore;
    scoreHUD->AddComponents<ScoreCounter>(score, true);
    scoreHUD->GetComponent<ScoreCounter>()->SetString("");
    scoreHUD->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true);
    scoreHUD->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(scoreHUD);

    const auto coinsCounterHud = scene->CreateGameObject("Z_CoinsCounterHUD");
    coinsCounterHud->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.4f, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f);
    coinsCounterHud->AddComponents<TextComp>("00", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    coinsCounterHud->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    coinsCounterHud->GetComponent<TextComp>()->SetCentered(true);
    const int coins = superMarioBros::GameManager::GetInstance().GetPlayerInfo().totalCoins;
    coinsCounterHud->AddComponents<ScoreCounter>(coins, true);
    coinsCounterHud->GetComponent<ScoreCounter>()->SetString("");
    coinsCounterHud->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true, 2);
    coinsCounterHud->GetComponent<ScoreCounter>()->SetGoalScore(100);
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
    const std::string levelStr = std::format("1-{}", superMarioBros::GameManager::GetInstance().GetPlayerInfo().currentLevel);
    worldCountHUD->AddComponents<TextComp>(levelStr, "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    worldCountHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    worldCountHUD->GetComponent<TextComp>()->SetCentered(true);
    worldCountHUD->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(worldCountHUD);

    const auto gameOver = scene->CreateGameObject("Z_gameOver");
    gameOver->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.5f, static_cast<float>(window::VIEWPORT.y) * 0.5f);
    gameOver->AddComponents<TextComp>("GAME OVER", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    gameOver->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    gameOver->GetComponent<TextComp>()->SetCentered(true);
    gameOver->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(gameOver);

    const auto timerName = scene->CreateGameObject("Z_timerName");
    timerName->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.85f, static_cast<float>(window::VIEWPORT.y) * 0.05f);
    timerName->AddComponents<TextComp>("TIME", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    timerName->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    timerName->GetComponent<TextComp>()->SetCentered(true);
    timerName->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(timerName);

    const auto fpsCounter = scene->CreateGameObject("Z_FPSCounter");
    fpsCounter->AddComponents<TextComp>("0 FPS", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    fpsCounter->GetComponent<TextComp>()->GetText().setCharacterSize(10);
    fpsCounter->AddComponents<FPSCounter>();
    fpsCounter->AddComponents<Transform>(window::VIEWPORT.x - 100, 40);
    fpsCounter->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(fpsCounter);
}

void SceneLoader::TransitionToNextLevel()
{
    ServiceLocator::GetSoundSystem().AddSoundRequest("sound/Transition.mp3", false);

    SceneManager::GetInstance().SetActiveScene(static_cast<int>(superMarioBros::superMarioBrosState::TransitionToNextLevel));
    const auto& scene = SceneManager::GetInstance().CreateScene(static_cast<int>(superMarioBros::superMarioBrosState::TransitionToNextLevel));
    GameStateManager::GetInstance().SetNewGameState(static_cast<GameState>(superMarioBros::superMarioBrosState::TransitionToNextLevel));
    Renderer::GetInstance().SetBackgroundColor(sf::Color(92, 148, 252));

    (void)TimerManager::GetInstance().SetTimer([]
    {
        superMarioBros::GameManager::GetInstance().SwitchToNextScene();
    }, 5.6f, false);
    
    const sf::FloatRect arena{ sf::Vector2f{ 0, -(115 * 4.5) }, sf::Vector2f{ 1920.f, 1080.f } };
    const auto camera = scene->CreateCameraObject("A_Camera");
    camera->AddComponents<Transform>(0, 0);
    camera->AddComponents<Camera>(sf::Vector2f{ 1920.f, 1080.f });
    camera->GetComponent<Camera>()->SetLevelBoundaries(arena);

    const auto staticBackground = scene->CreateGameObject("A_StaticBackground");
    staticBackground->AddComponents<Transform>(0, 178);
    staticBackground->AddComponents<TextureComp>("graphics/background.png");
    staticBackground->AddComponents<superMarioBros::CustomBackgroundRenderer>();

    const auto background = scene->CreateGameObject("B_Background");
    background->AddComponents<Transform>(0, 0);
    background->AddComponents<Sprite>("graphics/tiles_sheet.png");
    background->AddComponents<Render>();
    background->AddComponents<superMarioBros::BackgroundHandler>();
    
    const auto player = scene->CreateGameObject("X_PlayerChar");
    player->AddComponents<Transform>(200, 475);
    player->AddComponents<SpriteRenderComponent>("graphics/player.png", sf::Vector2i{ 16, 16 }, 3, 0.05f);
    player->GetComponent<SpriteRenderComponent>()->SetScale(3);
    player->AddComponents<Collider>(CollisionShape::ShapeType::RECT, sf::Vector2f{ 48, 48 });
    const auto collider = player->GetComponent<Collider>();
    collider->SetCollisionResponse(Collider::CollisionResponse::Ignore);
    collider->SetActive(false);
    collider->SetAffectedByGravity(false);
    player->AddComponents<superMarioBros::PlayerCharacter>(0.5f);
    player->GetComponent<superMarioBros::PlayerCharacter>()->PausePlayer();
    player->AddComponents<superMarioBros::MenuTransition>();
    
#pragma region HUD
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
    const int score = superMarioBros::GameManager::GetInstance().GetPlayerInfo().totalScore;
    scoreHUD->AddComponents<ScoreCounter>(score, true);
    scoreHUD->GetComponent<ScoreCounter>()->SetString("");
    scoreHUD->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true);
    scoreHUD->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(scoreHUD);

    const auto coinsCounterHud = scene->CreateGameObject("Z_CoinsCounterHUD");
    coinsCounterHud->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.4f, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f);
    coinsCounterHud->AddComponents<TextComp>("00", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    coinsCounterHud->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    coinsCounterHud->GetComponent<TextComp>()->SetCentered(true);
    const int coins = superMarioBros::GameManager::GetInstance().GetPlayerInfo().totalCoins;
    coinsCounterHud->AddComponents<ScoreCounter>(coins, true);
    coinsCounterHud->GetComponent<ScoreCounter>()->SetString("");
    coinsCounterHud->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true, 2);
    coinsCounterHud->GetComponent<ScoreCounter>()->SetGoalScore(100);
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
    const std::string levelStr = std::format("1-{}", superMarioBros::GameManager::GetInstance().GetPlayerInfo().currentLevel);
    worldCountHUD->AddComponents<TextComp>(levelStr, "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
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

    const auto fpsCounter = scene->CreateGameObject("Z_FPSCounter");
    fpsCounter->AddComponents<TextComp>("0 FPS", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    fpsCounter->GetComponent<TextComp>()->GetText().setCharacterSize(10);
    fpsCounter->AddComponents<FPSCounter>();
    fpsCounter->AddComponents<Transform>(window::VIEWPORT.x - 100, 40);
    fpsCounter->AddComponents<Render>();
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

    const auto staticBackground = scene->CreateGameObject("A_StaticBackground");
    staticBackground->AddComponents<Transform>(0, 178);
    staticBackground->AddComponents<TextureComp>("graphics/background.png");
    staticBackground->AddComponents<superMarioBros::CustomBackgroundRenderer>();

    const auto background = scene->CreateGameObject("B_Background");
    background->AddComponents<Transform>(0, 0);
    background->AddComponents<Sprite>("graphics/tiles_sheet.png");
    background->AddComponents<Render>();
    background->AddComponents<superMarioBros::BackgroundHandler>();

    const sf::FloatRect arena{ sf::Vector2f{ 0, -(115 * 4.5) }, sf::Vector2f{ 12000.f, 1080.f } };
    const auto camera = scene->CreateCameraObject("A_Camera");
    camera->AddComponents<Transform>(0, 0);
    camera->AddComponents<Camera>(sf::Vector2f{ 1920.f, 1080.f });
    camera->GetComponent<Camera>()->SetLevelBoundaries(arena);

    const auto player = scene->CreateGameObject("X_PlayerChar");
    player->AddComponents<Transform>(200, 0);
    player->AddComponents<SpriteRenderComponent>("graphics/player.png", sf::Vector2i{ 16, 16 }, 3, 0.05f);
    player->GetComponent<SpriteRenderComponent>()->SetScale(3);
    player->AddComponents<Collider>(CollisionShape::ShapeType::RECT, sf::Vector2f{ 48, 48 });
    player->GetComponent<Collider>()->SetRestitution(0.f);
    player->GetComponent<Collider>()->SetMass(0.89f);
    player->GetComponent<Collider>()->SetStaticFriction(0.25f);
    player->GetComponent<Collider>()->SetKineticFriction(0.15f);
    player->GetComponent<Collider>()->SetGenerateHitEvents(true);
    player->GetComponent<Collider>()->SetIsOnlyApplyingGroundFriction(true);
    player->GetComponent<Collider>()->SetMaxVelocity(sf::Vector2f{ 800.f, 1000.f });
    player->GetComponent<Collider>()->SetTag("player");
    player->AddComponents<superMarioBros::PlayerCharacter>(0.5f);
    player->AddComponents<superMarioBros::CameraClamping>();
    player->AddComponents<superMarioBros::BroadcastPlayerPosition>();
    player->AddComponents<superMarioBros::CheckPlayerTopPixel>();

    SceneManager::GetInstance().GetPhysicsWorld()->SetGravity(sf::Vector2f{ 0, 980 * 3.f });

    // const auto testObject = scene->CreateGameObject("Y_testObject");
    // // auto testObject = std::make_unique<diji::GameObject>();
    // testObject->AddComponents<Transform>(6000, 400);
    // testObject->AddComponents<SpriteRenderComponent>("graphics/piranhaPlant.png", sf::Vector2i{ 50, 75 }, 2, 0.135f);
    // testObject->AddComponents<Collider>(CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 75 });
    // const auto collider = testObject->GetComponent<Collider>();
    // collider->SetIsMoveable(false);
    // collider->SetTag("plant");
    // collider->SetAffectedByGravity(false);
    // collider->SetCollisionResponse(Collider::CollisionResponse::Overlap);
    // testObject->AddComponents<superMarioBros::PiranhaPlant>();
 
    
    // const auto testObject = scene->CreateGameObject("Y_testObject");
    // testObject->AddComponents<Transform>(11000, 250);
    // testObject->AddComponents<SpriteRenderComponent>("graphics/explosion.png", sf::Vector2i{ 50, 50 }, 3, 0.135f);
    // testObject->AddComponents<AutoDestroy>(0.405f);

    // auto breakableBlock = scene->CreateGameObject("Y_testObject");
    // // auto breakableBlock = std::make_unique<diji::GameObject>();
    // breakableBlock->AddComponents<diji::Transform>(600, 300);
    // breakableBlock->AddComponents<diji::SpriteRenderComponent>("graphics/breakableBlock.png", sf::Vector2i{ 50, 50 }, 1, 0.0f);
    // breakableBlock->GetComponent<diji::SpriteRenderComponent>()->SetLooping(false);
    // breakableBlock->AddComponents<diji::Collider>(diji::CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
    // const auto collider = breakableBlock->GetComponent<diji::Collider>();
    // collider->SetTag("breakBlock");
    // collider->SetAffectedByGravity(false);
    // collider->SetGenerateHitEvents(true);
    // collider->SetIsMoveable(false);
    // breakableBlock->AddComponents<superMarioBros::BreakableBlock>();

#pragma region HUD
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
    const int score = superMarioBros::GameManager::GetInstance().GetPlayerInfo().totalScore;
    scoreHUD->AddComponents<ScoreCounter>(0, true);
    scoreHUD->GetComponent<ScoreCounter>()->SetString("");
    scoreHUD->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true);
    scoreHUD->GetComponent<ScoreCounter>()->SetScore(score);
    scoreHUD->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(scoreHUD);

    const auto coinsCounterHud = scene->CreateGameObject("Z_CoinsCounterHUD");
    coinsCounterHud->AddComponents<Transform>(static_cast<float>(window::VIEWPORT.x) * 0.4f, static_cast<float>(window::VIEWPORT.y) * 0.05f + 30.f);
    coinsCounterHud->AddComponents<TextComp>("00", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    coinsCounterHud->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    coinsCounterHud->GetComponent<TextComp>()->SetCentered(true);
    const int coins = superMarioBros::GameManager::GetInstance().GetPlayerInfo().totalCoins;
    coinsCounterHud->AddComponents<ScoreCounter>(coins, true);
    coinsCounterHud->GetComponent<ScoreCounter>()->SetString("");
    coinsCounterHud->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true, 2);
    coinsCounterHud->GetComponent<ScoreCounter>()->SetGoalScore(100);
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
    const std::string levelStr = std::format("1-{}", superMarioBros::GameManager::GetInstance().GetPlayerInfo().currentLevel);
    worldCountHUD->AddComponents<TextComp>(levelStr, "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
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
    timerHUD->AddComponents<TextComp>("400", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    timerHUD->GetComponent<TextComp>()->GetText().setCharacterSize(25);
    timerHUD->GetComponent<TextComp>()->SetCentered(true);
    timerHUD->AddComponents<ScoreCounter>(400, true);
    timerHUD->GetComponent<ScoreCounter>()->SetString("");
    timerHUD->GetComponent<ScoreCounter>()->SetUsingZeroPadding(true, 3);
    timerHUD->GetComponent<ScoreCounter>()->SetGoalScore(0);
    timerHUD->AddComponents<superMarioBros::TimerScript>();
    timerHUD->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(timerHUD);

    const auto HUDManager = scene->CreateGameObject("Z_HUDManager");
    HUDManager->AddComponents<Transform>(0, 0);
    HUDManager->AddComponents<superMarioBros::HudManager>
    (
        scoreHUD->GetComponent<ScoreCounter>(),
        coinsCounterHud->GetComponent<ScoreCounter>(),
        timerHUD->GetComponent<ScoreCounter>()
    );

    const auto fpsCounter = scene->CreateGameObject("Z_FPSCounter");
    fpsCounter->AddComponents<TextComp>("0 FPS", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    fpsCounter->GetComponent<TextComp>()->GetText().setCharacterSize(10);
    fpsCounter->AddComponents<FPSCounter>();
    fpsCounter->AddComponents<Transform>(window::VIEWPORT.x - 100, 40);
    fpsCounter->AddComponents<Render>();
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