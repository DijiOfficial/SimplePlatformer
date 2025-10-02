#pragma once
#include <SFML/System/Vector2.hpp>
#include <cmath>

namespace diji
{
    class Helpers final
    {
    public:
        static sf::Vector2f Normalize(const sf::Vector2f& v)
        {
            const float length = std::sqrt(v.x * v.x + v.y * v.y);
            if (length == 0.f) return sf::Vector2f{0.f, 0.f};
            return sf::Vector2f{v.x / length, v.y / length};
        }

        template<typename T>
        static T lerp(const T& a, const T& b, float alpha)
        {
            alpha = clamp01(alpha);
            return a + (b - a) * alpha;
        }

        static sf::Vector2f lerp(const sf::Vector2f& a, const sf::Vector2f& b, float alpha)
        {
            alpha = clamp01(alpha);
            return { lerp(a.x, b.x, alpha), lerp(a.y, b.y, alpha) };
        }

        static float DotProduct(const sf::Vector2f& a, const sf::Vector2f& b)
        {
            return a.x * b.x + a.y * b.y;
        }
        
    private:
        static float clamp01(const float v)
        {
            return v < 0.f ? 0.f : v > 1.f ? 1.f : v;
        }

    };
}
