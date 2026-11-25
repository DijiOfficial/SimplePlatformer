#pragma once
#include <cstdint>
#include <functional>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <SFML/Graphics/Rect.hpp>

namespace diji
{
    class GameObject;
    enum class Directions;
}

namespace superMarioBros
{
    enum class WarpExitType : std::uint8_t;

    class WorldBuilder final
    {
    public:
        struct WorldSettings
        {
            static constexpr int MAX_LEVEL_HEIGHT = 14;
            static constexpr float TILE_SIZE = 50.0f;
            static constexpr sf::FloatRect ARENA_SIZE = { sf::Vector2f{ 0.0f, -(TILE_SIZE * MAX_LEVEL_HEIGHT * 0.5f + 100.0f) }, sf::Vector2f{ 12000.0f, 1080.0f } }; // 100 is the custom offset
            static constexpr std::string_view GROUND_MERGE_STOP_TILES = "H";
        };

        static void Init();
        static diji::GameObject* CreateWorld(const std::vector<char>& levelInfo, const int& rows, const int& cols, int colOffset = 0);
        [[nodiscard]] static const diji::GameObject* CreateTileObject(const diji::GameObject* world, int row, int col, char tile);
        static void SpawnWarpEntrance(const diji::GameObject* world, const sf::Vector2i& cell, diji::Directions direction, int colOffset = 0);
        static void SpawnWarpArrival(const diji::GameObject* world, const sf::Vector2i& cell, WarpExitType exitType, int colOffset = 0);
        [[nodiscard]] static sf::Vector2f GetWarpArrivalPosition(const sf::Vector2i& cell, WarpExitType exitType, int colOffset = 0);

    private:
        using TileHandler = std::function<int(const diji::GameObject* world, int row, int col, char tile)>;
        inline static std::unordered_map<char, TileHandler> m_Handlers;

        static void AttachToWorldObject(const diji::GameObject* world, const diji::GameObject* object);
        static void SpawnForegroundTile(const diji::GameObject* world, int row, int col, const sf::Vector2i& atlasPos);
        static void SpawnVerticalPipeOverlay(const diji::GameObject* world, int row, int col);
        static void SpawnBonusAreaTrigger(const diji::GameObject* world, const sf::Vector2f& center, const sf::Vector2f& size, diji::Directions entranceDirection);
        inline static const diji::GameObject* m_LastGameObjectCreated = nullptr;
    };
}
