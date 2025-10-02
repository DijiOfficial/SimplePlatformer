#pragma once
#include "Component.h"

#include <string>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace diji
{
    class Sprite final : public Component
    {
    public:
        explicit Sprite(GameObject* ownerPtr);
        explicit Sprite(GameObject* ownerPtr, std::string filename);
        explicit Sprite(GameObject* ownerPtr, std::string filename, int tileSize, int tileCount);
        explicit Sprite(GameObject* ownerPtr, std::string filename, int tileSize, int tileCount, int maxWidth, int maxHeight);
        ~Sprite() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
		
        void FixedUpdate() override {}
        void Update() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void SetTexture(const std::string& filename);
        void SetTileSize(const int tileSize) { m_TileSize = tileSize; }
        void SetTileCount(const int tileCount) { m_TileCountX = tileCount; m_TileCountY = tileCount; }
        void SetTileCount(const int tileCountX, const int tileCountY) { m_TileCountX = tileCountX; m_TileCountY = tileCountY; } 
        void SetMaxWidth(const int maxWidth) { m_MaxWidth = maxWidth; }
        void SetMaxHeight(const int maxHeight) { m_MaxHeight = maxHeight; }
        void SetWallSpritePosition(const sf::FloatRect& position) { m_WallSpritePosition = position; }
        void ResizeVertexArray()
        {
            m_VertexArray.resize
            (
                static_cast<size_t>(m_TileCountX) * 
                static_cast<size_t>(m_TileCountY) * 
                static_cast<size_t>(TRIANGLE_VERTICES)
            );
        }
        
        sf::Texture GetTexture() const { return m_SFMLTexture; }
        [[nodiscard]] sf::VertexArray& GetVertexArray() { return m_VertexArray; }

        void Render() const;
        void GenerateMap();

    private:
        sf::VertexArray m_VertexArray;
        sf::Texture m_SFMLTexture;
        std::string m_FilePath;
        // Give the exact pixel positions, not the tile position
        sf::FloatRect m_WallSpritePosition;

        int m_TileSize = 0;
        int m_TileCountX = 0;
        int m_TileCountY = 0;
        int m_MaxWidth = 0;
        int m_MaxHeight = 0;
        const int TRIANGLE_VERTICES = 6;
    };
}
