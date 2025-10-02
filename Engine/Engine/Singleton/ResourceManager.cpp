#include "ResourceManager.h"

#include <stdexcept>
#include <SFML/Graphics/Texture.hpp>

void diji::ResourceManager::Init(const std::string& dataPath)
{
	m_DataPath = dataPath;
}

sf::Texture& diji::ResourceManager::LoadTexture(const std::string& file)
{
	// check if texture is already loaded
	const auto fullPath = m_DataPath + file;
	const auto it = m_TexturesUMap.find(fullPath);
	if (it != m_TexturesUMap.cend())
	{
		return it->second;
	}

	// if not cache it
	sf::Texture tempTexture;
	if (!tempTexture.loadFromFile(fullPath.c_str()))
	{
		throw std::runtime_error("Failed to load texture: " + fullPath);
	}

	m_TexturesUMap[fullPath] = sf::Texture(tempTexture);
	return m_TexturesUMap[fullPath];
}

sf::Font& diji::ResourceManager::LoadFont(const std::string& file)
{
	// check if texture is already loaded
	const auto fullPath = m_DataPath + file;
	const auto it = m_FontsUMap.find(fullPath);
	if (it != m_FontsUMap.cend())
	{
		return it->second;
	}
	
	// Store it if it's not already loaded
	sf::Font tempFont;
	if (!tempFont.loadFromFile(fullPath))
	{
		throw std::runtime_error("Failed to load font: " + fullPath);
	}
	m_FontsUMap.emplace(fullPath, tempFont);

	return m_FontsUMap[fullPath];
}

sf::Sound& diji::ResourceManager::LoadSoundEffect(const std::string& file)
{
	// Check if sound already exists
	const auto fullPath = m_DataPath + file;
	if (const auto it = m_SoundEffectsUMap.find(fullPath); it != m_SoundEffectsUMap.end())
		return it->second;

	// Ensure buffer is loaded
	if (!m_SoundBuffersUMap.contains(fullPath))
	{
		sf::SoundBuffer buffer;
		if (!buffer.loadFromFile(fullPath))
			throw std::runtime_error("Failed to load sound: " + fullPath);
		
		m_SoundBuffersUMap.emplace(fullPath, std::move(buffer));
	}

	// Create sound using the stored buffer
	sf::Sound sound {m_SoundBuffersUMap.at(fullPath)};
    
	// Store and return the sound
	return m_SoundEffectsUMap.emplace(fullPath, std::move(sound)).first->second;
}