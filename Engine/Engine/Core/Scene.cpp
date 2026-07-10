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
    for (const auto& gameObjects : m_CanvasRenderMap | std::views::values)
    {
        for (const auto& gameObject : gameObjects)
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

    m_ObjectsUPtrMap = std::unordered_map<std::string, std::unique_ptr<GameObject>>();
    m_CanvasObjectsUPtrMap = std::unordered_map<std::string, std::unique_ptr<GameObject>>();
    m_CanvasRenderMap = std::map<int, std::unordered_set<const GameObject*>>();
    m_RenderChunks = std::unordered_map<GameObject::ChunkCoord, RenderChunk, ChunkCoordHasher>();
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
    m_ObjectsUPtrMap[name]->SetName(name);
    return m_ObjectsUPtrMap.at(name).get();
}

diji::GameObject* diji::Scene::CreateGameObject(const std::string& name, const GameObject* original)
{
    if (m_ObjectsUPtrMap.contains(name))
        throw std::runtime_error("A GameObject with the given name already exists.");

    m_ObjectsUPtrMap[name] = std::make_unique<GameObject>();
    original->CreateDuplicate(m_ObjectsUPtrMap.at(name).get());
    m_ObjectsUPtrMap[name]->SetName(name);
    return m_ObjectsUPtrMap.at(name).get();
}

diji::GameObject* diji::Scene::CreateGameObjectFromTemplate(const std::string& name, const GameObject* original)
{
    const std::string finalName = GenerateUniqueName(m_ObjectsUPtrMap, name);

    m_ObjectsUPtrMap[finalName] = std::make_unique<GameObject>();
    original->CreateDuplicate(m_ObjectsUPtrMap[finalName].get());
    m_ObjectsUPtrMap[finalName]->SetName(finalName);

    return m_ObjectsUPtrMap[finalName].get();
}

diji::GameObject* diji::Scene::CreateCanvasObjectFromTemplate(const std::string& name, const GameObject* original)
{
    const std::string finalName = GenerateUniqueName(m_CanvasObjectsUPtrMap, name);

    m_CanvasObjectsUPtrMap[finalName] = std::make_unique<GameObject>();
    original->CreateDuplicate(m_CanvasObjectsUPtrMap[finalName].get());
    m_CanvasObjectsUPtrMap[finalName]->SetName(finalName);
    m_CanvasRenderMap[0].insert(m_CanvasObjectsUPtrMap[finalName].get());
    return m_CanvasObjectsUPtrMap[finalName].get();
}

diji::GameObject* diji::Scene::AddObjectToScene(std::unique_ptr<GameObject> object, const std::string& desiredName)
{
    const std::string finalName = GenerateUniqueName(m_ObjectsUPtrMap, desiredName);

    m_ObjectsUPtrMap[finalName] = std::move(object);
    m_ObjectsUPtrMap[finalName]->SetName(finalName);
    return m_ObjectsUPtrMap[finalName].get();
}

diji::GameObject* diji::Scene::AddObjectToCanvas(std::unique_ptr<GameObject> object, const std::string& desiredName)
{
    const std::string finalName = GenerateUniqueName(m_CanvasObjectsUPtrMap, desiredName);

    m_CanvasObjectsUPtrMap[finalName] = std::move(object);
    m_CanvasObjectsUPtrMap[finalName]->SetName(finalName);
    m_CanvasRenderMap[0].insert(m_CanvasObjectsUPtrMap[finalName].get());
    return m_CanvasObjectsUPtrMap[finalName].get();
}

diji::GameObject* diji::Scene::OverwriteGameObjectFromTemplate(const std::string& name, const GameObject* original)
{
    if (m_ObjectsUPtrMap.contains(name))
        m_ObjectsUPtrMap[name]->OnDestroy();
    
    m_ObjectsUPtrMap[name] = std::make_unique<GameObject>();
    original->CreateDuplicate(m_ObjectsUPtrMap[name].get());
    m_ObjectsUPtrMap[name]->SetName(name);

    return m_ObjectsUPtrMap[name].get();
}

diji::GameObject* diji::Scene::OverwriteCanvasObjectFromTemplate(const std::string& name, const GameObject* original)
{
    if (m_CanvasObjectsUPtrMap.contains(name))
    {
        m_CanvasRenderMap[m_CanvasObjectsUPtrMap[name]->GetRenderLayer()].erase(m_CanvasObjectsUPtrMap[name].get());
        m_CanvasObjectsUPtrMap[name]->OnDestroy();
    }

    m_CanvasObjectsUPtrMap[name] = std::make_unique<GameObject>();
    original->CreateDuplicate(m_CanvasObjectsUPtrMap[name].get());
    m_CanvasObjectsUPtrMap[name]->SetName(name);
    m_CanvasRenderMap[0].insert(m_CanvasObjectsUPtrMap[name].get());

    return m_CanvasObjectsUPtrMap[name].get();
}

