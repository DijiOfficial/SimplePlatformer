#include "InputManager.h"

#include <ranges>
#include <unordered_set>
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

	for (auto& btnMap : m_ControllerPressedState | std::views::values)
	{
		for (auto& pressed : btnMap | std::views::values)
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
			HandleInputForPlayer(PlayerIdx::KEYBOARD, KeyState::RELEASED, event.key.code);
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
			HandleInputForPlayer(PlayerIdx::KEYBOARD, KeyState::RELEASED, event.mouseButton.button);
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
	ProcessControllerInput();
}

// This function does both polling and handling of controller input
void diji::InputManager::ProcessControllerInput()
{
    // Update internal controller objects
    for (const auto& controller : m_PlayersMap | std::views::values)
    {
    	controller->ProcessControllerInput();
    }

    for (const auto& [key, playerCommands] : m_CommandUMap)
    {
        if (std::holds_alternative<Controller::Button>(key.input) == false)
            continue;

        const auto button = std::get<Controller::Button>(key.input);

        for (const auto& [playerIndex, commandUPtr] : playerCommands)
        {
            if (playerIndex == PlayerIdx::KEYBOARD)
                continue;

            int pInt = static_cast<int>(playerIndex);

            // If no controller bound/created for this player, skip
            auto itController = m_PlayersMap.find(pInt);
            if (itController == m_PlayersMap.end() || !itController->second)
                continue;

            const Controller* controller = itController->second.get();

            const bool isPressedNow = controller->IsPressed(button);

            bool wasHeld = false;
            auto itHeldMap = m_ControllerHeldState.find(pInt);
            if (itHeldMap != m_ControllerHeldState.end())
                wasHeld = itHeldMap->second[button];

            // Decide event type and call the player-specific handler
            if (isPressedNow && !wasHeld)
            {
                // Pressed this frame for this player/button
                m_ControllerPressedState[pInt][button] = true;
                m_ControllerHeldState[pInt][button] = true;
                HandleInputForPlayer(playerIndex, KeyState::PRESSED, Input::InputType{ button });
            }
            else if (isPressedNow && wasHeld)
            {
                // Held
                HandleInputForPlayer(playerIndex, KeyState::HELD, Input::InputType{ button });
            }
            else if (!isPressedNow && wasHeld)
            {
                // Released
                m_ControllerHeldState[pInt][button] = false;
                m_ControllerPressedState[pInt][button] = false;
                HandleInputForPlayer(playerIndex, KeyState::RELEASED, Input::InputType{ button });
            }
        }
    }
}

void diji::InputManager::BindController(int controllerIdx)
{
	if (!m_PlayersMap.contains(controllerIdx))
		m_PlayersMap.emplace(controllerIdx, std::make_unique<Controller>(controllerIdx));
}