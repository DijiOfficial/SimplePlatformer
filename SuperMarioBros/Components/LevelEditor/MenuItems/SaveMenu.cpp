#include "SaveMenu.h"

#include "../../../Singletons/LevelEditorManager.h"

superMarioBros::MenuItem::MenuInfo superMarioBros::SaveMenu::ActivateMenu()
{
    // LevelEditorManager::GetInstance().SaveNewMap("NewTestLevel", "newTestLevel.txt");
    return MenuInfo{ .ShouldLockControls= false, .MenuType= eMenuType::Save, .Data= std::monostate{} };
}

void superMarioBros::SaveMenu::CloseMenu()
{
}
