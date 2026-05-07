#include "Scene.h"
#include "Engine.h"
#include "../Collision/Collider.h"
#include "../Components/Camera.h"

#include <format>
#include <ranges>
#include <stdexcept>

#include "../Singleton/Helpers.h"

diji::Scene::~Scene() noexcept
{
    m_ObjectsUPtrMap.clear();
    m_CanvasObjectsUPtrMap.clear();
}

void diji::Scene::Init()
{
    for (const auto& gameObject : m_ObjectsUPtrMap | std::views::values)
    {
        gameObject->Init();
    }

    for (const auto& gameObject : m_CanvasObjectsUPtrMap | std::views::values)
    {
        gameObject->Init();
    }

    for (const auto& gameObject : m_RenderOnTopObjectsUPtrMap | std::views::values)
    {
        gameObject->Init();
    }

    if (m_StaticBackgroundObjUPtr)
        m_StaticBackgroundObjUPtr->Init();
}

void diji::Scene::Start()
{
    for (const auto& gameObject : m_ObjectsUPtrMap | std::views::values)
    {
        gameObject->Start();
    }

    for (const auto& gameObject : m_CanvasObjectsUPtrMap | std::views::values)
    {
        gameObject->Start();
    }

    for (const auto& gameObject : m_RenderOnTopObjectsUPtrMap | std::views::values)
    {
        gameObject->Start();
    }

    if (m_StaticBackgroundObjUPtr)
        m_StaticBackgroundObjUPtr->Start();

    m_MainCameraCompPtr = m_MainCameraObjPtr ? m_MainCameraObjPtr->GetComponent<Camera>() : nullptr;
    m_MainCameraViewCopy = m_MainCameraCompPtr ? m_MainCameraCompPtr->GetCameraView() : sf::View{};
    m_RenderBackground = m_StaticBackgroundObjUPtr && m_MainCameraCompPtr;
}

void diji::Scene::FixedUpdate()
{
    for (const auto& gameObject : m_ObjectsUPtrMap | std::views::values)
    {
        gameObject->FixedUpdate();
    }

    for (const auto& gameObject : m_CanvasObjectsUPtrMap | std::views::values)
    {
        gameObject->FixedUpdate();
    }

    for (const auto& gameObject : m_RenderOnTopObjectsUPtrMap | std::views::values)
    {
        gameObject->FixedUpdate();
    }
}

void diji::Scene::Update()
{
    for(const auto& gameObject : m_ObjectsUPtrMap | std::views::values)
    {
        gameObject->Update();
    }

    for (const auto& gameObject : m_CanvasObjectsUPtrMap | std::views::values)
    {
        gameObject->Update();
    }

    for (const auto& gameObject : m_RenderOnTopObjectsUPtrMap | std::views::values)
    {
        gameObject->Update();
    }
}

void diji::Scene::LateUpdate()
{
    for (const auto& gameObject : m_ObjectsUPtrMap | std::views::values)
    {
        gameObject->LateUpdate();
    }

    for (const auto& gameObject : m_CanvasObjectsUPtrMap | std::views::values)
    {
        gameObject->LateUpdate();
    }

    for (const auto& gameObject : m_RenderOnTopObjectsUPtrMap | std::views::values)
    {
        gameObject->LateUpdate();
    }

    for (auto& view : m_MultiplayerViews)
    {
        view.Update();
    }
}

void diji::Scene::Render() const
{
    if (m_IsUsingMultiplayerViews)
    {
        RenderMultiplayerViews();
    }
    else
    {
        if (m_RenderBackground)
        {
            window::g_window_ptr->setView(m_MainCameraViewCopy);
            m_StaticBackgroundObjUPtr->Render();
        }
        
        if (m_MainCameraCompPtr) // todo: what happens if there is no camera?
            window::g_window_ptr->setView(m_MainCameraCompPtr->GetCameraView());
        
        DrawGameObjects();
    }

    window::g_window_ptr->setView(m_CanvasView);
    for (const auto& gameObject : m_CanvasObjectsUPtrMap | std::views::values)
    {
        gameObject->Render();
    }

    if (m_IsUsingMultiplayerViews)
    {
        RenderObjectOnTopMultiplayerViews();
    }
    else
    {
        window::g_window_ptr->setView(m_MainCameraCompPtr->GetCameraView());
        for (const auto& gameObject : m_RenderOnTopObjectsUPtrMap | std::views::values)
        {
            gameObject->Render();
        }
    }
}

