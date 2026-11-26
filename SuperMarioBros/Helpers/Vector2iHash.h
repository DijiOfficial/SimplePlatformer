#pragma once
#include <cstddef>
#include <functional>
#include <SFML/System/Vector2.hpp>

namespace superMarioBros
{
    struct Vector2iHash
    {
        std::size_t operator()(const sf::Vector2i& v) const noexcept
        {
            const std::size_t h1 = std::hash<int>()(v.x);
            const std::size_t h2 = std::hash<int>()(v.y);
            return h1 ^ (h2 << 1);
        }
    };
}
