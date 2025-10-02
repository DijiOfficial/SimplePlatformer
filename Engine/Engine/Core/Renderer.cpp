#include "Renderer.h"
#include "../Singleton/SceneManager.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

void diji::Renderer::Init(sf::RenderWindow* window)
{
	m_WindowPtr = window;
}

void diji::Renderer::Render() const
{
	m_WindowPtr->clear();
	
	SceneManager::GetInstance().Render();

	m_WindowPtr->display();
}

void diji::Renderer::Destroy()
{
	delete m_WindowPtr;
	m_WindowPtr = nullptr;
}

void diji::Renderer::RenderTexture(const sf::Texture& texture, const sf::Vector2f& origin, const float rotation, const float x, const float y, const float scaleX, const float scaleY) const
{
	sf::Sprite sprite(texture);

	sprite.setOrigin(origin);
	sprite.setPosition(sf::Vector2f{ x, y });
	sprite.setScale(sf::Vector2f{ scaleX, scaleY });
	sprite.setRotation(rotation);

	m_WindowPtr->draw(sprite);
}

void diji::Renderer::RenderTexture(const sf::Texture& texture, const sf::Vector2f& origin, const float rotation, const float x, const float y, const float scale) const
{
	RenderTexture(texture, origin, rotation, x, y, scale, scale);
}

void diji::Renderer::RenderTexture(const sf::Texture& texture, const sf::Vector2f& origin, const float rotation, const float x, const float y, const int width, const int height) const
{
	const sf::Vector2u texSize = texture.getSize();
	const float scaleX = static_cast<float>(width) / static_cast<float>(texSize.x);
	const float scaleY = static_cast<float>(height) / static_cast<float>(texSize.y);

	RenderTexture(texture, origin, rotation, x, y, scaleX, scaleY);
}

void diji::Renderer::RenderVertexArray(const sf::Texture& texture, const sf::VertexArray& vertexArray) const
{
	m_WindowPtr->draw(vertexArray, &texture);
}

void diji::Renderer::RenderText(sf::Text& text, const float x, const float y, const float scaleX, const float scaleY, bool isCentered) const
{
	if (isCentered)
	{
		const sf::FloatRect textRect = text.getLocalBounds();
		text.setOrigin(sf::Vector2f { textRect.left + textRect.width * 0.5f, textRect.top + textRect.height * 0.5f });
	}

	text.setPosition(sf::Vector2f{ x, y });
	text.setScale(sf::Vector2f{ scaleX, scaleY });

	m_WindowPtr->draw(text);
}

void diji::Renderer::RenderText(sf::Text& text, const float x, const float y, const float scale, const bool isCentered) const
{
	RenderText(text , x, y, scale, scale, isCentered);
}

void diji::Renderer::RenderText(sf::Text& text, const float x, const float y, const int width, const int height, const bool isCentered) const
{
	const sf::FloatRect texSize = text.getLocalBounds();
	const float scaleX = static_cast<float>(width) / texSize.width;
	const float scaleY = static_cast<float>(height) / texSize.height;

	RenderText(text , x, y, scaleX, scaleY, isCentered);
}

void diji::Renderer::RenderRotatedTexture(const sf::Texture& texture, const float x, const float y, const float angle, const float scaleX, const float scaleY) const
{
	sf::Sprite sprite(texture);

	const sf::Vector2u texSize = texture.getSize();
	sprite.setOrigin(sf::Vector2f{ static_cast<float>(texSize.x) * 0.5f, static_cast<float>(texSize.y) * 0.5f });

	sprite.setPosition(sf::Vector2f{ x, y });
	sprite.setRotation(angle);
	sprite.setScale(sf::Vector2f{ scaleX, scaleY });

	m_WindowPtr->draw(sprite);
}

void diji::Renderer::RenderRotatedTexture(const sf::Texture& texture, const float x, const float y, const float angle, const float scale) const
{
	RenderRotatedTexture(texture, x, y, angle, scale, scale);
}

void diji::Renderer::RenderRotatedTexture(const sf::Texture& texture, const float x, const float y, const int width, const int height, const float angle) const
{
	const sf::Vector2u texSize = texture.getSize();
	const float scaleX = static_cast<float>(width) / static_cast<float>(texSize.x);
	const float scaleY = static_cast<float>(height) / static_cast<float>(texSize.y);

	RenderRotatedTexture(texture, x, y, angle, scaleX, scaleY);
}

void diji::Renderer::DrawRect(const sf::RectangleShape& shape, const sf::Color& outline, const sf::Color& fill, const float outlineWidth) const
{
	sf::RectangleShape rect = shape;
	rect.setFillColor(fill);
	rect.setOutlineColor(outline);
	rect.setOutlineThickness(outlineWidth);

	m_WindowPtr->draw(rect);
}

void diji::Renderer::DrawRect(const sf::RectangleShape& shape) const
{
	DrawRect(shape, sf::Color::White ,sf::Color::Transparent);
}

void diji::Renderer::DrawFilledRect(const sf::RectangleShape& shape, const sf::Color& color) const
{
	DrawRect(shape, color, color, 0);
}

void diji::Renderer::SetBackgroundColor(const sf::Color& color)
{
	m_ClearColor = color;
}
