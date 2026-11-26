#include "../GameLoader.h"
#include "../GameState.h"
#include "../../Components/Backgrounds/CustomBackgroundRenderer.h"
#include "../../Components/Backgrounds/BackgroundHandler.h"
#include "../../Components/LevelEditor/MenuItems/LoadMenu.h"
#include "../../Components/LevelEditor/MenuItems/SaveMenu.h"
#include "../../Components/LevelEditor/SelectorControls.h"
#include "../../Components/LevelEditor/Selector.h"
#include "../../Components/LevelEditor/MenuItems/BlockSelector.h"
#include "../../Components/LevelEditor/MenuItems/BackgroundSelector.h"
#include "../../Components/LevelEditor/MenuItems/PlayMenu.h"
#include "../../Components/LevelEditor/OnScreenKeyboard/OnScreenKeyboardManager.h"
#include "../../Helpers/WorldBuilder.h"
#include "../../Input/LevelEditorCommands.h"
#include "Engine/Components/TextureComp.h"
#include "Engine/Core/Renderer.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/FPSCounter.h"
#include "Engine/Components/ShapeRender.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Singleton/GameStateManager.h"
#include "Engine/Components/Sprite.h"
#include "Engine/Components/TextComp.h"

using namespace diji;

void SceneLoader::LevelEditor()
{
    // ServiceLocator::GetSoundSystem().AddSoundRequest("sound/LevelMusic.mp3", true);

    SceneManager::GetInstance().SetActiveScene(static_cast<int>(superMarioBros::superMarioBrosState::LevelEditor));
    const auto& scene = SceneManager::GetInstance().CreateScene(static_cast<int>(superMarioBros::superMarioBrosState::LevelEditor));
    GameStateManager::GetInstance().SetNewGameState(static_cast<GameState>(superMarioBros::superMarioBrosState::LevelEditor));
    Renderer::GetInstance().SetBackgroundColor(superMarioBros::WorldBuilder::WorldSettings::SKY_COLOR);

    SceneManager::GetInstance().GetPhysicsWorld()->DisablePhysics();

    const auto selector = scene->CreateGameObject("X_SelectorPlayer");
    const auto staticBackground = scene->CreateGameObject("A_StaticBackground");
    staticBackground->SetObjectPosition({ 0, 228 });
    staticBackground->AddComponent<TextureComp>("graphics/background.png");
    staticBackground->AddComponent<superMarioBros::CustomBackgroundRenderer>(selector->GetRootComponent());
    staticBackground->SetRenderLayer(-10);
    scene->SetToAlwaysRender(staticBackground, true);
    
    selector->SetObjectPosition({ 525, 25 });
    selector->AddComponent<TextureComp>("graphics/squareWhiteSmaller50.png");
    selector->AddComponent<Render>();
    const auto& selectorControlsComp = selector->AddComponent<superMarioBros::SelectorControls>();
    const auto& selectorComp = selector->AddComponent<superMarioBros::Selector>();
    selector->AddComponent<Camera>(sf::Vector2f{ 1920.0f, 1080.0f });
    selector->GetComponent<Camera>()->SetLevelBoundaries(superMarioBros::WorldBuilder::WorldSettings::ARENA_SIZE);
    selector->SetRenderLayer(10);
    
    // todo: fix background to only parse the visible background
    const auto background = scene->CreateGameObject("B_Background");
    background->SetObjectPosition({ 0, 0 });
    background->AddComponent<Sprite>("graphics/tiles_sheet.png");
    background->AddComponent<Render>();
    const auto& backgroundHandlerComp = background->AddComponent<superMarioBros::BackgroundHandler>();
    background->GetComponent<superMarioBros::BackgroundHandler>()->DisableLevelLoadingOnStart();
    selectorComp->SetBackgroundHandlerRef(backgroundHandlerComp);
    scene->SetToAlwaysRender(background,  true);

    const auto player = scene->CreateGameObject("X_PlayerChar");
    player->SetObjectPosition({ 200, 0 });
    AddPlayerComponents(player);
    player->SetActive(false);

    const auto tempKeyboard = scene->CreateGameObject("X_tempKeyboard");
    tempKeyboard->AddComponent<onScreenKeyboard::OnScreenKeyboardManager>();

#pragma region HUD
    float menuYPosition = static_cast<float>(window::VIEWPORT.y) * 0.15f;
    std::vector<superMarioBros::MenuItem*> menuTransforms;
    const float renderRatio = static_cast<float>(window::g_window_ptr->getSize().y) / 1080.0f;
    selectorControlsComp->SetMenuRenderRatio(renderRatio);
    
    const auto download = scene->CreateGameObject("Z_UI_Download");
    download->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.1f, menuYPosition });
    download->AddComponent<TextureComp>("graphics/level_editor_download.png");
    download->AddComponent<Render>(renderRatio);
    const auto& loadMenuComp = download->AddComponent<superMarioBros::LoadMenu>();
    scene->SetGameObjectAsCanvasObject(download);
    
    const auto save = scene->CreateGameObject("Z_UI_Save");
    save->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.125f, menuYPosition });
    save->AddComponent<TextureComp>("graphics/level_editor_save.png");
    save->AddComponent<Render>(renderRatio);
    const auto& saveMenuComp = save->AddComponent<superMarioBros::SaveMenu>();
    scene->SetGameObjectAsCanvasObject(save);

    const auto selection = scene->CreateGameObject("Z_UI_Selection");
    selection->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.09f, menuYPosition * 2 });
    selection->AddComponent<TextureComp>("graphics/level_editor_selection.png");
    selection->AddComponent<Render>(renderRatio);
    selection->SetActive(false);
    scene->SetGameObjectAsCanvasObject(selection);
    
    const auto items = scene->CreateGameObject("Z_UI_Items");
    items->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.150f, menuYPosition });
    items->AddComponent<TextureComp>("graphics/level_editor_item1.png");
    items->AddComponent<Render>(renderRatio);
    const auto& blockSelector = items->AddComponent<superMarioBros::BlockSelector>();
    blockSelector->SetSelectorControls(selectorControlsComp);
    blockSelector->SetSelector(selectorComp);
    scene->SetGameObjectAsCanvasObject(items);

    const auto itemChoices = scene->CreateGameObject("Z_UI_ItemsChoice");
    itemChoices->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.150f, menuYPosition * 3 });
    itemChoices->AddComponent<TextureComp>("graphics/tiles_sheet_selection.png");
    itemChoices->AddComponent<Render>(renderRatio);
    scene->SetGameObjectAsCanvasObject(itemChoices);
    itemChoices->AttachToObject(items, true);

    const auto backgroundMenu = scene->CreateGameObject("UI_Background");
    backgroundMenu->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.175f, menuYPosition });
    backgroundMenu->AddComponent<TextureComp>("graphics/level_editor_background.png");
    backgroundMenu->AddComponent<Render>(renderRatio);
    const auto& backgroundSelector = backgroundMenu->AddComponent<superMarioBros::BackgroundSelector>();
    backgroundSelector->SetSelectorControls(selectorControlsComp);
    scene->SetGameObjectAsCanvasObject(backgroundMenu);

    const auto backgroundChoices = scene->CreateGameObject("UI_BackgroundChoice");
    backgroundChoices->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.175f, menuYPosition * 3 });
    backgroundChoices->AddComponent<TextureComp>("graphics/level_editor_backgrounds.png");
    backgroundChoices->AddComponent<Render>(renderRatio);
    scene->SetGameObjectAsCanvasObject(backgroundChoices);
    backgroundChoices->AttachToObject(backgroundMenu, true);

    const auto play = scene->CreateGameObject("UI_Play");
    play->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.200f, menuYPosition });
    play->AddComponent<TextureComp>("graphics/level_editor_play.png");
    play->AddComponent<Render>(renderRatio);
    play->AddComponent<superMarioBros::PlayMenu>();
    scene->SetGameObjectAsCanvasObject(play);

    // const auto secondItem = scene->CreateGameObject("Z_UI_Item2");
    // secondItem->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x) * 0.20f, menuYPosition });
    // secondItem->AddComponent<TextureComp>("graphics/level_editor_item1.png");
    // secondItem->AddComponent<Render>(renderRatio);
    // secondItem->AddComponent<superMarioBros::MenuItem>();
    // scene->SetGameObjectAsCanvasObject(secondItem);

    menuTransforms.push_back(download->GetComponent<superMarioBros::MenuItem>());
    menuTransforms.push_back(save->GetComponent<superMarioBros::MenuItem>());
    menuTransforms.push_back(items->GetComponent<superMarioBros::MenuItem>());
    menuTransforms.push_back(backgroundMenu->GetComponent<superMarioBros::MenuItem>());
    menuTransforms.push_back(play->GetComponent<superMarioBros::MenuItem>());
    // menuTransforms.push_back(secondItem->GetComponent<superMarioBros::MenuItem>());
    selector->GetComponent<superMarioBros::SelectorControls>()->SetMenuTransform(menuTransforms);
    selector->GetComponent<superMarioBros::SelectorControls>()->SetMenuArrow(selection);

    const auto fpsCounter = scene->CreateGameObject("Z_FPSCounter");
    fpsCounter->AddComponent<TextComp>("0 FPS", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    fpsCounter->GetComponent<TextComp>()->GetText().setCharacterSize(10);
    fpsCounter->AddComponent<FPSCounter>();
    fpsCounter->SetObjectPosition({ static_cast<float>(window::VIEWPORT.x - 100), 40 });
    fpsCounter->AddComponent<Render>();
    scene->SetGameObjectAsCanvasObject(fpsCounter);
#pragma endregion
    
#pragma region Commands
    BindLevelEditorCommands();
#pragma endregion

#pragma region Events

    saveMenuComp->OnSaveLevelEvent.AddListener(loadMenuComp, &superMarioBros::LoadMenu::AddNewName);
    // superMarioBros::GameManager::GetInstance().OnScoreAddedEvent.AddListener(scoreHUD->GetComponent<ScoreCounter>(), &ScoreCounter::IncreaseScore);
    // superMarioBros::GameManager::GetInstance().OnCoinCollectedEvent.AddListener(coinsCounterHud->GetComponent<ScoreCounter>(), &ScoreCounter::IncreaseScore);
    // timerHUD->GetComponent<ScoreCounter>()->OnGivenScoreReachedEvent.AddListener(player->GetComponent<superMarioBros::PlayerCharacter>(), &superMarioBros::PlayerCharacter::KillPlayer);
#pragma endregion
}

