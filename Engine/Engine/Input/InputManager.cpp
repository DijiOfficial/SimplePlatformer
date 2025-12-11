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
    while (auto eventOpt = m_WindowPtr->pollEvent())
    {
        const sf::Event& event = *eventOpt;

        if (event.is<sf::Event::Closed>())
            return false;

        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                return false;
        }

        if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>())
        {
            m_MousePosition = mouseMoved->position;
            for (auto& command : m_MouseMoveCommandsVec)
            {
                if (command)
                    command->Execute(mouseMoved->position);
            }
        }

        SetInputState<sf::Mouse::Button, sf::Event::MouseButtonPressed, sf::Event::MouseButtonReleased>(event, m_MousePressedState, m_MouseHeldState,&sf::Event::MouseButtonPressed::button, &sf::Event::MouseButtonReleased::button);
        SetInputState<sf::Keyboard::Scancode, sf::Event::KeyPressed, sf::Event::KeyReleased>(event, m_KeyPressedState, m_KeyHeldState, &sf::Event::KeyPressed::scancode, &sf::Event::KeyReleased::scancode);

        auto HandleKeyReleaseEvent = [&](KeyState state, auto* inputEvent, auto eventType)
        {
            if (inputEvent)
                HandleInputOfPlayer(PlayerIdx::KEYBOARD, state, eventType(*inputEvent));
        };
        HandleKeyReleaseEvent(KeyState::RELEASED, event.getIf<sf::Event::KeyReleased>(), [](const auto& e) { return e.scancode; });
        HandleKeyReleaseEvent(KeyState::RELEASED, event.getIf<sf::Event::MouseButtonReleased>(), [](const auto& e) { return e.button; });
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
                HandleInputOfPlayer(playerIndex, KeyState::PRESSED, Input::InputType{ button });
            }
            else if (isPressedNow && wasHeld)
            {
                // Held
                HandleInputOfPlayer(playerIndex, KeyState::HELD, Input::InputType{ button });
            }
            else if (!isPressedNow && wasHeld)
            {
                // Released
                m_ControllerHeldState[pInt][button] = false;
                m_ControllerPressedState[pInt][button] = false;
                HandleInputOfPlayer(playerIndex, KeyState::RELEASED, Input::InputType{ button });
            }
        }
    }
}

void diji::InputManager::BindController(int controllerIdx)
{
	if (!m_PlayersMap.contains(controllerIdx))
		m_PlayersMap.emplace(controllerIdx, std::make_unique<Controller>(controllerIdx));
}