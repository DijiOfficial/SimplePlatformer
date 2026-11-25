#include "Renderer.h"
#include "../Singleton/SceneManager.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/View.hpp>

#include <cmath>

void diji::Renderer::Init(sf::RenderWindow* window)
{
	m_WindowPtr = window;
	m_ClearColor = sf::Color::Black;
}

void diji::Renderer::Render() const
{
	m_WindowPtr->clear(m_ClearColor);
	
	SceneManager::GetInstance().Render();

	FlushSpriteBatch();
	m_WindowPtr->display();
}

void diji::Renderer::Destroy()
{
	delete m_WindowPtr;
	m_WindowPtr = nullptr;
}

void diji::Renderer::RenderTexture(const sf::Texture& texture, const sf::Vector2f& origin, const sf::Angle& rotation, const float x, const float y, const float scaleX, const float scaleY) const
{
	FlushSpriteBatch();

	sf::Sprite sprite(texture);

	sprite.setOrigin(origin);
	sprite.setPosition(sf::Vector2f{ x, y });
	sprite.setScale(sf::Vector2f{ scaleX, scaleY });
	sprite.setRotation(rotation);

	m_WindowPtr->draw(sprite);
}

void diji::Renderer::RenderTexture(const sf::Texture& texture, const sf::Vector2f& origin, const sf::Angle& rotation, const float x, const float y, const float scale) const
{
	RenderTexture(texture, origin, rotation, x, y, scale, scale);
}

void diji::Renderer::RenderTexture(const sf::Texture& texture, const sf::Vector2f& origin, const sf::Angle& rotation, const float x, const float y, const int width, const int height) const
{
	const sf::Vector2u texSize = texture.getSize();
	const float scaleX = static_cast<float>(width) / static_cast<float>(texSize.x);
	const float scaleY = static_cast<float>(height) / static_cast<float>(texSize.y);

	RenderTexture(texture, origin, rotation, x, y, scaleX, scaleY);
}

void diji::Renderer::RenderVertexArray(const sf::Texture& texture, const sf::VertexArray& vertexArray) const
{
	FlushSpriteBatch();

	m_WindowPtr->draw(vertexArray, &texture);
}

void diji::Renderer::RenderText(sf::Text& text, const float x, const float y, const float scaleX, const float scaleY, bool isCentered) const
{
	FlushSpriteBatch();

	if (isCentered)
	{
		const sf::FloatRect textRect = text.getLocalBounds();
		text.setOrigin(sf::Vector2f { textRect.position.x + textRect.size.x * 0.5f, textRect.position.y + textRect.size.y * 0.5f });
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
	const float scaleX = static_cast<float>(width) / texSize.size.x;
	const float scaleY = static_cast<float>(height) / texSize.size.y;

	RenderText(text , x, y, scaleX, scaleY, isCentered);
}

void diji::Renderer::RenderRotatedTexture(const sf::Texture& texture, const float x, const float y, const sf::Angle& angle, const float scaleX, const float scaleY) const
{
	FlushSpriteBatch();

	sf::Sprite sprite(texture);

	const sf::Vector2u texSize = texture.getSize();
	sprite.setOrigin(sf::Vector2f{ static_cast<float>(texSize.x) * 0.5f, static_cast<float>(texSize.y) * 0.5f });

	sprite.setPosition(sf::Vector2f{ x, y });
	sprite.setRotation(angle);
	sprite.setScale(sf::Vector2f{ scaleX, scaleY });

	m_WindowPtr->draw(sprite);
}

void diji::Renderer::RenderRotatedTexture(const sf::Texture& texture, const float x, const float y, const sf::Angle& angle, const float scale) const
{
	RenderRotatedTexture(texture, x, y, angle, scale, scale);
}

void diji::Renderer::RenderRotatedTexture(const sf::Texture& texture, const float x, const float y, const int width, const int height, const sf::Angle& angle) const
{
	const sf::Vector2u texSize = texture.getSize();
	const float scaleX = static_cast<float>(width) / static_cast<float>(texSize.x);
	const float scaleY = static_cast<float>(height) / static_cast<float>(texSize.y);

	RenderRotatedTexture(texture, x, y, angle, scaleX, scaleY);
}

void diji::Renderer::DrawRect(const sf::RectangleShape& shape, const sf::Color& outline, const sf::Color& fill, const float outlineWidth) const
{
	FlushSpriteBatch();

	sf::RectangleShape rect = shape;
	rect.setFillColor(fill);
	rect.setOutlineColor(outline);
	rect.setOutlineThickness(outlineWidth);

	m_WindowPtr->draw(rect);
}

void diji::Renderer::DrawRect(const sf::RectangleShape& shape) const
{
	DrawRect(shape, shape.getOutlineColor() ,sf::Color::Transparent);
}

void diji::Renderer::DrawFilledRect(const sf::RectangleShape& shape, const sf::Color& color) const
{
	DrawRect(shape, color, color, 0);
}

void diji::Renderer::DrawShape(const sf::Shape& shape) const
{
	FlushSpriteBatch();

	m_WindowPtr->draw(shape);
}

void diji::Renderer::RenderSprite(const sf::Sprite& sprite) const
{
	const sf::Texture& texture = sprite.getTexture();
	if (m_SpriteBatchTexturePtr != &texture)
	{
		FlushSpriteBatch();
		m_SpriteBatchTexturePtr = &texture;
	}

	const sf::FloatRect textureRect{ sprite.getTextureRect() };
	const sf::Vector2f absoluteSize{ std::abs(textureRect.size.x), std::abs(textureRect.size.y) };
	const sf::Transform& transform = sprite.getTransform();
	const sf::Color color = sprite.getColor();

	const sf::Vertex topLeft{ transform.transformPoint(sf::Vector2f{ 0.0f, 0.0f }), color, textureRect.position };
	const sf::Vertex bottomLeft{ transform.transformPoint(sf::Vector2f{ 0.0f, absoluteSize.y }), color, textureRect.position + sf::Vector2f{ 0.0f, textureRect.size.y } };
	const sf::Vertex topRight{ transform.transformPoint(sf::Vector2f{ absoluteSize.x, 0.0f }), color, textureRect.position + sf::Vector2f{ textureRect.size.x, 0.0f } };
	const sf::Vertex bottomRight{ transform.transformPoint(absoluteSize), color, textureRect.position + textureRect.size };

	m_SpriteBatch.append(topLeft);
	m_SpriteBatch.append(bottomLeft);
	m_SpriteBatch.append(topRight);

	m_SpriteBatch.append(topRight);
	m_SpriteBatch.append(bottomLeft);
	m_SpriteBatch.append(bottomRight);
}

void diji::Renderer::RenderSpriteWithShader(const sf::Sprite& sprite, const sf::Shader* shader) const
{
	FlushSpriteBatch();

	m_WindowPtr->draw(sprite, shader);
}

void diji::Renderer::SetView(const sf::View& view) const
{
	FlushSpriteBatch();
	m_WindowPtr->setView(view);
}

void diji::Renderer::FlushSpriteBatch() const
{
	if (m_SpriteBatch.getVertexCount() == 0) return;

	m_WindowPtr->draw(m_SpriteBatch, m_SpriteBatchTexturePtr);
	m_SpriteBatch.clear();
	m_SpriteBatchTexturePtr = nullptr;
}


void diji::Renderer::SetBackgroundColor(const sf::Color& color)
{
	m_ClearColor = color;
}
