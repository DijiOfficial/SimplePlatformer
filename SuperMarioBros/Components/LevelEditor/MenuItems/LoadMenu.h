#pragma once
#include "MenuItem.h"

#include <memory>
#include <string>

namespace diji
{
    class Transform;
}

namespace superMarioBros
{
    class LoadMenu final : public MenuItem
    {
    public:
        using MenuItem::MenuItem;

        void Init() override;
        void Start() override;
        //todo: listen to save level to update level names data
        
        [[nodiscard]] MenuInfo ActivateMenu() override;
        void CloseMenu() override;

    private:
        diji::Transform*  m_TransformCompPtr = nullptr;
        std::unique_ptr<diji::GameObject> m_ItemTemplateUPtr = nullptr;
        std::vector<std::string> m_LevelNames;
        std::vector<diji::GameObject*> m_LevelNameObjects;
        std::vector<LoadingLevelData> m_LevelData;
        
        const int ITEM_SPACING = 60;
        const int INITIAL_SPACING = 150;
        const int SELECTOR_SPACING = 20;
        bool m_MenuWasLoadedOnce = false;

        [[nodiscard]] MenuInfo LoadMenuItems();
        void CreateMenuItem(const std::string& levelName, float xPos, float yPos);
    };
}