void diji::Scene::RenderMultiplayerViews() const
{
    for (size_t i = 0; i < m_MultiplayerViews.size(); ++i)
    {
        // Set background view
        if (m_StaticBackgroundObjUPtr)
        {
            window::g_window_ptr->setView(m_MultiplayerViewsCopy[i].GetView());
            m_StaticBackgroundObjUPtr->Render();
        }

        // Set gameplay view
        window::g_window_ptr->setView(m_MultiplayerViews[i].GetView());
        DrawGameObjects();
    }
}

void diji::Scene::RenderObjectOnTopMultiplayerViews() const
{
    for (size_t i = 0; i < m_MultiplayerViews.size(); ++i)
    {
        window::g_window_ptr->setView(m_MultiplayerViews[i].GetView());
        for (const auto& gameObject : m_RenderOnTopObjectsUPtrMap | std::views::values)
        {
            gameObject->Render();
        }
    }
}


void diji::Scene::OnDestroy()
{
    // Clear listeners from objects in this scene and free the memory used by the events
    EventRegistry::GetInstance().ClearAllEvents();
    
    for (const auto& gameObject : m_ObjectsUPtrMap | std::views::values)
    {
        gameObject->OnDestroy();
    }

    for (const auto& gameObject : m_CanvasObjectsUPtrMap | std::views::values)
    {
        gameObject->OnDestroy();
    }

    for (const auto& gameObject : m_RenderOnTopObjectsUPtrMap | std::views::values)
    {
        gameObject->OnDestroy();
    }

    m_ObjectsUPtrMap = std::map<std::string, std::unique_ptr<GameObject>>();
    m_CanvasObjectsUPtrMap = std::map<std::string, std::unique_ptr<GameObject>>();
    m_RenderOnTopObjectsUPtrMap = std::map<std::string, std::unique_ptr<GameObject>>();
}

diji::GameObject* diji::Scene::CreateCameraObject(const std::string& name)
{
    m_MainCameraObjPtr = CreateGameObject(name);

    return m_MainCameraObjPtr;
}

diji::GameObject* diji::Scene::CreateGameObject(const std::string& name)
{
    if (m_ObjectsUPtrMap.contains(name))
        throw std::runtime_error("A GameObject with the given name already exists.");
	
    m_ObjectsUPtrMap[name] = std::make_unique<GameObject>();
    return m_ObjectsUPtrMap.at(name).get();
}

diji::GameObject* diji::Scene::CreateGameObject(const std::string& name, const GameObject* original)
{
    if (m_ObjectsUPtrMap.contains(name))
        throw std::runtime_error("A GameObject with the given name already exists.");

    m_ObjectsUPtrMap[name] = std::make_unique<GameObject>();
    original->CreateDuplicate(m_ObjectsUPtrMap.at(name).get());
    return m_ObjectsUPtrMap.at(name).get();
}

diji::GameObject* diji::Scene::CreateGameObjectFromTemplate(const std::string& name, const GameObject* original)
{
    const std::string finalName = GenerateUniqueName(m_ObjectsUPtrMap, name);

    m_ObjectsUPtrMap[finalName] = std::make_unique<GameObject>();
    original->CreateDuplicate(m_ObjectsUPtrMap[finalName].get());

    return m_ObjectsUPtrMap[finalName].get();
}

