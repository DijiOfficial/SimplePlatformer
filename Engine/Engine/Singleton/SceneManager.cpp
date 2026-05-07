#include "SceneManager.h"
#include "TimerManager.h"
#include "../Components/Transform.h"
#include "../Input/InputManager.h"

#include <ranges>
#include <stdexcept>

diji::Scene* diji::SceneManager::CreateScene(const int id)
{
    if (!m_PhysicsWorldUPtr)
        m_PhysicsWorldUPtr = std::make_unique<PhysicsWorld>();

    if (!m_TimelineManagerUPtr)
        m_TimelineManagerUPtr = std::make_unique<TimelineManager>();
    
    // Check if the scene already exists in the map
    const auto it = m_ScenesUPtrMap.find(id);
    if (it != m_ScenesUPtrMap.end())
    {
        // Scene already exists, return the existing scene
        return it->second.get();
    }

    // Scene does not exist, create a new one and store it in the map
    m_ScenesUPtrMap[id] = std::make_unique<Scene>();

    // if multiplayer is enabled, set the new scene to use split screen
    if (m_IsMultiplayer)
        m_ScenesUPtrMap[id].get()->SetMultiplayerSplitScreen(m_NumPlayers);
    
    return m_ScenesUPtrMap[id].get();
}

void diji::SceneManager::Init()
{
    if (!m_PhysicsWorldUPtr)
        m_PhysicsWorldUPtr = std::make_unique<PhysicsWorld>();

    if (!m_TimelineManagerUPtr)
        m_TimelineManagerUPtr = std::make_unique<TimelineManager>();
    
    TimerManager::GetInstance().Init();
    m_ScenesUPtrMap.at(m_ActiveSceneId)->Init();
}

void diji::SceneManager::Start() const
{
    m_ScenesUPtrMap.at(m_ActiveSceneId)->Start();
}

void diji::SceneManager::FixedUpdate() const
{
    m_PhysicsWorldUPtr->FixedUpdate();
    
    m_ScenesUPtrMap.at(m_ActiveSceneId)->FixedUpdate();
}

void diji::SceneManager::Update() const
{
    m_ScenesUPtrMap.at(m_ActiveSceneId)->Update();

    m_TimelineManagerUPtr->UpdateAll();
}

void diji::SceneManager::LateUpdate() const
{
    m_ScenesUPtrMap.at(m_ActiveSceneId)->LateUpdate();

    TimerManager::GetInstance().Update();
}

void diji::SceneManager::Render() const
{
    m_ScenesUPtrMap.at(m_ActiveSceneId)->Render();
}

void diji::SceneManager::OnDestroy() const
{
    m_ScenesUPtrMap.at(m_ActiveSceneId)->OnDestroy();
}

void diji::SceneManager::EndFrameUpdate()
{
    //  We can load the new scene
    if (m_IsSceneChange) // todo: async new scene loading
    {
        m_IsSceneChange = false;

        // Clear Commands assigned for that scene
        InputManager::GetInstance().ResetCommands();

        // Clear all timers
        TimerManager::GetInstance().ClearAllTimers();

        // Clear Physics World
        m_PhysicsWorldUPtr->Reset();

        // Clear Timeline Manager
        m_TimelineManagerUPtr->ClearAll();
        
        // Destroy current scene
        OnDestroy();
        m_ScenesUPtrMap.erase(m_ActiveSceneId);
        
        m_ActiveSceneId = m_NextScene;

        // Load New scene
        if (const auto it = m_SceneLoaders.find(m_ActiveSceneId); it != m_SceneLoaders.end())
            it->second();
        else
            throw std::runtime_error("SceneLoader not registered for SceneId.");

        Init();
        Start();
        
        return;
    }

    // Handle pending destroy
    if (!m_HasPendingDestroy) return;
    // todo: Instead of iterating through all of them and removing them, pass it to the scene so I can use swap method to destroy all of them without having to iterate over all of the game objects multiple time.
    for (const auto gameObject : m_PendingDestroyVec)
    {
        m_ScenesUPtrMap.at(m_ActiveSceneId)->Remove(gameObject);
        m_TimelineManagerUPtr->ClearGameObjectTimelines(gameObject);
    }

    m_PendingDestroyVec = std::vector<const GameObject*>();
    m_HasPendingDestroy = false;

    m_ScenesUPtrMap.at(m_ActiveSceneId)->ValidateCollidersAfterDestroy();
}

void diji::SceneManager::SetPendingDestroy(const GameObject* gameObject)
{
    m_HasPendingDestroy = true;
    m_PendingDestroyVec.push_back(gameObject);
}

diji::GameObject* diji::SceneManager::GetMainCamera() const
{
    return m_ScenesUPtrMap.at(m_ActiveSceneId)->GetMainCamera();
}

diji::GameObject* diji::SceneManager::GetGameObject(const std::string& name) const
{
    return m_ScenesUPtrMap.at(m_ActiveSceneId)->GetGameObject(name);
}

std::string diji::SceneManager::GetGameObjectName(const GameObject* object) const
{
    return m_ScenesUPtrMap.at(m_ActiveSceneId)->GetGameObjectName(object);
}

