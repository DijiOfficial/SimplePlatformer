#pragma once
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <numbers>

namespace diji
{
    class Helpers final
    {
    public:
        static constexpr double PI = std::numbers::pi;

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

        static float Distance(const sf::Vector2f& a, const sf::Vector2f& b)
        {
            const float dx = b.x - a.x;
            const float dy = b.y - a.y;
            return std::sqrt(dx * dx + dy * dy);
        }

        static float DistanceSquared(const sf::Vector2f& a, const sf::Vector2f& b)
        {
            const float dx = b.x - a.x;
            const float dy = b.y - a.y;
            return dx * dx + dy * dy;
        }

        template <typename T>
        static T Length(const sf::Vector2<T>& v)
        {
            return static_cast<T>(std::hypot(static_cast<double>(v.x), static_cast<double>(v.y)));
        }

        static float LengthFast(const sf::Vector2f& v)
        {
            return std::sqrt(v.x * v.x + v.y * v.y);
        }

        static float LengthSquared(const sf::Vector2f& v)
        {
            return v.x * v.x + v.y * v.y;
        }
        
        template<typename T>
        constexpr T DegToRad(T degrees) noexcept
        {
            static_assert(std::is_floating_point_v<T>, "DegToRad requires a floating point type");
            return degrees * static_cast<T>(PI / 180.0);
        }

        template<typename T>
        constexpr T RadToDeg(T radians) noexcept
        {
            static_assert(std::is_floating_point_v<T>, "RadToDeg requires a floating point type");
            return radians * static_cast<T>(180.0 / PI);
        }

        // Convenience overloads for common types
        constexpr float DegToRadF(const float d) noexcept { return DegToRad<float>(d); }
        constexpr double DegToRadD(const double d) noexcept { return DegToRad<double>(d); }

        constexpr float RadToDegF(const float r) noexcept { return RadToDeg<float>(r); }
        constexpr double RadToDegD(const double r) noexcept { return RadToDeg<double>(r); }
    private:
        static float clamp01(const float v)
        {
            return v < 0.f ? 0.f : v > 1.f ? 1.f : v;
        }

    };
}