diji::GameObject* diji::Scene::CreateCanvasObjectFromTemplate(const std::string& name, const GameObject* original)
{
    const std::string finalName = GenerateUniqueName(m_CanvasObjectsUPtrMap, name);

    m_CanvasObjectsUPtrMap[finalName] = std::make_unique<GameObject>();
    original->CreateDuplicate(m_CanvasObjectsUPtrMap[finalName].get());

    return m_CanvasObjectsUPtrMap[finalName].get();
}

diji::GameObject* diji::Scene::AddObjectToScene(std::unique_ptr<GameObject> object, const std::string& desiredName)
{
    const std::string finalName = GenerateUniqueName(m_ObjectsUPtrMap, desiredName);

    m_ObjectsUPtrMap[finalName] = std::move(object);
    return m_ObjectsUPtrMap[finalName].get();
}

diji::GameObject* diji::Scene::AddObjectToCanvas(std::unique_ptr<GameObject> object, const std::string& desiredName)
{
    const std::string finalName = GenerateUniqueName(m_CanvasObjectsUPtrMap, desiredName);

    m_CanvasObjectsUPtrMap[finalName] = std::move(object);
    return m_CanvasObjectsUPtrMap[finalName].get();
}

diji::GameObject* diji::Scene::OverwriteGameObjectFromTemplate(const std::string& name, const GameObject* original)
{
    if (m_ObjectsUPtrMap.contains(name))
        m_ObjectsUPtrMap[name]->OnDestroy();
    
    m_ObjectsUPtrMap[name] = std::make_unique<GameObject>();
    original->CreateDuplicate(m_ObjectsUPtrMap[name].get());

    return m_ObjectsUPtrMap[name].get();
}

diji::GameObject* diji::Scene::OverwriteCanvasObjectFromTemplate(const std::string& name, const GameObject* original)
{
    if (m_CanvasObjectsUPtrMap.contains(name))
        m_CanvasObjectsUPtrMap[name]->OnDestroy();

    m_CanvasObjectsUPtrMap[name] = std::make_unique<GameObject>();
    original->CreateDuplicate(m_CanvasObjectsUPtrMap[name].get());

    return m_CanvasObjectsUPtrMap[name].get();
}

diji::GameObject* diji::Scene::OverwriteObjectInScene(std::unique_ptr<GameObject> object, const std::string& name)
{
    if (m_ObjectsUPtrMap.contains(name))
        m_ObjectsUPtrMap[name]->OnDestroy();

    m_ObjectsUPtrMap[name] = std::move(object);
    return m_ObjectsUPtrMap[name].get();
}

diji::GameObject* diji::Scene::OverwriteObjectInCanvas(std::unique_ptr<GameObject> object, const std::string& name)
{
    if (m_CanvasObjectsUPtrMap.contains(name))
        m_CanvasObjectsUPtrMap[name]->OnDestroy();

    m_CanvasObjectsUPtrMap[name] = std::move(object);
    return m_CanvasObjectsUPtrMap[name].get();
}

void diji::Scene::Remove(const GameObject* object)
{
    for (auto it = m_ObjectsUPtrMap.begin(); it != m_ObjectsUPtrMap.end(); ++it)
    {
        // Safe destruction pattern to avoid double deletion
        if (it->second.get() == object)
        {
            const std::unique_ptr<GameObject> localUp = std::move(it->second);
            m_ObjectsUPtrMap.erase(it);

            localUp->OnDestroy();
            return;
        }
    }

    for (auto it = m_CanvasObjectsUPtrMap.begin(); it != m_CanvasObjectsUPtrMap.end(); ++it)
    {
        if (it->second.get() == object)
        {
            const std::unique_ptr<GameObject> localUp = std::move(it->second);
            m_CanvasObjectsUPtrMap.erase(it);

            localUp->OnDestroy();
            return;
        }
    }

    for (auto it = m_RenderOnTopObjectsUPtrMap.begin(); it != m_RenderOnTopObjectsUPtrMap.end(); ++it)
    {
        if (it->second.get() == object)
        {
            const std::unique_ptr<GameObject> localUp = std::move(it->second);
            m_RenderOnTopObjectsUPtrMap.erase(it);

            localUp->OnDestroy();
            return;
        }
    }
}

