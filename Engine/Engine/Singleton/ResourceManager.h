#pragma once
#include "Singleton.h"
// #include "Music.h"

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Graphics/Shader.hpp>

namespace sf
{
    class Texture;
}

namespace diji
{
    class ResourceManager final : public Singleton<ResourceManager>
    {
    public:
        void Init(const std::string& dataPath);
        [[nodiscard]] sf::Texture& LoadTexture(const std::string& file);
        [[nodiscard]] sf::Font& LoadFont(const std::string& file);
        [[nodiscard]] sf::Sound& LoadSoundEffect(const std::string& file);
        [[nodiscard]] sf::Shader& LoadShader(const std::string& vertexFile, const std::string& shaderFile);
        // Music* LoadMusic(const std::string& file);

    private:
        std::string m_DataPath;

        std::unordered_map<std::string, sf::Texture> m_TexturesUMap;
        std::unordered_map<std::string, sf::Font> m_FontsUMap;
        
        std::unordered_map<std::string, sf::Sound> m_SoundEffectsUMap;
        std::unordered_map<std::string, sf::SoundBuffer> m_SoundBuffersUMap;
        std::map<std::pair<std::string, std::string>, std::unique_ptr<sf::Shader>> m_ShaderBuffersUMap;
        // std::unordered_map<std::string, std::unique_ptr<Music>> m_MusicUPtrUMap;
    };
}
