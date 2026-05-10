#pragma once
#include <vector>

namespace diji
{
    class GameObject;
}

namespace superMarioBros
{
    class WorldBuilder final
    {
    public:
        static diji::GameObject* CreateWorld(const std::vector<char>& levelInfo, const int& rows, const int& cols);
    };
}