void diji::Scene::Remove(const std::string& name) // todo: add canvas and render on top versions ?
{
    const auto it = m_ObjectsUPtrMap.find(name);
    if (it != m_ObjectsUPtrMap.end())
    {
        m_ObjectsUPtrMap.erase(it);
    }
}

void diji::Scene::RemoveAll()
{
    m_ObjectsUPtrMap = std::map<std::string, std::unique_ptr<GameObject>>();
    m_RenderOnTopObjectsUPtrMap = std::map<std::string, std::unique_ptr<GameObject>>();
}

diji::GameObject* diji::Scene::GetGameObject(const std::string& name) const // todo: add canvas and render on top versions ?
{
    const auto it = m_ObjectsUPtrMap.find(name);
	
    return it != m_ObjectsUPtrMap.end() ? it->second.get() : nullptr;
}

std::string diji::Scene::GetGameObjectName(const GameObject* object) const
{
    for (const auto& [name, gameObject] : m_ObjectsUPtrMap)
    {
        if (gameObject.get() == object)
        {
            return name;
        }
    }
    throw std::runtime_error("GameObject not found in the scene.");
}

void diji::Scene::ChangeViewCenter(const int idx, const sf::Vector2f& newCenter)
{
    m_MultiplayerViews.at(idx).SetCenter(newCenter);
}

void diji::Scene::SetViewParameters(const int idx, Transform* target, const bool isFollowing, const sf::Vector2f& offset)
{
    m_MultiplayerViews.at(idx).SetTargetTransform(target);
    m_MultiplayerViews.at(idx).SetIsFollowing(isFollowing);
    m_MultiplayerViews.at(idx).SetOffset(offset);
}

sf::Vector2i diji::Scene::GetScreenPosition(const sf::Vector2f& worldCoords) const
{
    if (m_MainCameraCompPtr)
        return window::g_window_ptr->mapCoordsToPixel(worldCoords, m_MainCameraCompPtr->GetCameraView());

    return window::g_window_ptr->mapCoordsToPixel(worldCoords); // Use default view
}

sf::Vector2f diji::Scene::GetWorldPositionFromScreen(const sf::Vector2i& screenCoords) const
{
    if (m_MainCameraCompPtr)
        return window::g_window_ptr->mapPixelToCoords(screenCoords, m_MainCameraCompPtr->GetCameraView());

    return window::g_window_ptr->mapPixelToCoords(screenCoords); // Use default view
}

void diji::Scene::SetGameObjectAsCanvasObject(const std::string& name)
{
    const auto it = m_ObjectsUPtrMap.find(name);
    if (it != m_ObjectsUPtrMap.end())
    {
        m_CanvasObjectsUPtrMap[name] = std::move(it->second);
        m_ObjectsUPtrMap.erase(it);
        return;
    }

    const auto it2 = m_RenderOnTopObjectsUPtrMap.find(name);
    if (it2 != m_RenderOnTopObjectsUPtrMap.end())
    {
        m_CanvasObjectsUPtrMap[name] = std::move(it2->second);
        m_RenderOnTopObjectsUPtrMap.erase(it2);
        return;
    }
    
    throw std::runtime_error("GameObject with the given name does not exist in the scene.");
}

void diji::Scene::SetGameObjectAsCanvasObject(const GameObject* object)
{
    for (auto it = m_ObjectsUPtrMap.begin(); it != m_ObjectsUPtrMap.end(); ++it)
    {
        if (it->second.get() == object)
        {
            m_CanvasObjectsUPtrMap[it->first] = std::move(it->second);
            m_ObjectsUPtrMap.erase(it);
            return;
        }
    }

    for (auto it = m_RenderOnTopObjectsUPtrMap.begin(); it != m_RenderOnTopObjectsUPtrMap.end(); ++it)
    {
        if (it->second.get() == object)
        {
            m_CanvasObjectsUPtrMap[it->first] = std::move(it->second);
            m_RenderOnTopObjectsUPtrMap.erase(it);
            return;
        }
    }

    throw std::runtime_error("GameObject does not exist in the scene.");
}

