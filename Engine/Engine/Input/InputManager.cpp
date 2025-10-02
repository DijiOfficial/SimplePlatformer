#include "InputManager.h"

#include <ranges>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

bool diji::InputManager::ProcessInput()
{
	ResetPressedStates();

	if (!PollEvents())
		return false;
	
	ProcessAllInputMaps();
	
	return m_Continue;
}

void diji::InputManager::ResetPressedStates()
{
	for (auto& pressed : m_KeyPressedState | std::views::values)
	{
		pressed = false;
	}

	for (auto& pressed : m_MousePressedState | std::views::values)
	{
		pressed = false;
	}
}

bool diji::InputManager::PollEvents()
{
	sf::Event event;
	while (m_WindowPtr->pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			return false;

		// Look how I butchered my code because of this shit version of SFML
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Escape)
				return false;
			m_KeyPressedState[event.key.code] = true;
			m_KeyHeldState[event.key.code] = true;
		}
		if (event.type == sf::Event::KeyReleased)
		{
			m_KeyPressedState[event.key.code] = false;
			m_KeyHeldState[event.key.code] = false;
			HandleInput(KeyState::RELEASED, event.key.code);
		}
		if (event.type == sf::Event::MouseButtonPressed)
		{
			m_MousePressedState[event.mouseButton.button] = true;
			m_MouseHeldState[event.mouseButton.button] = true;
		}
		if (event.type == sf::Event::MouseButtonReleased)
		{
			m_MousePressedState[event.mouseButton.button] = false;
			m_MouseHeldState[event.mouseButton.button] = false;
			HandleInput(KeyState::RELEASED, event.mouseButton.button);
		}
		if (event.type == sf::Event::MouseMoved)
		{
			m_MousePosition = { event.mouseMove.x, event.mouseMove.y };
			for (auto& command : m_MouseMoveCommandsVec)
			{
				if (command)
					command->Execute(m_MousePosition);
			}
		}
	}
	return true;
}

void diji::InputManager::ProcessAllInputMaps()
{
	ProcessInputMap(m_KeyPressedState, m_KeyHeldState);
	ProcessInputMap(m_MousePressedState, m_MouseHeldState);
	// add controller
}
