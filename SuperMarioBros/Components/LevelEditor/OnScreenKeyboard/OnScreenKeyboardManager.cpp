#include "OnScreenKeyboardManager.h"

#include "OnScreenKey.h"
#include "Engine/Components/TextComp.h"
#include "Engine/Components/TextureComp.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Components/Render.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/SceneManager.h"

void onScreenKeyboard::OnScreenKeyboardManager::Init()
{
    CreateItemTemplate();
    m_KeyboardGrid = BuildKeyboard();
    LinkNeighbours();

    m_CurrentKeyPtr = m_KeyboardGrid[0][0];

    auto selection = std::make_unique<diji::GameObject>();
    const float renderRatio = (static_cast<float>(window::g_window_ptr->getSize().y) / 1080.0f) * 0.5f;
    selection->AddComponent<diji::TextureComp>("graphics/squareWhiteSmaller50.png");
    selection->AddComponent<diji::Render>(renderRatio);
    
    m_Selector = diji::SceneManager::GetInstance().AddGameObjectToCanvas("Z_UI_Selection2", std::move(selection), sf::Vector2f{ 0, 0 });
    m_Selector->AttachToObject(m_KeyboardRef, false);
    m_Selector->SetObjectPosition(m_CurrentKeyPtr->GetOwner()->GetObjectPosition());

    m_KeyboardRef->SetActive(false);
}

void onScreenKeyboard::OnScreenKeyboardManager::Move(const sf::Vector2f& direction) //todo add continous move
{
    if (direction.x > 0.f)
    {
        m_CurrentKeyPtr = m_CurrentKeyPtr->GetNeighbour(OnScreenKey::Direction::Right);
    }
    else if (direction.x < 0.f)
    {
        m_CurrentKeyPtr = m_CurrentKeyPtr->GetNeighbour(OnScreenKey::Direction::Left);
    }
    else if (direction.y > 0.f)
    {
        m_CurrentKeyPtr = m_CurrentKeyPtr->GetNeighbour(OnScreenKey::Direction::Down);
    }
    else if (direction.y < 0.f)
    {
        m_CurrentKeyPtr = m_CurrentKeyPtr->GetNeighbour(OnScreenKey::Direction::Up);
    }

    m_Selector->SetObjectPosition(m_CurrentKeyPtr->GetOwner()->GetObjectPosition());
}

char onScreenKeyboard::OnScreenKeyboardManager::ConfirmSelection() const
{
    return m_CurrentKeyPtr->PressKey();
}

void onScreenKeyboard::OnScreenKeyboardManager::Toggle() const
{
    m_KeyboardRef->SetActive(!m_KeyboardRef->IsActive());
}

