#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include "../Singleton/Singleton.h"

namespace sf
{
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

		// Handle shape drawing with function overloading
		void DrawShape(const sf::Shape& shape) const;
		
		// void DrawLine(const glm::vec2& p1, const glm::vec2& p2, const SDL_Color& color = { 255, 255, 255, 255 }) const
		// {
		// 	SDL_SetRenderDrawColor(m_RendererPtr, color.r, color.g, color.b, color.a);
		// 	SDL_RenderDrawLine(m_RendererPtr, static_cast<int>(p1.x), static_cast<int>(p1.y), static_cast<int>(p2.x), static_cast<int>(p2.y));
		// }
		//
		// void DrawPolygon(const std::vector<glm::vec2>& points)
		// {
		// 	SDL_SetRenderDrawColor(m_RendererPtr, 255, 0, 0, 255); // Set color to green
		//
		// 	// Convert glm::vec2 points to SDL_Point
		// 	std::vector<SDL_Point> sdlPoints;
		// 	for (const auto& point : points)
		// 	{
		// 		sdlPoints.push_back({ static_cast<int>(point.x), static_cast<int>(point.y) });
		// 	}
		//
		// 	// Ensure the polygon is closed by adding the first point at the end
		// 	if (!sdlPoints.empty())
		// 	{
		// 		sdlPoints.push_back(sdlPoints.front());
		// 	}
		//
		// 	// Draw the lines
		// 	SDL_RenderDrawLines(m_RendererPtr, sdlPoints.data(), static_cast<int>(sdlPoints.size()));
		// }
		//
		// void DrawTriangle(const glm::vec2& center, float size, const SDL_Color& color = { 255, 255, 255, 255 }) const
		// {
		// 	// Calculate the height of the equilateral triangle
		// 	float height = (sqrt(3.0f) * 0.5f) * size;
		//
		// 	// Calculate the three vertices of the equilateral triangle
		// 	//glm::vec2 p1(center.x, center.y - (2.0f / 3.0f) * height); // Top vertex
		// 	//glm::vec2 p2(center.x - size * 0.5f, center.y + (1.0f / 3.0f) * height); // Bottom left vertex
		// 	//glm::vec2 p3(center.x + size * 0.5f, center.y + (1.0f / 3.0f) * height); // Bottom right vertex
		//
		// 	glm::vec2 p1(center.x - size * 0.5f, center.y - (2.0f / 3.0f) * height); // Top vertex
		// 	glm::vec2 p2(center.x - size * 0.5f, center.y + (2.0f / 3.0f) * height); // Bottom left vertex
		// 	glm::vec2 p3(center.x + size * 0.5f, center.y); // Bottom right vertex
		//
		// 	// Call the original DrawTriangle function to draw the triangle
		// 	DrawTriangle(p1, p2, p3, color);
		// }
		//
		// void DrawTriangle(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const SDL_Color& color = { 255, 255, 255, 255 }) const
		// {
		// 	// Set the drawing color to the given color
		// 	SDL_SetRenderDrawColor(m_RendererPtr, color.r, color.g, color.b, color.a);
		//
		// 	// Draw the three lines connecting the vertices of the triangle
		// 	SDL_RenderDrawLine(m_RendererPtr, static_cast<int>(p1.x), static_cast<int>(p1.y), static_cast<int>(p2.x), static_cast<int>(p2.y));
		// 	SDL_RenderDrawLine(m_RendererPtr, static_cast<int>(p2.x), static_cast<int>(p2.y), static_cast<int>(p3.x), static_cast<int>(p3.y));
		// 	SDL_RenderDrawLine(m_RendererPtr, static_cast<int>(p3.x), static_cast<int>(p3.y), static_cast<int>(p1.x), static_cast<int>(p1.y));
		// }
		//
		// void DrawFilledTriangle(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const SDL_Color& color = { 255, 255, 255, 255 }) const
		// {
		// 	// Set the drawing color to the given color
		// 	SDL_SetRenderDrawColor(m_RendererPtr, color.r, color.g, color.b, color.a);
		//
		// 	// Helper function to draw a horizontal line between two x-coordinates on the same y-coordinate
		// 	auto drawHorizontalLine = [&](int y, int x1, int x2)
		// 		{
		// 			if (x1 > x2) std::swap(x1, x2);  // Ensure x1 <= x2
		// 			for (int x = x1; x <= x2; ++x)
		// 			{
		// 				SDL_RenderDrawPoint(m_RendererPtr, x, y);
		// 			}
		// 		};
		//
		// 	// Assume p1 and p2 have the same x coordinate (left points), and p3 is the right point
		// 	float invSlope1 = (p3.y - p1.y) != 0 ? (p3.x - p1.x) / (p3.y - p1.y) : 0; // Slope between p1 and p3
		// 	float invSlope2 = (p3.y - p2.y) != 0 ? (p3.x - p2.x) / (p3.y - p2.y) : 0; // Slope between p2 and p3
		//
		// 	// Determine the top and bottom left points (p1 is top, p2 is bottom if p1.y < p2.y)
		// 	glm::vec2 top = (p1.y < p2.y) ? p1 : p2;
		// 	glm::vec2 bottom = (p1.y < p2.y) ? p2 : p1;
		//
		// 	// Draw the triangle line by line using the scanline algorithm
		// 	for (int y = static_cast<int>(top.y); y <= static_cast<int>(bottom.y); ++y)
		// 	{
		// 		// Compute the x-coordinates of the current scanline intersections
		// 		float x1 = top.x + (y - top.y) * invSlope1; // Intersection with p1-p3 edge
		// 		float x2 = bottom.x + (y - bottom.y) * invSlope2; // Intersection with p2-p3 edge
		//
		// 		// Draw horizontal line between the two x-coordinates
		// 		drawHorizontalLine(y, static_cast<int>(x1), static_cast<int>(x2));
		// 	}
		// }
		//
		// void DrawFilledTriangle(const glm::vec2& center, float size, const SDL_Color& color = { 255, 255, 255, 255 }) const
		// {
		// 	// The two left points are vertically aligned, and the right point is centered between them
		// 	glm::vec2 p1(center.x - size * 0.5f, center.y - size * 0.5f); // Top-left point
		// 	glm::vec2 p2(center.x - size * 0.5f, center.y + size * 0.5f); // Bottom-left point
		// 	glm::vec2 p3(center.x + size * 0.5f, center.y);            // Right point (middle y)
		//
		// 	// Call the filled triangle function with the calculated vertices
		// 	const std::vector<SDL_Vertex> verts = 
		// 	{
		// 		{ SDL_FPoint{ p1.x, p1.y }, color, SDL_FPoint{ 0 }, },
		// 		{ SDL_FPoint{ p2.x, p2.y }, color, SDL_FPoint{ 0 }, },
		// 		{ SDL_FPoint{ p3.x, p3.y }, color, SDL_FPoint{ 0 }, },
		// 	};
		// 	DrawFilledTriangle(verts);
		// }
		//
		// void DrawFilledTriangle(const std::vector<SDL_Vertex>& verts) const
		// {
		// 	SDL_RenderGeometry(m_RendererPtr, nullptr, verts.data(), static_cast<int>(verts.size()), nullptr, 0);
		// }

		[[nodiscard]] const sf::Color& GetBackgroundColor() const { return m_ClearColor; }
		void SetBackgroundColor(const sf::Color& color);

	private:
		// todo: use the static global window pointer instead
		sf::RenderWindow* m_WindowPtr = nullptr;
		sf::Color m_ClearColor;
	};
}

