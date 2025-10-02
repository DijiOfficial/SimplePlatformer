#include "GameLoader.h"

#include "GameState.h"
#include "../Components/BackgroundHandler.h"
#include "../Components/PlayerCharacter.h"
#include "../Input/CustomCommands.h"
#include "../Singletons/GameManager.h"
#include "Engine//Collision/Collider.h"
#include "Engine/Components/TextureComp.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Interfaces/ISoundSystem.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/Render.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/RectRender.h"
#include "Engine/Components/Sprite.h"
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

    const auto thomas = scene->CreateGameObject("X_Thomas");
    thomas->AddComponents<Transform>(0, 0);
    thomas->AddComponents<TextureComp>("graphics/thomas.png");
    thomas->AddComponents<Render>();
    thomas->AddComponents<Collider>(44, 70, sf::Vector2f{ 3.f, 30.f });
    thomas->AddComponents<thomasWasLate::PlayerCharacter>(0.45f);
    thomas->AddComponents<RectRender>(true);

    const auto bob = scene->CreateGameObject("X_Thomas2", thomas);
    bob->GetComponent<thomasWasLate::PlayerCharacter>()->SetMass(25);
    
#pragma region Commands
    auto& input = InputManager::GetInstance();

    input.BindCommand<thomasWasLate::MoveCharacter>(PlayerIdx::KEYBOARD, KeyState::HELD, sf::Keyboard::Key::D, thomas, sf::Vector2f{ 1.f, 0.f });
    input.BindCommand<thomasWasLate::MoveCharacter>(PlayerIdx::KEYBOARD, KeyState::HELD, sf::Keyboard::Key::A, thomas, sf::Vector2f{ -1.f, 0.f });

    input.BindCommand<thomasWasLate::CharacterJump>(PlayerIdx::KEYBOARD, KeyState::HELD, sf::Keyboard::Key::W, thomas, true);
    input.BindCommand<thomasWasLate::CharacterJump>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Key::W, thomas, false);
    
    
#pragma endregion

#pragma region Events

    // zombiesRemainingText->GetComponent<ScoreCounter>()->OnGivenScoreReachedEvent.AddListener(pistol->GetComponent<zombieArena::Pistol>(), &zombieArena::Pistol::SaveInfo);
    
#pragma endregion
}