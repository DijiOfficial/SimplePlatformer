#include "../GameLoader.h"
#include "../GameState.h"
#include "../../Components/Backgrounds/CustomBackgroundRenderer.h"
#include "../../Components/LevelEditor/Selector.h"
#include "../../Components/Player/CameraClamping.h"
#include "../../Input/LevelEditorCommands.h"
#include "Engine/Components/TextureComp.h"
#include "Engine/Core/Renderer.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Components/Camera.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Singleton/GameStateManager.h"
#include "Engine/Collision/Collider.h"

using namespace diji;

void SceneLoader::LevelEditor()
{
    // ServiceLocator::GetSoundSystem().AddSoundRequest("sound/LevelMusic.mp3", true);

    SceneManager::GetInstance().SetActiveScene(static_cast<int>(superMarioBros::superMarioBrosState::LevelEditor));
    const auto& scene = SceneManager::GetInstance().CreateScene(static_cast<int>(superMarioBros::superMarioBrosState::LevelEditor));
    GameStateManager::GetInstance().SetNewGameState(static_cast<GameState>(superMarioBros::superMarioBrosState::LevelEditor));
    Renderer::GetInstance().SetBackgroundColor(sf::Color(92, 148, 252));

    const auto staticBackground = scene->CreateGameObject("A_StaticBackground");
    staticBackground->SetObjectPosition({ 0, 178 });
    staticBackground->AddComponent<TextureComp>("graphics/background.png");
    staticBackground->AddComponent<superMarioBros::CustomBackgroundRenderer>();

    constexpr sf::FloatRect arena{ sf::Vector2f{ 0, -(115 * 4.5) }, sf::Vector2f{ 12000.f, 1080.f } };
    const auto selector = scene->CreateGameObject("X_PlayerChar");
    selector->SetObjectPosition({ 525, 25 });
    selector->AddComponent<TextureComp>("graphics/squareWhiteSmaller50.png");
    selector->AddComponent<Render>();
    selector->AddComponent<superMarioBros::Selector>();
    selector->AddComponent<Camera>(sf::Vector2f{ 1920.f, 1080.f });
    selector->GetComponent<Camera>()->SetLevelBoundaries(arena);

    // selector->AddComponent<Collider>(CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 450 });
    // const auto collider = selector->GetComponent<Collider>();
    // collider->SetTag("player");
    // collider->SetCollisionResponse(Collider::CollisionResponse::Ignore);
    // collider->SetAffectedByGravity(false);

    // const auto background = scene->CreateGameObject("B_Background");
    // background->SetObjectPosition({ 0, 0 });
    // background->AddComponent<Sprite>("graphics/tiles_sheet.png");
    // background->AddComponent<Render>();
    // background->AddComponent<superMarioBros::BackgroundHandler>();
    //
    // const sf::FloatRect arena{ sf::Vector2f{ 0, -(115 * 4.5) }, sf::Vector2f{ 12000.f, 1080.f } };
    // const auto camera = scene->CreateCameraObject("A_Camera");
    // camera->SetObjectPosition({ 0, 0 });
    // camera->AddComponent<Camera>(sf::Vector2f{ 1920.f, 1080.f });
    // camera->GetComponent<Camera>()->SetLevelBoundaries(arena);
    //
    // const auto player = scene->CreateGameObject("X_PlayerChar");
    // player->SetObjectPosition({ 200, 0 });
    // player->AddComponent<SpriteRenderComponent>("graphics/player.png", sf::Vector2i{ 16, 16 }, 3, 0.05f);
    // player->GetComponent<SpriteRenderComponent>()->SetScale(3);
    // player->AddComponent<Collider>(CollisionShape::ShapeType::RECT, sf::Vector2f{ 48, 48 });
    // player->GetComponent<Collider>()->SetRestitution(0.f);
    // player->GetComponent<Collider>()->SetMass(0.89f);
    // player->GetComponent<Collider>()->SetStaticFriction(0.25f);
    // player->GetComponent<Collider>()->SetKineticFriction(0.15f);
    // player->GetComponent<Collider>()->SetGenerateHitEvents(true);
    // player->GetComponent<Collider>()->SetIsOnlyApplyingGroundFriction(true);
    // player->GetComponent<Collider>()->SetMaxVelocity(sf::Vector2f{ 800.f, 1000.f });
    // player->GetComponent<Collider>()->SetTag("player");
    // player->AddComponent<superMarioBros::PlayerCharacter>();
    // player->AddComponent<superMarioBros::PlayerInputManager>(0.5f);
    // player->AddComponent<superMarioBros::CameraClamping>();
    // player->AddComponent<superMarioBros::BroadcastPlayerPosition>();
    // player->AddComponent<superMarioBros::CheckPlayerTopPixel>();
    // player->AddComponent<ShapeRender>(true);
    //
    // SceneManager::GetInstance().GetPhysicsWorld()->SetGravity(sf::Vector2f{ 0, 980 * 3.f });

#pragma region HUD
    // Create the HUD
    const auto firstItem = scene->CreateGameObject("Z_UI_Item1");
    firstItem->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.15f, static_cast<float>(window::VIEWPORT.y) * 0.95f });
    firstItem->AddComponent<TextureComp>("graphics/level_editor_item1.png");
    firstItem->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(firstItem);