diji::GameObject* diji::Scene::OverwriteObjectInScene(std::unique_ptr<GameObject> object, const std::string& name)
{
    if (m_ObjectsUPtrMap.contains(name))
        m_ObjectsUPtrMap[name]->OnDestroy();

    m_ObjectsUPtrMap[name] = std::move(object);
    m_ObjectsUPtrMap[name]->SetName(name);
    return m_ObjectsUPtrMap[name].get();
}

diji::GameObject* diji::Scene::OverwriteObjectInCanvas(std::unique_ptr<GameObject> object, const std::string& name)
{
    if (m_CanvasObjectsUPtrMap.contains(name))
    {
        m_CanvasRenderMap[m_CanvasObjectsUPtrMap[name]->GetRenderLayer()].erase(m_CanvasObjectsUPtrMap[name].get());
        m_CanvasObjectsUPtrMap[name]->OnDestroy();
    }

    m_CanvasObjectsUPtrMap[name] = std::move(object);
    m_CanvasObjectsUPtrMap[name]->SetName(name);
    m_CanvasRenderMap[0].insert(m_CanvasObjectsUPtrMap[name].get());
    return m_CanvasObjectsUPtrMap[name].get();
}

void diji::Scene::Remove(const GameObject* object)
{
    m_CanvasRenderMap[object->GetRenderLayer()].erase(object);
    if (RemoveFromContainer(m_ObjectsUPtrMap, object)) return;
    if (RemoveFromContainer(m_CanvasObjectsUPtrMap, object)) return;

#ifdef _DEBUG
    assert(false && "Attempted to remove unknown GameObject");
#endif
}

void diji::Scene::Remove(const std::string& name)
{
    // todo: add unified remove from map
    const auto it = m_ObjectsUPtrMap.find(name);
    if (it != m_ObjectsUPtrMap.end())
    {
        RemoveFromChunk(it);
        m_ObjectsUPtrMap.erase(it);
    }
}

void diji::Scene::RemoveAll()
{
    m_ObjectsUPtrMap = std::unordered_map<std::string, std::unique_ptr<GameObject>>();
    m_RenderChunks = std::unordered_map<GameObject::ChunkCoord, RenderChunk, ChunkCoordHasher>();
    m_CanvasObjectsUPtrMap = std::unordered_map<std::string, std::unique_ptr<GameObject>>();
    m_CanvasRenderMap = std::map<int, std::unordered_set<const GameObject*>>();
}

diji::GameObject* diji::Scene::GetGameObject(const std::string& name) const
{
    auto it = m_ObjectsUPtrMap.find(name);
    if (it != m_ObjectsUPtrMap.end())
        return it->second.get();

    it = m_CanvasObjectsUPtrMap.find(name);
    if (it != m_CanvasObjectsUPtrMap.end())
        return it->second.get();

    return nullptr;
}

std::string diji::Scene::GetGameObjectName(const GameObject* object) const // todo: add canvas and render on top versions ?
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
        RemoveFromChunk(it);
        m_CanvasObjectsUPtrMap[name] = std::move(it->second);
        m_CanvasRenderMap[m_CanvasObjectsUPtrMap[name]->GetRenderLayer()].insert(m_CanvasObjectsUPtrMap[name].get());
        m_ObjectsUPtrMap.erase(it);
        return;
    }

    throw std::runtime_error("GameObject with the given name does not exist in the scene.");
}

void diji::Scene::SetGameObjectAsCanvasObject(const GameObject* object)
{
    SetGameObjectAsCanvasObject(object->GetName());
}

void diji::Scene::SetCanvasObjectAsGameObject(const GameObject* object)
{
    const std::string& name = object->GetName();
    const auto it = m_CanvasObjectsUPtrMap.find(name);
    if (it != m_CanvasObjectsUPtrMap.end())
    {
        m_CanvasRenderMap[it->second->GetRenderLayer()].erase(it->second.get());
        m_ObjectsUPtrMap[name] = std::move(it->second);
        m_CanvasObjectsUPtrMap.erase(it);
        RegisterToChunk(m_ObjectsUPtrMap[name].get());
        return;
    }
    
    throw std::runtime_error("GameObject with the given name does not exist in the scene.");
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
        RemoveFromChunk(it);
        m_StaticBackgroundObjUPtr = std::move(it->second);
        m_ObjectsUPtrMap.erase(it);
    }
    else
        throw std::runtime_error("GameObject with the given name does not exist in the scene.");
}