void onScreenKeyboard::OnScreenKeyboardManager::CreateItemTemplate()
{
    m_KeyTemplateUPtr = std::make_unique<diji::GameObject>();
    m_KeyTemplateUPtr->AddComponent<diji::Render>();  // todo add render template
    m_KeyTemplateUPtr->AddComponent<diji::TextComp>("A", "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    m_KeyTemplateUPtr->AddComponent<OnScreenKey>();
}

onScreenKeyboard::OnScreenKeyboardManager::KeyGrid onScreenKeyboard::OnScreenKeyboardManager::BuildKeyboard()
{
    const auto& sceneManager = diji::SceneManager::GetInstance();
    m_KeyboardRef = sceneManager.AddGameObjectToCanvas("Z_OnScreenKeyboardObject", std::make_unique<diji::GameObject>(), sf::Vector2f{960, 540}); // todo: should be camera space

    KeyGrid grid;
    grid.resize(KEYBOARD_DATA.size());

    const size_t maxColumns = std::ranges::max(KEYBOARD_DATA, {}, &std::ranges::range_value_t<decltype(KEYBOARD_DATA)>::size).size();
    const float maxRowWidth = (maxColumns * KEY_WIDTH) + ((maxColumns - 1) * HORIZONTAL_SPACING);

    for(size_t y = 0; y < KEYBOARD_DATA.size(); y++)
    {
        const auto& layoutRow = KEYBOARD_DATA[y];
        const float currentRowWidth = (layoutRow.size() * KEY_WIDTH) + ((layoutRow.size() - 1) * HORIZONTAL_SPACING);
        const float rowOffsetX = (maxRowWidth - currentRowWidth) * 0.5f;

        grid[y].resize(layoutRow.size());
        for(size_t x = 0; x < layoutRow.size(); x++)
        {
            const sf::Vector2f position =
            {
                START_POSITION.x + rowOffsetX + x * (KEY_WIDTH + HORIZONTAL_SPACING),
                START_POSITION.y + y * (KEY_HEIGHT + VERTICAL_SPACING)
            };

            const auto& key = sceneManager.AddGameObjectToCanvas("A_OnScreenKeyboardKey", m_KeyTemplateUPtr.get(), position);

            grid[y][x] = key->GetComponent<OnScreenKey>();
            grid[y][x]->SetKey(layoutRow[x]);
            key->AttachToObject(m_KeyboardRef, false);
        }
    }

    return grid;
}

void onScreenKeyboard::OnScreenKeyboardManager::LinkNeighbours() const
{
    LinkHorizontal();
    LinkVertical();
}

void onScreenKeyboard::OnScreenKeyboardManager::LinkHorizontal() const
{
    for (const auto& row : m_KeyboardGrid)
    {
        const int size = static_cast<int>(row.size());
        if (size == 0)
            continue;

        for (int i = 0; i < size; ++i)
        {
            OnScreenKey* key = row[i];

            key->SetNeighbour(OnScreenKey::Direction::Left, row[Wrap(i - 1, size)]);
            key->SetNeighbour(OnScreenKey::Direction::Right, row[Wrap(i + 1, size)]);
        }
    }
}

void onScreenKeyboard::OnScreenKeyboardManager::LinkVertical() const
{
    const int rows = static_cast<int>(m_KeyboardGrid.size());
    if (rows == 0)
        return;

    for (int y = 0; y < rows; ++y)
    {
        const auto& row = m_KeyboardGrid[y];
        if (row.empty())
            continue;

        for (int x = 0; x < static_cast<int>(row.size()); ++x)
        {
            OnScreenKey* key = row[x];

            sf::Vector2f origin = key->GetOwner()->GetObjectPosition();

            const OnScreenKey* up = FindVerticalCandidate(y, -1, origin);
            const OnScreenKey* down = FindVerticalCandidate(y, +1, origin);

            key->SetNeighbour(OnScreenKey::Direction::Up, up);
            key->SetNeighbour(OnScreenKey::Direction::Down, down);
        }
    }
}

onScreenKeyboard::OnScreenKey* onScreenKeyboard::OnScreenKeyboardManager::FindVerticalCandidate(const int startRow, const int direction, const sf::Vector2f& origin) const
{
    const int rows = static_cast<int>(m_KeyboardGrid.size());

    for (int step = 1; step <= rows; ++step)
    {
        const int y = Wrap(startRow + step * direction, rows);

        const auto& row = m_KeyboardGrid[y];
        if (row.empty())
            continue;

        if (OnScreenKey* candidate = FindBestByPosition(row, origin))
            return candidate;
    }

    return nullptr;
}

onScreenKeyboard::OnScreenKey* onScreenKeyboard::OnScreenKeyboardManager::FindBestByPosition(const std::vector<OnScreenKey*>& row, const sf::Vector2f& origin) const
{
    const auto it = std::ranges::min_element(row,{},
        [&](const OnScreenKey* key)
        {
            const auto* owner = key->GetOwner();
            const sf::Vector2f& pos = owner->GetObjectPosition();
            return diji::Helpers::DistanceSquared(pos, origin);
        }
    );

    return (it != row.end()) ? *it : nullptr;
}