#pragma once
#include "GameObject.h"
#include <map>
#include <memory>
#include <string>
#include <SFML/Graphics/View.hpp>

#include "Engine.h"
#include "../RAII_Wrappers/SplitScreenView.h"

namespace diji 
{
    class Camera;

    class Scene final
    {
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
        [[nodiscard]] GameObject* AddObjectToScene(std::unique_ptr<GameObject> object, const std::string& desiredName);
        void Remove(const GameObject* object);
        void Remove(const std::string& name);
        void RemoveAll();
        [[nodiscard]] GameObject* GetGameObject(const std::string& name) const;
        [[nodiscard]] std::string GetGameObjectName(const GameObject* object) const;
        void ChangeViewCenter(int idx, const sf::Vector2f& newCenter);
        void SetViewParameters(int idx, const Transform* target, bool isFollowing = false, const sf::Vector2f& offset = {});

        void SetGameObjectAsCanvasObject(const std::string& name);
        void SetGameObjectAsCanvasObject(const GameObject* object);
        void SetCanvasView(const sf::View& view) { m_CanvasView = view; }

        void SetMultiplayerSplitScreen(int numPlayers);
        void SetGameObjectAsStaticBackground(const std::string& name);
        void SetGameObjectAsStaticBackground(const GameObject* object);
        
    private:
        std::map<std::string, std::unique_ptr<GameObject>> m_ObjectsUPtrMap;
        std::map<std::string, std::unique_ptr<GameObject>> m_CanvasObjectsUPtrMap;
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
        std::string GenerateUniqueName(const std::string& baseName) const;
    };
}
