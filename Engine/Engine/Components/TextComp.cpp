#include "TextComp.h"
#include "../Core/GameObject.h"
#include "../Singleton/ResourceManager.h"

#include <SFML/Graphics/Font.hpp>

diji::TextComp::TextComp(GameObject* ownerPtr, const std::string& text, const std::string& filename, const sf::Color& color, const bool isCentered)
	: Component(ownerPtr)
	, m_IsCentered{ isCentered }
{
	m_Font = ResourceManager::GetInstance().LoadFont(filename);

	m_Text.setFont(m_Font);
	m_Text.setString(text);
	m_Text.setFillColor(color);
}

void diji::TextComp::SetFont(const sf::Font& font)
{
	m_Font = font;
	
	m_Text.setFont(m_Font);
}



