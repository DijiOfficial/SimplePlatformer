#include "Selector.h"

#include "../../Singletons/LevelEditorManager.h"

void superMarioBros::Selector::Init()
{
    LevelEditorManager::GetInstance().Init();
}