void diji::Scene::SetGameObjectToRenderOnTop(const std::string& name)
{
    const auto it = m_ObjectsUPtrMap.find(name);
    if (it != m_ObjectsUPtrMap.end())
    {
        m_RenderOnTopObjectsUPtrMap[name] = std::move(it->second);
        m_ObjectsUPtrMap.erase(it);
    }
    else
        throw std::runtime_error("GameObject with the given name does not exist in the scene.");
}

void diji::Scene::SetGameObjectToRenderOnTop(const GameObject* object)
{
    for (auto it = m_ObjectsUPtrMap.begin(); it != m_ObjectsUPtrMap.end(); ++it)
    {
        if (it->second.get() == object)
        {
            m_RenderOnTopObjectsUPtrMap[it->first] = std::move(it->second);
            m_ObjectsUPtrMap.erase(it);
            return;
        }
    }
}

void diji::Scene::SetMultiplayerSplitScreen(const int numPlayers)
{
    m_MultiplayerViews = std::vector<SplitScreenView>();
    m_IsUsingMultiplayerViews = true;

    switch (numPlayers)
    {
    case 1:
        m_IsUsingMultiplayerViews = false;
        break;
    case 2:
        {
            // Two players: left and right
            SplitScreenView leftView    (sf::FloatRect(sf::Vector2{ 0.f, 0.f  }, sf::Vector2{ 0.5f, 1.f }));
            SplitScreenView rightView   (sf::FloatRect(sf::Vector2{ 0.5f, 0.f }, sf::Vector2{ 0.5f, 1.f }));

            m_MultiplayerViews.emplace_back(leftView);
            m_MultiplayerViews.emplace_back(rightView);
            break;
        }
    case 3:
        {
            // Three players: two top, one bottom (wider)
            SplitScreenView topLeft (sf::FloatRect(sf::Vector2{ 0.f, 0.f  }, sf::Vector2{ 0.5f, 0.5f }));
            SplitScreenView topRight(sf::FloatRect(sf::Vector2{ 0.5f, 0.f }, sf::Vector2{ 0.5f, 0.5f }));
            SplitScreenView bottom  (sf::FloatRect(sf::Vector2{ 0.f, 0.5f }, sf::Vector2{ 1.f, 0.5f  }));

            m_MultiplayerViews.emplace_back(topLeft);
            m_MultiplayerViews.emplace_back(topRight);
            m_MultiplayerViews.emplace_back(bottom);
            break;
        }
    case 4:
        {
            // Four players: 2x2 grid
            SplitScreenView topLeft     (sf::FloatRect(sf::Vector2{ 0.f, 0.f   }, sf::Vector2{ 0.5f, 0.5f }));
            SplitScreenView topRight    (sf::FloatRect(sf::Vector2{ 0.5f, 0.f  }, sf::Vector2{ 0.5f, 0.5f }));
            SplitScreenView bottomLeft  (sf::FloatRect(sf::Vector2{ 0.f, 0.5f  }, sf::Vector2{ 0.5f, 0.5f }));
            SplitScreenView bottomRight (sf::FloatRect(sf::Vector2{ 0.5f, 0.5f }, sf::Vector2{ 0.5f, 0.5f }));

            m_MultiplayerViews.emplace_back(topLeft);
            m_MultiplayerViews.emplace_back(topRight);
            m_MultiplayerViews.emplace_back(bottomLeft);
            m_MultiplayerViews.emplace_back(bottomRight);
            break;
        }
    default:
        throw std::invalid_argument("Invalid number of players. Must be 2, 3, or 4.");
    }

    m_MultiplayerViewsCopy = m_MultiplayerViews;
}

