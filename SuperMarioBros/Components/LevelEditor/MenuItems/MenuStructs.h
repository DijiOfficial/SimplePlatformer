#pragma once
#include <string>
#include <SFML/System/Vector2.hpp>

namespace superMarioBros
{
    enum class eMenuType : std::uint8_t
    {
        None = 0,
        Load = 1,  
        Save = 2 ,
    };
    
    struct LoadingLevelData
    {
        sf::Vector2f Position;
        std::string Name;
    };
}
