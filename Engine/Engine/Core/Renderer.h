#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include "../Singleton/Singleton.h"

namespace sf
{
	class Sprite;
	class VertexArray;
	class RectangleShape;
	class CircleShape;
	class ConvexShape;
	class Shape;
	class Color;
	class Angle;
	class RenderWindow;
	class Texture;
	class Text;
}

namespace diji
{
	class Renderer final : public Singleton<Renderer>
	{
	public:
		void Init(sf::RenderWindow* window);
		void Render() const;
		void Destroy();
		
		void RenderTexture(const sf::Texture& texture, const sf::Vector2f& origin, float rotation, float x, float y, float scaleX = 1, float scaleY = 1) const;
		void RenderTexture(const sf::Texture& texture, const sf::Vector2f& origin, float rotation, float x, float y, float scale = 1) const;
		void RenderTexture(const sf::Texture& texture, const sf::Vector2f& origin, float rotation, float x, float y, int width, int height) const;
		void RenderVertexArray(const sf::Texture& texture, const sf::VertexArray& vertexArray) const;
		
		void RenderText(sf::Text& text, float x, float y, float scaleX = 1, float scaleY = 1, bool isCentered = false) const;
		void RenderText(sf::Text& text, float x, float y, float scale = 1, bool isCentered = false) const;
		void RenderText(sf::Text& text, float x, float y, int width, int height, bool isCentered = false) const;
		void RenderRotatedTexture(const sf::Texture& texture, float x, float y, float angle, float scaleX = 1, float scaleY = 1) const;
		void RenderRotatedTexture(const sf::Texture& texture, float x, float y, float angle, float scale = 1) const;
		void RenderRotatedTexture(const sf::Texture& texture, float x, float y, int width, int height, float angle) const;
		
		// todo: unecessary fucntions?
		void DrawRect(const sf::RectangleShape& shape, const sf::Color& outline, const sf::Color& fill, float outlineWidth = 1.f) const;
		void DrawRect(const sf::RectangleShape& shape) const;
		void DrawFilledRect(const sf::RectangleShape& shape, const sf::Color& color) const;
		
		void DrawShape(const sf::Shape& shape) const;
		void RenderSprite(const sf::Sprite& sprite) const;

		[[nodiscard]] const sf::Color& GetBackgroundColor() const { return m_ClearColor; }
		void SetBackgroundColor(const sf::Color& color);

	private:
		// todo: use the static global window pointer instead
		sf::RenderWindow* m_WindowPtr = nullptr;
		sf::Color m_ClearColor;
	};
}

