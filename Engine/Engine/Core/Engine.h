#pragma once
#include <string>
#include <functional>
#include <memory>
#include <SFML/Graphics/RenderWindow.hpp>

namespace window
{
	inline std::unique_ptr<sf::RenderWindow> g_window_ptr{};
	inline sf::Vector2u VIEWPORT = sf::Vector2u{ sf::VideoMode::getDesktopMode().size.x, sf::VideoMode::getDesktopMode().size.y };
	inline std::string g_window_title = "Diji Engine";
}

namespace diji
{
	class Engine final
	{
	public:
		explicit Engine(const std::string& dataPath, const std::string& title, bool useScreenResolution = true);
		~Engine() noexcept = default;

		Engine(const Engine& other) = delete;
		Engine(Engine&& other) = delete;
		Engine& operator=(const Engine& other) = delete;
		Engine& operator=(Engine&& other) = delete;

		static void Run(const std::function<void()>& load);

	private:
		static constexpr float FIXED_TIME_STEP{ 1 / 60.f };
		static unsigned int QueryRefreshRate();
		static void ValidateVSync();
	};
}
