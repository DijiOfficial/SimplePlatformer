#pragma once
#include <vector>

namespace superMarioBros
{
    class WorldBuilder final
    {
    public:
        static void CreateWorld(const std::vector<char>& levelInfo, const int& rows, const int& cols);
    };
}
