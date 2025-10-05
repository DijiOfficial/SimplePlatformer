#include "GameLoader.h"

#include "GameState.h"
#include "../Components/BackgroundHandler.h"
#include "../Components/PlayerCharacter.h"
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
    staticBackground->AddComponents<Transform>(0, 0);
    staticBackground->AddComponents<TextureComp>("graphics/background.png");
    staticBackground->AddComponents<Render>();
    scene->SetGameObjectAsStaticBackground(staticBackground);

    const auto background = scene->CreateGameObject("B_Background");
    background->AddComponents<Transform>(0, 0);
    background->AddComponents<Sprite>("graphics/tiles_sheet.png");
    background->AddComponents<Render>();
    background->AddComponents<thomasWasLate::BackgroundHandler>();
    
    const auto camera = scene->CreateCameraObject("A_Camera");
    camera->AddComponents<Transform>(0, 0);
    camera->AddComponents<Camera>(window::VIEWPORT);
    // camera->GetComponent<Camera>()->SetLevelBoundaries(static_cast<sf::FloatRect>(arena));
    camera->GetComponent<Camera>()->SetClampingMode(false);

    // const auto thomas = scene->CreateGameObject("X_Thomas");
    // thomas->AddComponents<Transform>(0, 0);
    // thomas->AddComponents<TextureComp>("graphics/thomas.png");
    // thomas->AddComponents<Render>();
    // // thomas->AddComponents<Collider>(44, 70, sf::Vector2f{ 3.f, 30.f });
    // thomas->AddComponents<thomasWasLate::PlayerCharacter>(0.45f);
    // // thomas->AddComponents<RectRender>(true);

    const auto testBall = scene->CreateGameObject("X_TestBall");
    testBall->AddComponents<Transform>(0, 0);
    testBall->AddComponents<TextureComp>("graphics/thomas.png");
    testBall->AddComponents<Render>();
    testBall->AddComponents<Collider>(CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 100 });
    testBall->GetComponent<Collider>()->SetRestitution(0.5f);
    // testBall->AddComponents<thomasWasLate::PlayerCharacter>(0.45f);
    testBall->AddComponents<ShapeRender>(true);

    // const auto testBall = scene->CreateGameObject("X_TestBall");
    // testBall->AddComponents<Transform>(0, 0);
    // testBall->AddComponents<TextureComp>("graphics/ball.png");
    // testBall->AddComponents<Render>();
    // testBall->AddComponents<Collider>(CollisionShape::ShapeType::CIRCLE, 65.f);
    // testBall->GetComponent<Collider>()->SetRestitution(0.35f);
    // // testBall->GetComponent<Collider>()->SetRestitution(0.5f);
    // // testBall->AddComponents<thomasWasLate::PlayerCharacter>(0.45f);
    // testBall->AddComponents<ShapeRender>(true);
    
    const auto tempBoundBottom = scene->CreateGameObject("Z_TempBoundBottom");
    tempBoundBottom->AddComponents<Transform>(-959, 489);
    tempBoundBottom->AddComponents<Collider>(CollisionShape::ShapeType::RECT, sf::Vector2f{ 1918, 50 });
    tempBoundBottom->GetComponent<Collider>()->SetStatic(true);
    tempBoundBottom->AddComponents<ShapeRender>();

    const auto tempTopBound = scene->CreateGameObject("Z_TempBoundTop");
    tempTopBound->AddComponents<Transform>(-959, -539);
    tempTopBound->AddComponents<Collider>(CollisionShape::ShapeType::RECT, sf::Vector2f{ 1918, 50 });
    tempTopBound->GetComponent<Collider>()->SetStatic(true);
    tempTopBound->AddComponents<ShapeRender>();

    const auto tempBoundLeft = scene->CreateGameObject("Z_TempBoundLeft");
    tempBoundLeft->AddComponents<Transform>(-959, -539);
    tempBoundLeft->AddComponents<Collider>(CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 1078 });
    tempBoundLeft->GetComponent<Collider>()->SetStatic(true);
    tempBoundLeft->AddComponents<ShapeRender>();

    const auto tempBoundRight = scene->CreateGameObject("Z_TempBoundRight");
    tempBoundRight->AddComponents<Transform>(909, -539);
    tempBoundRight->AddComponents<Collider>(CollisionShape::ShapeType::RECT, sf::Vector2f{ 50, 1078 });
    tempBoundRight->GetComponent<Collider>()->SetStatic(true);
    tempBoundRight->AddComponents<ShapeRender>();

    
    // const auto randomCircle = scene->CreateGameObject("Z_TemRoundCirlRight");
    // randomCircle->AddComponents<Transform>(0, 339);
    // randomCircle->AddComponents<Collider>(CollisionShape::ShapeType::CIRCLE, 100.f);
    // randomCircle->GetComponent<Collider>()->SetStatic(true);
    // randomCircle->AddComponents<ShapeRender>();
    
    const auto fpsCounter = scene->CreateGameObject("Z_FPSCounter");
    fpsCounter->AddComponents<TextComp>("0 FPS", "fonts/Roboto-Light.ttf", sf::Color::White, true);
    fpsCounter->GetComponent<TextComp>()->GetText().setCharacterSize(33);
    fpsCounter->AddComponents<FPSCounter>();
    fpsCounter->AddComponents<Transform>(window::VIEWPORT.x - 100, 40);
    fpsCounter->AddComponents<Render>();
    scene->SetGameObjectAsCanvasObject(fpsCounter);
    

    // const auto center = m_Camera->GetCameraView().getCenter();
    // const auto size = m_Camera->GetCameraView().getSize();
    // const auto col = sf::FloatRect{ m_TransformCompPtr->GetPosition(), sf::Vector2f{ 44, 70 } };
    // // const auto col = m_ColliderCompPtr->GetAABB();
    // // const auto offset = m_ColliderCompPtr->GetOffset();
    // const auto offset = sf::Vector2f{ 0.f, 0.f };
    // // bool bounced = false;
    //
    // // Calculate bounds
    // const float leftBound = center.x - size.x * 0.5f;
    // const float rightBound = center.x + size.x * 0.5f - col.width;
    // const float topBound = center.y - size.y * 0.5f;
    // const float bottomBound = center.y + size.y * 0.5f - col.height - offset.y;

    // const auto bob = scene->CreateGameObject("X_Thomas2", thomas);
    // bob->GetComponent<thomasWasLate::PlayerCharacter>()->SetMass(25);
    
#pragma region Commands
    auto& input = InputManager::GetInstance();

    // input.BindCommand<thomasWasLate::MoveCharacter>(PlayerIdx::KEYBOARD, KeyState::HELD, sf::Keyboard::Key::D, thomas, sf::Vector2f{ 1.f, 0.f });
    // input.BindCommand<thomasWasLate::MoveCharacter>(PlayerIdx::KEYBOARD, KeyState::HELD, sf::Keyboard::Key::A, thomas, sf::Vector2f{ -1.f, 0.f });
    //
    // input.BindCommand<thomasWasLate::CharacterJump>(PlayerIdx::KEYBOARD, KeyState::HELD, sf::Keyboard::Key::W, thomas, true);
    // input.BindCommand<thomasWasLate::CharacterJump>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Key::W, thomas, false);

    input.BindCommand<thomasWasLate::TempAddImpulse>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Key::Space, testBall);
    
#pragma endregion

#pragma region Events

    // zombiesRemainingText->GetComponent<ScoreCounter>()->OnGivenScoreReachedEvent.AddListener(pistol->GetComponent<zombieArena::Pistol>(), &zombieArena::Pistol::SaveInfo);
    
#pragma endregion
}