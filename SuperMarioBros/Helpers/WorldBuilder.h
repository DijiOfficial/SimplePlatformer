#pragma once
#include <functional>
#include <unordered_map>
#include <vector>

namespace diji
{
    class GameObject;
}

namespace superMarioBros
{
    static float TILE_SIZE = 50.f;

    class WorldBuilder final
    {
    public:
        static void Init();
        static diji::GameObject* CreateWorld(const std::vector<char>& levelInfo, const int& rows, const int& cols);
        static void CreateTileObject(const diji::GameObject* world, int row, int col, char tile);

    private:
        using TileHandler = std::function<int(const diji::GameObject* world, int row, int col, char tile)>;
        inline static std::unordered_map<char, TileHandler> m_Handlers;

        static void AttachToWorldObject(const diji::GameObject* world, const diji::GameObject* object);
    };
}
