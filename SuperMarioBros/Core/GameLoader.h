#pragma once

namespace diji
{
    class GameObject;
}

namespace SceneLoader
{
    void GameStartUp();
    void StartMenu();
    void Level();
    void LivesDisplayMenu();
    void GameOverMenu();
    void TransitionToNextLevel();
    void  LevelEditor();

    void AddPlayerComponents(diji::GameObject* player);
    void BindPlayerCommands(diji::GameObject* player);
    void BindLevelEditorCommands();
    void BindPlayTestCommands();
}