void SceneLoader::BindLevelEditorCommands()
{
    const auto& sceneManager = SceneManager::GetInstance();
    GameObject* selector = sceneManager.GetGameObject("X_SelectorPlayer");
    GameObject* items = sceneManager.GetGameObject("Z_UI_Items");

    auto& input = InputManager::GetInstance();

    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::D, selector, sf::Vector2f{ 1.0f, 0.0f }, true);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::Right, selector, sf::Vector2f{ 1.0f, 0.0f }, true);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::DPadRight, selector, sf::Vector2f{ 1.0f, 0.0f }, true);
    
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::A, selector, sf::Vector2f{ -1.0f, 0.0f }, true);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::Left, selector, sf::Vector2f{ -1.0f, 0.0f }, true);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::DPadLeft, selector, sf::Vector2f{ -1.0f, 0.0f }, true);
    
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::W, selector, sf::Vector2f{ 0.0f, -1.0f }, true);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::Up, selector, sf::Vector2f{ 0.0f, -1.0f }, true);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::DPadUp, selector, sf::Vector2f{ 0.0f, -1.0f }, true);
    
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::S, selector, sf::Vector2f{ 0.0f, 1.0f }, true);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::Down, selector, sf::Vector2f{ 0.0f, 1.0f }, true);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::DPadDown, selector, sf::Vector2f{ 0.0f, 1.0f }, true);

    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::D, selector, sf::Vector2f{ 1.0f, 0.0f }, false);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::Right, selector, sf::Vector2f{ 1.0f, 0.0f }, false);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::PLAYER1, KeyState::RELEASED, Controller::Button::DPadRight, selector, sf::Vector2f{ 1.0f, 0.0f }, false);
    
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::A, selector, sf::Vector2f{ -1.0f, 0.0f }, false);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::Left, selector, sf::Vector2f{ -1.0f, 0.0f }, false);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::PLAYER1, KeyState::RELEASED, Controller::Button::DPadLeft, selector, sf::Vector2f{ -1.0f, 0.0f }, false);
    
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::W, selector, sf::Vector2f{ 0.0f, -1.0f }, false);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::Up, selector, sf::Vector2f{ 0.0f, -1.0f }, false);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::PLAYER1, KeyState::RELEASED, Controller::Button::DPadUp, selector, sf::Vector2f{ 0.0f, -1.0f }, false);
    
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::S, selector, sf::Vector2f{ 0.0f, 1.0f }, false);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::KEYBOARD, KeyState::RELEASED, sf::Keyboard::Scancode::Down, selector, sf::Vector2f{ 0.0f, 1.0f }, false);
    input.BindCommand<superMarioBros::MoveSelector>(PlayerIdx::PLAYER1, KeyState::RELEASED, Controller::Button::DPadDown, selector, sf::Vector2f{ 0.0f, 1.0f }, false);

    input.BindCommand<superMarioBros::SelectMenuItem>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::Enter, selector);
    input.BindCommand<superMarioBros::SelectMenuItem>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::A, selector);

    input.BindCommand<superMarioBros::ClearSpecialMenu>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::Escape, selector);
    input.BindCommand<superMarioBros::ClearSpecialMenu>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::B, selector);
    
    input.BindCommand<superMarioBros::OpenSaveMenu>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::Start, selector);
    
    input.BindCommand<superMarioBros::OpenBlocksMenu>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::Back, selector);
    
    input.BindCommand<superMarioBros::SelectNextBlock>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::RightShoulder, items, true);
    input.BindCommand<superMarioBros::SelectNextBlock>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::LeftShoulder, items, false);

    input.BindCommand<superMarioBros::CopyBlockHoveredPosition>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::X, items);
}

void SceneLoader::BindPlayTestCommands()
{
    auto& input = InputManager::GetInstance();

    input.BindCommand<superMarioBros::ExitPlayTest>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::Enter, nullptr);
    input.BindCommand<superMarioBros::ExitPlayTest>(PlayerIdx::KEYBOARD, KeyState::PRESSED, sf::Keyboard::Scancode::Escape, nullptr);
    input.BindCommand<superMarioBros::ExitPlayTest>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::Start, nullptr);
    input.BindCommand<superMarioBros::ExitPlayTest>(PlayerIdx::PLAYER1, KeyState::PRESSED, Controller::Button::Back, nullptr);
}
