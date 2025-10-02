#include "SceneManager.h"

#include <ranges>
#include <stdexcept>

#include "TimerManager.h"
#include "../Collision/CollisionSingleton.h"
#include "../Components/Transform.h"
#include "../Input/InputManager.h"

diji::Scene* diji::SceneManager::CreateScene(const int id)
{
    // Check if the scene already exists in the map
    auto it = m_ScenesUPtrMap.find(id);
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

void diji::SceneManager::Init() const
{
    TimerManager::GetInstance().Init();
    m_ScenesUPtrMap.at(m_ActiveSceneId)->Init();
}

void diji::SceneManager::Start() const
{
    m_ScenesUPtrMap.at(m_ActiveSceneId)->Start();
}

void diji::SceneManager::FixedUpdate() const
{
    m_ScenesUPtrMap.at(m_ActiveSceneId)->FixedUpdate();
}

void diji::SceneManager::Update() const
{
    m_ScenesUPtrMap.at(m_ActiveSceneId)->Update();
    TimerManager::GetInstance().Update(); // I'm not sure if it matters where/when this is updated. (before/after comps, late update?)
}

void diji::SceneManager::LateUpdate() const
{
    m_ScenesUPtrMap.at(m_ActiveSceneId)->LateUpdate();
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
    // Handle pending destroy
    if (m_HasPendingDestroy)
    {
        // todo: Instead of iterating through all of them and removing them, pass it to the scene so I can use swap method to destroy all of them without having to iterate over all of the game objects multiple time.
        for (const auto gameObject : m_PendingDestroyVec)
        {
            m_ScenesUPtrMap.at(m_ActiveSceneId)->Remove(gameObject);
        }

        m_PendingDestroyVec = std::vector<const GameObject*>();
        m_HasPendingDestroy = false;
    }
    
    //  We can load the new scene
    if (m_IsSceneChange) // todo: async new scene loading
    {
        m_IsSceneChange = false;

        // Clear Commands assigned for that scene
        InputManager::GetInstance().ResetCommands();

        // Clear All Colliders
        CollisionSingleton::GetInstance().Reset();

        // Clear all timers
        TimerManager::GetInstance().ClearAllTimers();
        
        // Destroy current scene
        OnDestroy();
        m_ScenesUPtrMap.erase(m_ActiveSceneId);
        
        m_ActiveSceneId = m_NextScene;

        // Load New scene
        if (const auto it = m_SceneLoaders.find(m_ActiveSceneId); it != m_SceneLoaders.end())
            it->second();
        else
            throw std::runtime_error("SceneLoader not registered for SceneId.");

        m_ScenesUPtrMap.at(m_ActiveSceneId)->Init();
        m_ScenesUPtrMap.at(m_ActiveSceneId)->Start();
    }
}

void diji::SceneManager::SetPendingDestroy(const GameObject* gameObject)
{
    m_HasPendingDestroy = true;
    m_PendingDestroyVec.push_back(gameObject);
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
    const auto gameObject = m_ScenesUPtrMap.at(m_ActiveSceneId).get()->CreateGameObjectFromTemplate(name, original);

    gameObject->GetComponent<Transform>()->SetPosition(spawnLocation);

    gameObject->Init();
    gameObject->Start();

    return gameObject;
}

void diji::SceneManager::ChangePlayerViewCenter(const int currPlayer, const sf::Vector2f& newCenter) const
{
    m_ScenesUPtrMap.at(m_ActiveSceneId)->ChangeViewCenter(currPlayer, newCenter);
}

void diji::SceneManager::SetViewParameters(const int idx, const Transform* target, const bool isFollowing, const sf::Vector2f& offset) const
{
    for (const auto& scene : m_ScenesUPtrMap | std::views::values)
    {
        scene->SetViewParameters(idx, target, isFollowing, offset);
    }
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