void diji::Scene::SetGameObjectAsStaticBackground(const GameObject* object)
{
    SetGameObjectAsStaticBackground(object->GetName());
}

void diji::Scene::ValidateCollidersAfterDestroy()
{
    // todo: multi-thread?
    for (const auto& gameObject : m_ObjectsUPtrMap | std::views::values)
    {
        if (!gameObject->HasComponent<Collider>()) continue;

        gameObject->GetComponent<Collider>()->ValidateColliderLists();
    }
}

void diji::Scene::SetToAlwaysRender(const GameObject* object, const bool shouldAlwaysRender)
{
    if (shouldAlwaysRender)
    {
        const auto it = m_ObjectsUPtrMap.find(object->GetName());
        if (it != m_ObjectsUPtrMap.end())
        {
            RemoveFromChunk(it);
            m_AlwaysRender.insert(object);
        }
    }
    else
    {
        const auto it = m_AlwaysRender.find(object);
        if (it != m_AlwaysRender.end())
        {
            m_AlwaysRender.erase(object);
            RegisterToChunk(object);
        }
    }
}

void diji::Scene::SetMainCamera(const GameObject* cameraObject)
{
    m_MainCameraObjPtr = const_cast<GameObject*>(cameraObject);
    m_MainCameraCompPtr = m_MainCameraObjPtr->GetComponent<Camera>();
}

void diji::Scene::DrawGameObjects() const
{
    auto [min, max] = GetVisibleChunkRange(window::g_window_ptr->getView());
    min.x -= 1;
    min.y -= 1;
    max.x += 1;
    max.y += 1;

    for (const GameObject* gameObject : m_AlwaysRender)
    {
        gameObject->Render();
    }
    
    for (int y = min.y; y <= max.y; ++y)
    {
        for (int x = min.x; x <= max.x; ++x)
        {
            GameObject::ChunkCoord coord{ .x= x, .y= y};

            auto it = m_RenderChunks.find(coord);
            if (it == m_RenderChunks.end())
                continue;

            // Fucking rendering can update an object position and cause it to remove itself from the chunk
            const auto renderLayers = it->second.objects;
            for (const auto& objects : renderLayers | std::views::values)
            {
                for (const GameObject* obj : objects)
                    obj->Render();
            }
        }
    }
}

std::string diji::Scene::GenerateUniqueName(const std::unordered_map<std::string, std::unique_ptr<GameObject>>& objectMap, const std::string& baseName)
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

// todo: register using visual bounds and store in multiple chunks
void diji::Scene::RegisterToChunk(const GameObject* object)
{
    if (!object) return;

    const auto it = m_ObjectsUPtrMap.find(object->GetName());
    if (it == m_ObjectsUPtrMap.end())
        return;

    const sf::Vector2f pos = object->GetRootComponent()->GetWorldPosition();
    const GameObject::ChunkCoord coord = WorldToChunk(pos);

    const auto layer = object->GetRenderLayer();
    m_RenderChunks[object->GetChunkCoord()].objects[layer].erase(object);
    
    object->SetChunkCoord(coord);
    m_RenderChunks[coord].objects[layer].insert(object);
}

std::pair<diji::GameObject::ChunkCoord, diji::GameObject::ChunkCoord> diji::Scene::GetVisibleChunkRange(const sf::View& view) const
{
    const sf::Vector2f center = view.getCenter();
    const sf::Vector2f size = view.getSize();

    const sf::Vector2f topLeft{ center.x - size.x * 0.5f, center.y - size.y * 0.5f };
    const sf::Vector2f bottomRight{ center.x + size.x * 0.5f, center.y + size.y * 0.5f };

    GameObject::ChunkCoord min = WorldToChunk(topLeft);
    GameObject::ChunkCoord max = WorldToChunk(bottomRight);

    return { min, max };
}

void diji::Scene::RemoveFromChunk(const std::unordered_map<std::string, std::unique_ptr<GameObject>>::iterator& it)
{
    if (const auto renderChunkIt = m_RenderChunks.find(it->second->GetChunkCoord()); renderChunkIt != m_RenderChunks.end())
    {
        renderChunkIt->second.objects[it->second->GetRenderLayer()].erase(it->second.get());

        if (renderChunkIt->second.objects.empty())
            m_RenderChunks.erase(renderChunkIt);
    }
}

void diji::Scene::UpdateGameObjectRenderLayerInChunk(const GameObject* gameObject, const GameObject::RenderLayer oldLayer)
{
    const auto coords = gameObject->GetChunkCoord();
    m_RenderChunks[coords].objects[oldLayer].erase(gameObject);
    m_RenderChunks[coords].objects[gameObject->GetRenderLayer()].insert(gameObject);
}