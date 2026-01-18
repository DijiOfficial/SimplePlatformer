#pragma once
#include <vector>

namespace superMarioBros
{
    class WorldBuilder
    {
    public:
        static void CreateWorld(const std::vector<char>& levelInfo, const int& rows, const int& cols);
    };
}
