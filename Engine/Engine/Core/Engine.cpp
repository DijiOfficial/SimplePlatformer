#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include "Engine.h"
#include "Renderer.h"
#include "../Input/InputManager.h"
#include "../Singleton/SceneManager.h"
#include "../Singleton/ResourceManager.h"
#include "../Singleton/Singleton.h"
#include "../Singleton/TimeSingleton.h"
#include "../Singleton/PauseSingleton.h"

#include <chrono>
#include <SFML/Graphics.hpp>
#include <stdexcept>
#include <thread>

diji::Engine::Engine(const std::string& dataPath, const std::string& title, const bool useScreenResolution)
{
	window::VIEWPORT = useScreenResolution ? sf::Vector2u{ sf::VideoMode::getDesktopMode().size.x, sf::VideoMode::getDesktopMode().size.y } : sf::Vector2u{ 1920, 1080 };
	window::g_window_ptr = std::make_unique<sf::RenderWindow>(sf::VideoMode(sf::Vector2u{ window::VIEWPORT.x, window::VIEWPORT.y }), title, sf::Style::Default ,sf::State::Windowed);
	
	if (window::g_window_ptr == nullptr)
	{
		throw std::runtime_error(std::string("Create Window Error"));
	}

	Renderer::GetInstance().Init(window::g_window_ptr.get());
	InputManager::GetInstance().Init(window::g_window_ptr.get());
	ResourceManager::GetInstance().Init(dataPath);
}

// todo: settings?
// constexpr float FRAME_RATE = 260.f;
// constexpr bool useFixedFrameRate = true;
// constexpr auto FRAME_DURATION = std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(1.0 / FRAME_RATE));

void diji::Engine::Run(const std::function<void()>& load)
{
	load();
	
	//Enable vSync
	// ValidateVSync();
	// window::g_window_ptr->setVerticalSyncEnabled(true);
	// constexpr bool isFrameRateTooLow = useFixedFrameRate && (FRAME_RATE <= FIXED_TIME_STEP);
	
	const auto& renderer = Renderer::GetInstance();
	auto& sceneManager = SceneManager::GetInstance();
	auto& input = InputManager::GetInstance();
	const auto& pause = PauseSingleton::GetInstance();
	auto& time = TimeSingleton::GetInstance();

	auto lastFrameTime = std::chrono::steady_clock::now();
	// auto nextFrameTime = lastFrameTime;

	float lag = 0.0f;

	sceneManager.Init();
	sceneManager.Start();

	time.SetFixedUpdateDeltaTime(FIXED_TIME_STEP);
	
	while (window::g_window_ptr->isOpen())
	{
		const auto currentTime = std::chrono::steady_clock::now();
		const float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
		lastFrameTime = currentTime;

		TimeSingleton::GetInstance().SetDeltaTime(deltaTime);
		if (!input.ProcessInput())
			break;
		
		if (pause.GetIsPaused())
		{
			renderer.Render();
			continue;
		}

		lag += deltaTime;
		while (lag >= FIXED_TIME_STEP)
		{
			sceneManager.FixedUpdate(); // todo: pass down fixed time step instead of getting it from the singleton
			lag -= FIXED_TIME_STEP;
		}

		time.SetFixedTimeAlpha(lag / FIXED_TIME_STEP);
		// time.SetFixedTimeAlpha(isFrameRateTooLow ? 1.0f : lag / FIXED_TIME_STEP);
		// starting to consider passing the deltaTime to Update methods with [[maybe_unused]] attribute
		// TimeSingleton is useful for other purposes but gets annoying to use in update methods and mostly optimizing the call
		sceneManager.Update();
		sceneManager.LateUpdate();
		
		renderer.Render();
		
		sceneManager.EndFrameUpdate();

		// if (!useFixedFrameRate) continue;
		// nextFrameTime += FRAME_DURATION;
		// const auto frameEnd = std::chrono::steady_clock::now();
		// if (nextFrameTime > frameEnd)
		// 	std::this_thread::sleep_until(nextFrameTime);
		// else
		// 	nextFrameTime = frameEnd;
	}
	
	window::g_window_ptr->close();
	
	sceneManager.OnDestroy();
}

unsigned int diji::Engine::QueryRefreshRate()
{
#ifdef _WIN32
	DEVMODE devMode = {};
	devMode.dmSize = sizeof(devMode);
	if (EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode))
		return devMode.dmDisplayFrequency;
#endif
	// Add other platforms if needed
	return 0; // Unknown
}

void diji::Engine::ValidateVSync()
{
	window::g_window_ptr->setVerticalSyncEnabled(QueryRefreshRate() > 60);
}