diji::GameObject* diji::SceneManager::SpawnGameObject(const std::string& name, const GameObject* original, const sf::Vector2f& spawnLocation) const
{
    return CreateAndInitGameObject(spawnLocation,
        [name, original](auto* scene) -> GameObject*
        {
            return scene->CreateGameObjectFromTemplate(name, original);
        });
}

diji::GameObject* diji::SceneManager::SpawnGameObject(const std::string& name, std::unique_ptr<GameObject> original, const sf::Vector2f& spawnLocation) const
{
    return CreateAndInitGameObject(spawnLocation,
        [orig = std::move(original), name = name](auto* scene) mutable -> GameObject*
        {
            return scene->AddObjectToScene(std::move(orig), name);
        });
}

diji::GameObject* diji::SceneManager::AddGameObjectToCanvas(const std::string& name, const GameObject* original, const sf::Vector2f& spawnLocation) const
{
    return CreateAndInitGameObject(spawnLocation,
        [name, original](auto* scene) -> diji::GameObject*
        {
            return scene->CreateCanvasObjectFromTemplate(name, original);
        });
}

diji::GameObject* diji::SceneManager::AddGameObjectToCanvas(const std::string& name, std::unique_ptr<GameObject> original, const sf::Vector2f& spawnLocation) const
{
    return CreateAndInitGameObject(spawnLocation,
        [orig = std::move(original), name = name](auto* scene) mutable -> GameObject*
        {
            return scene->AddObjectToCanvas(std::move(orig), name);
        });
}

diji::GameObject* diji::SceneManager::OverwriteGameObject(const std::string& name, const GameObject* original, const sf::Vector2f& spawnLocation) const
{
    return CreateAndInitGameObject(spawnLocation,
        [name, original](auto* scene) -> GameObject*
        {
            return scene->OverwriteGameObjectFromTemplate(name, original);
        });
}

diji::GameObject* diji::SceneManager::OverwriteGameObject(const std::string& name, std::unique_ptr<GameObject> original, const sf::Vector2f& spawnLocation) const
{
    return CreateAndInitGameObject(spawnLocation,
        [orig = std::move(original), name = name](auto* scene) mutable -> GameObject*
        {
            return scene->OverwriteObjectInScene(std::move(orig), name);
        });
}

diji::GameObject* diji::SceneManager::OverwriteCanvasObject(const std::string& name, const GameObject* original, const sf::Vector2f& spawnLocation) const
{
    return CreateAndInitGameObject(spawnLocation,
        [name, original](auto* scene) -> GameObject*
        {
            return scene->OverwriteCanvasObjectFromTemplate(name, original);
        });
}

diji::GameObject* diji::SceneManager::OverwriteCanvasObject(const std::string& name, std::unique_ptr<GameObject> original, const sf::Vector2f& spawnLocation) const
{
    return CreateAndInitGameObject(spawnLocation,
        [orig = std::move(original), name = name](auto* scene) mutable -> GameObject*
        {
            return scene->OverwriteObjectInCanvas(std::move(orig), name);
        });
}

void diji::SceneManager::SetGameObjectToRenderOnTop(const GameObject* gameObject) const
{
    m_ScenesUPtrMap.at(m_ActiveSceneId)->SetGameObjectToRenderOnTop(gameObject);
}

void diji::SceneManager::ChangePlayerViewCenter(const int currPlayer, const sf::Vector2f& newCenter) const
{
    m_ScenesUPtrMap.at(m_ActiveSceneId)->ChangeViewCenter(currPlayer, newCenter);
}

void diji::SceneManager::SetViewParameters(const int idx, Transform* target, const bool isFollowing, const sf::Vector2f& offset) const
{
    for (const auto& scene : m_ScenesUPtrMap | std::views::values)
    {
        scene->SetViewParameters(idx, target, isFollowing, offset);
    }
}

sf::Vector2f diji::SceneManager::GetScreenPosition(const sf::Vector2f& mapCoords) const
{
    return static_cast<sf::Vector2f>(m_ScenesUPtrMap.at(m_ActiveSceneId)->GetScreenPosition(mapCoords));
}

sf::Vector2f diji::SceneManager::GetWorldPositionFromScreen(const sf::Vector2f& mapCoords) const
{
    return m_ScenesUPtrMap.at(m_ActiveSceneId)->GetWorldPositionFromScreen(static_cast<sf::Vector2i>(mapCoords));
}

void diji::SceneManager::SetMultiplayerSplitScreen(const int numPlayers)
{
    m_IsMultiplayer = true;
    m_NumPlayers = numPlayers;

    if (numPlayers == 1)
        m_IsMultiplayer = false;

    // update all existing scenes to use split screen
    for (const auto& scene : m_ScenesUPtrMap | std::views::values)
    {
        scene->SetMultiplayerSplitScreen(numPlayers);
    }
}

diji::Timeline* diji::SceneManager::CreateTimeline(const GameObject* owner) const
{
    return m_TimelineManagerUPtr->CreateTimeline(owner);
}
