#include "Scene.h"

#include <ranges>
#include <stdexcept>

#include "Engine.h"
#include "../Components/Camera.h"

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
    // update all colliders here


    
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
        
        if (m_MainCameraCompPtr)
            window::g_window_ptr->setView(m_MainCameraCompPtr->GetCameraView());
        
        DrawGameObjects();
    }

    window::g_window_ptr->setView(m_CanvasView);
    for (const auto& gameObject : m_CanvasObjectsUPtrMap | std::views::values)
    {
        gameObject->Render();
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
    for (const auto& gameObject : m_ObjectsUPtrMap | std::views::values)
    {
        gameObject->OnDestroy();
    }

    for (const auto& gameObject : m_CanvasObjectsUPtrMap | std::views::values)
    {
        gameObject->OnDestroy();
    }

    m_ObjectsUPtrMap = std::map<std::string, std::unique_ptr<GameObject>>();
    m_CanvasObjectsUPtrMap = std::map<std::string, std::unique_ptr<GameObject>>();
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
    const std::string finalName = GenerateUniqueName(name);

    m_ObjectsUPtrMap[finalName] = std::make_unique<GameObject>();
    original->CreateDuplicate(m_ObjectsUPtrMap[finalName].get());

    return m_ObjectsUPtrMap[finalName].get();
}

diji::GameObject* diji::Scene::AddObjectToScene(std::unique_ptr<GameObject> object, const std::string& desiredName)
{
    const std::string finalName = GenerateUniqueName(desiredName);

    m_ObjectsUPtrMap[finalName] = std::move(object);
    return m_ObjectsUPtrMap[finalName].get();
}

void diji::Scene::Remove(const GameObject* object)
{
    object->OnDestroy();
    
    for (auto it = m_ObjectsUPtrMap.begin(); it != m_ObjectsUPtrMap.end(); ++it)
    {
        if (it->second.get() == object)
        {
            m_ObjectsUPtrMap.erase(it);
            break;
        }
    }
}

void diji::Scene::Remove(const std::string& name)
{
    auto it = m_ObjectsUPtrMap.find(name);
    if (it != m_ObjectsUPtrMap.end())
    {
        m_ObjectsUPtrMap.erase(it);
    }
}

void diji::Scene::RemoveAll()
{
    m_ObjectsUPtrMap = std::map<std::string, std::unique_ptr<GameObject>>();
}

diji::GameObject* diji::Scene::GetGameObject(const std::string& name) const
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

void diji::Scene::SetViewParameters(const int idx, const Transform* target, const bool isFollowing, const sf::Vector2f& offset)
{
    m_MultiplayerViews.at(idx).SetTargetTransform(target);
    m_MultiplayerViews.at(idx).SetIsFollowing(isFollowing);
    m_MultiplayerViews.at(idx).SetOffset(offset);
}

void diji::Scene::SetGameObjectAsCanvasObject(const std::string& name)
{
    const auto it = m_ObjectsUPtrMap.find(name);
    if (it != m_ObjectsUPtrMap.end())
    {
        m_CanvasObjectsUPtrMap[name] = std::move(it->second);
        m_ObjectsUPtrMap.erase(it);
    }
    else
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

    throw std::runtime_error("GameObject does not exist in the scene.");
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

void diji::Scene::DrawGameObjects() const
{
    for (const auto& gameObject : m_ObjectsUPtrMap | std::views::values)
    {
        gameObject->Render();
    }
}

std::string diji::Scene::GenerateUniqueName(const std::string& baseName) const
{
    std::string finalName = baseName;

    // Check if name already exists, if it does get the suffix and increment it until you get new name
    // todo: Likely some optimization can be done here. Like keeping track of the last used suffix or internal counter or ...
    while (m_ObjectsUPtrMap.contains(finalName))
    {
        // Find numeric suffix
        size_t pos = finalName.size();
        while (pos > 0 && std::isdigit(static_cast<unsigned char>(finalName[pos - 1])))
            --pos;

        if (pos == finalName.size()) // No number suffix so append 0
        {
            finalName += "0";
        }
        else
        {
            // Increment existing number suffix
            std::string namePart = finalName.substr(0, pos);
            const int number = std::stoi(finalName.substr(pos));
            finalName = namePart + std::to_string(number + 1);
        }
    }

    return finalName;
}