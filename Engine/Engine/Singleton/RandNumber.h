#pragma once
#include <random>

namespace diji
{
    static thread_local std::mt19937 rng(std::random_device{}()); // cool thing I did not know, generating a device is slow and so only doing it once is good optimization

    class RandNumber final
    {
    public:
        static int GetRandomRangeInt(const int min, const int max)
        {
            std::uniform_int_distribution<int> dist(min, max);
            return dist(rng);
        }

        static float GetRandomRangeFloat(const float min, const float max)
        {
            std::uniform_real_distribution<float> dist(min, max);
            return dist(rng);
        }

        static sf::Vector2f GetRandomVector(const float v1Min, const float v1Max, const float v2Min, const float v2Max)
        {
            return { GetRandomRangeFloat(v1Min, v1Max), GetRandomRangeFloat(v2Min, v2Max) };
        }

        static sf::Vector2f GetRandomVector(const sf::Vector2f v1, const sf::Vector2f v2)
        {
            return { GetRandomRangeFloat(v1.x, v2.x), GetRandomRangeFloat(v1.y, v2.y) };
        }
        
        static sf::Vector2f GetRandomVector(const float min, const float max)
        {
            return { GetRandomRangeFloat(min, max), GetRandomRangeFloat(min, max) };
        }
    };
}