#pragma endregion
    
#pragma region Commands
    auto& input = InputManager::GetInstance();
    
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::D, selector, sf::Vector2f{ 1.f, 0.f }, true);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::Right, selector, sf::Vector2f{ 1.f, 0.f }, true);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::DPadRight, selector, sf::Vector2f{ 1.f, 0.f }, true);
    
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::A, selector, sf::Vector2f{ -1.f, 0.f }, true);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::Left, selector, sf::Vector2f{ -1.f, 0.f }, true);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::DPadLeft, selector, sf::Vector2f{ -1.f, 0.f }, true);
    
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::W, selector, sf::Vector2f{ 0.f, -1.f }, true);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::Up, selector, sf::Vector2f{ 0.f, -1.f }, true);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::DPadUp, selector, sf::Vector2f{ 0.f, -1.f }, true);
    
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::S, selector, sf::Vector2f{ 0.f, 1.f }, true);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::Down, selector, sf::Vector2f{ 0.f, 1.f }, true);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::DPadDown, selector, sf::Vector2f{ 0.f, 1.f }, true);

    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::D, selector, sf::Vector2f{ 1.f, 0.f }, false);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::Right, selector, sf::Vector2f{ 1.f, 0.f }, false);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::PLAYER1, KeyState::RELEASED, Controller::Button::DPadRight, selector, sf::Vector2f{ 1.f, 0.f }, false);
    
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::A, selector, sf::Vector2f{ -1.f, 0.f }, false);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::Left, selector, sf::Vector2f{ -1.f, 0.f }, false);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::PLAYER1, KeyState::RELEASED, Controller::Button::DPadLeft, selector, sf::Vector2f{ -1.f, 0.f }, false);
    
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::W, selector, sf::Vector2f{ 0.f, -1.f }, false);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::Up, selector, sf::Vector2f{ 0.f, -1.f }, false);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::PLAYER1, KeyState::RELEASED, Controller::Button::DPadUp, selector, sf::Vector2f{ 0.f, -1.f }, false);
    
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::S, selector, sf::Vector2f{ 0.f, 1.f }, false);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::Down, selector, sf::Vector2f{ 0.f, 1.f }, false);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::PLAYER1, KeyState::RELEASED, Controller::Button::DPadDown, selector, sf::Vector2f{ 0.f, 1.f }, false);
#pragma endregion

#pragma region Events

    // superMarioBros::GameManager::GetInstance().OnScoreAddedEvent.AddListener(scoreHUD->GetComponent<ScoreCounter>(), &ScoreCounter::IncreaseScore);
    // superMarioBros::GameManager::GetInstance().OnCoinCollectedEvent.AddListener(coinsCounterHud->GetComponent<ScoreCounter>(), &ScoreCounter::IncreaseScore);
    // timerHUD->GetComponent<ScoreCounter>()->OnGivenScoreReachedEvent.AddListener(player->GetComponent<superMarioBros::PlayerCharacter>(), &superMarioBros::PlayerCharacter::KillPlayer);
#pragma endregion
}
