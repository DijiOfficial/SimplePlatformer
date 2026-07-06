#pragma once
#include "GameObject.h"
#include "Engine.h"
#include "../RAII_Wrappers/SplitScreenView.h"

#include <map>
#include <memory>
#include <string>
#include <SFML/Graphics/View.hpp>

namespace diji 
{
    class Camera;

    class Scene final
    {
        friend class SceneManager;
        
    public:
        Scene() = default;
        ~Scene() noexcept;

        Scene(const Scene& other) = delete;
        Scene(Scene&& other) = delete;
        Scene& operator=(const Scene& other) = delete;
        Scene& operator=(Scene&& other) = delete;

        void Init();
        void Start();
        void FixedUpdate();
        void Update();
        void LateUpdate();
        void Render() const;
        void RenderMultiplayerViews() const;
        void OnDestroy();
		
        [[nodiscard]] GameObject* CreateCameraObject(const std::string& name);
        [[nodiscard]] GameObject* CreateGameObject(const std::string& name);
        [[nodiscard]] GameObject* CreateGameObject(const std::string& name, const GameObject* original);
        [[nodiscard]] GameObject* CreateGameObjectFromTemplate(const std::string& name, const GameObject* original);
        [[nodiscard]] GameObject* CreateCanvasObjectFromTemplate(const std::string& name, const GameObject* original);
        [[nodiscard]] GameObject* AddObjectToScene(std::unique_ptr<GameObject> object, const std::string& desiredName);
        [[nodiscard]] GameObject* AddObjectToCanvas(std::unique_ptr<GameObject> object, const std::string& desiredName);
        [[nodiscard]] GameObject* OverwriteGameObjectFromTemplate(const std::string& name, const GameObject* original);
        [[nodiscard]] GameObject* OverwriteCanvasObjectFromTemplate(const std::string& name, const GameObject* original);
        [[nodiscard]] GameObject* OverwriteObjectInScene(std::unique_ptr<GameObject> object, const std::string& name);
        [[nodiscard]] GameObject* OverwriteObjectInCanvas(std::unique_ptr<GameObject> object, const std::string& name);
        void Remove(const GameObject* object);
        void Remove(const std::string& name);
        void RemoveAll();
        [[nodiscard]] GameObject* GetMainCamera() const { return m_MainCameraObjPtr; }
        [[nodiscard]] GameObject* GetGameObject(const std::string& name) const;
        [[nodiscard]] std::string GetGameObjectName(const GameObject* object) const;
        void ChangeViewCenter(int idx, const sf::Vector2f& newCenter);
        void SetViewParameters(int idx, Transform* target, bool isFollowing = false, const sf::Vector2f& offset = {});
        [[nodiscard]] sf::Vector2i GetScreenPosition(const sf::Vector2f& worldCoords) const;
        [[nodiscard]] sf::Vector2f GetWorldPositionFromScreen(const sf::Vector2i& screenCoords) const;
        
        void SetGameObjectAsCanvasObject(const std::string& name);
        void SetGameObjectAsCanvasObject(const GameObject* object);
        void SetCanvasView(const sf::View& view) { m_CanvasView = view; }

        void SetMultiplayerSplitScreen(int numPlayers);
        void SetGameObjectAsStaticBackground(const std::string& name);
        void SetGameObjectAsStaticBackground(const GameObject* object);
        void ValidateCollidersAfterDestroy();
        void SetToAlwaysRender(const GameObject* object, bool shouldAlwaysRender);
    
    private:
        std::unordered_map<std::string, unsigned long long int> m_NameIndexUMap;
        std::unordered_set<const GameObject*> m_AlwaysRender;
        std::unordered_map<std::string, std::unique_ptr<GameObject>> m_ObjectsUPtrMap;
        std::unordered_map<std::string, std::unique_ptr<GameObject>> m_CanvasObjectsUPtrMap;
        std::vector<SplitScreenView> m_MultiplayerViews;
        std::vector<SplitScreenView> m_MultiplayerViewsCopy;
        std::unique_ptr<GameObject> m_StaticBackgroundObjUPtr = nullptr;
        GameObject* m_MainCameraObjPtr = nullptr;
        Camera* m_MainCameraCompPtr = nullptr;
        sf::View m_MainCameraViewCopy = {};
        sf::View m_CanvasView = sf::View(sf::Vector2f{ static_cast<float>(window::VIEWPORT.x) * 0.5f, static_cast<float>(window::VIEWPORT.y) * 0.5f }, sf::Vector2f{ window::VIEWPORT });
        bool m_IsUsingMultiplayerViews = false;
        bool m_RenderBackground = false;

        void DrawGameObjects() const;
        std::string GenerateUniqueName(const std::unordered_map<std::string, std::unique_ptr<GameObject>>& objectMap, const std::string& baseName);

        template<typename TMap>
        bool RemoveFromContainer(TMap& container, const GameObject* object)
        {
            const auto it = container.find(object->GetName());
            if (it == container.end())
                return false;

            RemoveFromChunk(it);
            auto localUp = std::move(it->second);
            container.erase(it);

            localUp->OnDestroy();
            return true;
        }

        // Frustum Culling
        struct RenderChunk
        {
            std::map<int, std::unordered_set<const GameObject*>> objects;
        };
        struct ChunkCoordHasher
        {
            size_t operator()(const GameObject::ChunkCoord& c) const noexcept
            {
                const size_t h1 = std::hash<int>{}(c.x);
                const size_t h2 = std::hash<int>{}(c.y);
                return h1 ^ (h2 << 1);
            }
        };
        static constexpr float CHUNK_SIZE = 512.f; // tune later
        std::unordered_map<GameObject::ChunkCoord, RenderChunk, ChunkCoordHasher> m_RenderChunks;
        GameObject::ChunkCoord WorldToChunk(const sf::Vector2f& pos) const { return GameObject::ChunkCoord{.x= static_cast<int>(std::floor(pos.x / CHUNK_SIZE)), .y= static_cast<int>(std::floor(pos.y / CHUNK_SIZE))};}
        void RegisterToChunk(const GameObject* object);
        std::pair<GameObject::ChunkCoord, GameObject::ChunkCoord> GetVisibleChunkRange(const sf::View& view) const;
        void RemoveFromChunk(const std::unordered_map<std::string, std::unique_ptr<GameObject>>::iterator& it);
        void UpdateGameObjectRenderLayerInChunk(const GameObject* gameObject, GameObject::RenderLayer oldLayer);
    };
}