void diji::Scene::SetGameObjectAsStaticBackground(const std::string& name)
{
    const auto it = m_ObjectsUPtrMap.find(name);
    if (it != m_ObjectsUPtrMap.end())
    {
        m_StaticBackgroundObjUPtr = std::move(it->second);
        m_ObjectsUPtrMap.erase(it);
    }
    else
        throw std::runtime_error("GameObject with the given name does not exist in the scene.");
}

void diji::Scene::SetGameObjectAsStaticBackground(const GameObject* object)
{
    for (auto it = m_ObjectsUPtrMap.begin(); it != m_ObjectsUPtrMap.end(); ++it)
    {
        if (it->second.get() == object)
        {
            m_StaticBackgroundObjUPtr = std::move(it->second);
            m_ObjectsUPtrMap.erase(it);
            return;
        }
    }

    throw std::runtime_error("GameObject does not exist in the scene.");
}

void diji::Scene::ValidateCollidersAfterDestroy()
{
    // todo: multi-thread?
    for (const auto& gameObject : m_ObjectsUPtrMap | std::views::values)
    {
        if (!gameObject->HasComponent<Collider>()) continue;

        gameObject->GetComponent<Collider>()->ValidateColliderLists();
    }

    for (const auto& gameObject : m_RenderOnTopObjectsUPtrMap | std::views::values)
    {
        if (!gameObject->HasComponent<Collider>()) continue;

        gameObject->GetComponent<Collider>()->ValidateColliderLists();
    }
}

void diji::Scene::DrawGameObjects() const
{
    for (const auto& gameObject : m_ObjectsUPtrMap | std::views::values)
    {
        gameObject->Render();
    }
}

//// Attempt at frustum culling is in fact not faster.
// void diji::Scene::DrawGameObjects() const
// {
//     if (!m_MainCameraCompPtr) return;
//
//     const sf::View& camView = m_MainCameraCompPtr->GetCameraView();
//     const sf::FloatRect cameraRect(sf::Vector2f{ camView.getCenter().x - camView.getSize().x * 0.5f, camView.getCenter().y - camView.getSize().y * 0.5f }, sf::Vector2f{ camView.getSize().x, camView.getSize().y });
//
//     for (const auto& gameObject : m_ObjectsUPtrMap | std::views::values)
//     {
//         if (auto bounds = gameObject->GetBoundingBox())
//             if (Helpers::RectsOverlap(cameraRect, *bounds))
//                 gameObject->Render();
//     }
// }

std::string diji::Scene::GenerateUniqueName(const std::map<std::string, std::unique_ptr<GameObject>>& objectMap, const std::string& baseName)
{
    size_t pos = baseName.size();
    while (pos > 0 && std::isdigit(static_cast<unsigned char>(baseName[pos - 1])))
        --pos;

    const std::string strippedName = baseName.substr(0, pos);
    const bool baseHasNumber = pos < baseName.size();
    unsigned long long parsedSuffix = 0;

    if (baseHasNumber)
    {
        const auto begin = baseName.data() + pos;
        const auto end   = baseName.data() + baseName.size();
        auto [charPtr, errc] = std::from_chars(begin, end, parsedSuffix);
        if (errc != std::errc())
            throw std::invalid_argument("what the fuck did you do cuh?");
    }

    auto &idx = m_NameIndexUMap[strippedName];

    if (!objectMap.contains(baseName))
    {
        idx = baseHasNumber ? parsedSuffix + 1ULL : 1ULL;
        return baseName;
    }
    
    if (idx == 0)
        idx = baseHasNumber ? parsedSuffix + 1ULL : 1ULL;
    
    std::string finalName;
    finalName.reserve(strippedName.size() + 32);

    for (;; ++idx)
    {
        finalName = strippedName;
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), idx);
        finalName.append(buf, ptr - buf);

        if (!objectMap.contains(finalName))
        {
            m_NameIndexUMap[strippedName] = idx + 1ULL;
            return finalName;
        }
    }
}
