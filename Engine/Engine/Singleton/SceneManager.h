#pragma once
#include "Singleton.h"
#include "../Core/Scene.h"
#include "../Collision/PhysicsWorld.h"
#include "../Interfaces/TimelineManager.h"
#include "../Components/Transform.h"

namespace diji
{
    class Timeline;

    class SceneManager final : public Singleton<SceneManager>
    {
    public:
        Scene* CreateScene(int id);

        // Similar loop to Unity
        // Initialization phase
        void Init();                // 1st
        void Start() const;         // 2nd

        // Game loop
        void FixedUpdate() const;   // 3rd
        void Update() const;        // 4th
        void LateUpdate() const;    // 5th
        void Render() const;        // 6th

        // Destroy Phase
        void OnDestroy() const;     // 6th

        // Handle End of Frame tasks
        void EndFrameUpdate();

        void SetActiveScene(const int id) { m_ActiveSceneId = id; m_NextScene = id; }
        //todo: better name would be ChangeScene
        void SetNextSceneToActivate(const int id) { m_NextScene = id; m_IsSceneChange = true; }
        [[nodiscard]] int GetActiveSceneId() const { return m_ActiveSceneId; }
        void SetPendingDestroy(const GameObject* gameObject);

        [[nodiscard]] GameObject* GetMainCamera() const;
        [[nodiscard]] GameObject* GetGameObject(const std::string& name) const;
        [[nodiscard]] std::string GetGameObjectName(const GameObject* object) const;
        GameObject* SpawnGameObject(const std::string& name, const GameObject* original, const sf::Vector2f& spawnLocation) const;
        GameObject* SpawnGameObject(const std::string& name, std::unique_ptr<GameObject> original, const sf::Vector2f& spawnLocation) const;
        GameObject* AddGameObjectToCanvas(const std::string& name, const GameObject* original, const sf::Vector2f& spawnLocation) const;
        GameObject* AddGameObjectToCanvas(const std::string& name, std::unique_ptr<GameObject> original, const sf::Vector2f& spawnLocation) const;
        GameObject* OverwriteGameObject(const std::string& name, const GameObject* original, const sf::Vector2f& spawnLocation) const;
        GameObject* OverwriteGameObject(const std::string& name, std::unique_ptr<GameObject> original, const sf::Vector2f& spawnLocation) const;
        GameObject* OverwriteCanvasObject(const std::string& name, const GameObject* original, const sf::Vector2f& spawnLocation) const;
        GameObject* OverwriteCanvasObject(const std::string& name, std::unique_ptr<GameObject> original, const sf::Vector2f& spawnLocation) const;
        void SetGameObjectToRenderOnTop(const GameObject* gameObject) const;
        

        void ChangePlayerViewCenter(int currPlayer, const sf::Vector2f& newCenter) const;
        void SetViewParameters(int idx, Transform* target, bool isFollowing = false, const sf::Vector2f& offset = {}) const;
        [[nodiscard]] sf::Vector2f GetScreenPosition(const sf::Vector2f& mapCoords) const;
        [[nodiscard]] sf::Vector2f GetWorldPositionFromScreen(const sf::Vector2f& mapCoords) const;

        using SceneLoaderFunc = std::function<void()>;
        void RegisterScene(const int id, SceneLoaderFunc loader) { m_SceneLoaders[id] = std::move(loader); }

        void SetMultiplayerSplitScreen(int numPlayers);

        [[nodiscard]] PhysicsWorld* GetPhysicsWorld() const { return m_PhysicsWorldUPtr.get(); }
        Timeline* CreateTimeline(const GameObject* owner) const;

    private:
        // todo: replace int with SceneId enum class??
        std::map<int, std::unique_ptr<Scene>> m_ScenesUPtrMap;
        std::vector<const GameObject*> m_PendingDestroyVec;
        std::unordered_map<int, SceneLoaderFunc> m_SceneLoaders;
        std::unique_ptr<PhysicsWorld> m_PhysicsWorldUPtr = nullptr;
        std::unique_ptr<TimelineManager> m_TimelineManagerUPtr = nullptr;
        
        int m_ActiveSceneId = 0;
        int m_NextScene = 0;
        bool m_IsSceneChange = false;
        bool m_HasPendingDestroy = false;
        bool m_IsMultiplayer = false;
        int m_NumPlayers = 0;

        template<typename Function>
        GameObject* CreateAndInitGameObject(const sf::Vector2f& spawnLocation, Function&& func) const
        {
            auto* scene = m_ScenesUPtrMap.at(m_ActiveSceneId).get();
            GameObject* gameObject = std::forward<Function>(func)(scene);

            gameObject->SetObjectPosition(spawnLocation);
            gameObject->Init();
            gameObject->Start();

            return gameObject;
        }
    };
}
