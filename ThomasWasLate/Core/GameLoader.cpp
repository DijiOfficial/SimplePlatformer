#include "GameLoader.h"

#include "GameState.h"
#include "../Components/BackgroundHandler.h"
#include "../Components/CustomBackgroundRenderer.h"
#include "../Components/GoombaAI.h"
#include "../Components/PlayerCharacter.h"
#include "../Components/tempTest.h"
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
    player->GetComponent<Collider>()->SetMaxVelocity(sf::Vector2f{ 400.f, 800.f });
    // player->GetComponent<Collider>()->SetAffectedByGravity(false);
    player->AddComponents<thomasWasLate::PlayerCharacter>(0.45f);
    // player->AddComponents<ShapeRender>(true);

    SceneManager::GetInstance().GetPhysicsWorld()->SetGravity(sf::Vector2f{ 0, 980 * 3.f });

    const auto goombaTest = scene->CreateGameObject("Y_GoombaTest");
    goombaTest->AddComponents<Transform>(1200, 0);
    goombaTest->AddComponents<SpriteRenderComponent>("graphics/goomba.png", sf::Vector2i{ 50,50 }, 2, 0.15f);
    goombaTest->AddComponents<Collider>(CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 50 });
    goombaTest->GetComponent<Collider>()->SetRestitution(0.f);
    goombaTest->GetComponent<Collider>()->SetMass(0.89f);
    goombaTest->GetComponent<Collider>()->SetFriction(0.25f);
    goombaTest->GetComponent<Collider>()->SetMaxVelocity(sf::Vector2f{ 400.f, 800.f });
    goombaTest->AddComponents<thomasWasLate::GoombaAI>();
    
    
    const auto fpsCounter = scene->CreateGameObject("Z_FPSCounter");
    fpsCounter->AddComponents<TextComp>("0 FPS", "fonts/Roboto-Light.ttf", sf::Color::White, true);
    fpsCounter->GetComponent<TextComp>()->GetText().setCharacterSize(33);
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

    // zombiesRemainingText->GetComponent<ScoreCounter>()->OnGivenScoreReachedEvent.AddListener(pistol->GetComponent<zombieArena::Pistol>(), &zombieArena::Pistol::SaveInfo);
    
#pragma endregion
}