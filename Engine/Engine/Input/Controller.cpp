#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include <XInput.h>
#include "Controller.h"

#include <iostream>
class diji::Controller::XInput final
{
public:
	explicit XInput(const int controllerIdx) 
		: m_ControllerIdx{ controllerIdx }
	{
		//this does not work the way I intended
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		const DWORD dwResult = XInputGetState(controllerIdx, &state);

		if (dwResult != ERROR_SUCCESS)
		{
			switch (dwResult)
			{
			case ERROR_DEVICE_NOT_CONNECTED:
				std::cout << "Controller error! Device not connected! \n";
				break;
			case ERROR_NOT_SUPPORTED:
				std::cout << "Controller error! Not supported! \n";
				break;
			default:
				std::cout << "Controller error! \n";
				break;
			}
		}
	}
	~XInput() = default;

	XInput(const XInput& other) = delete;
	XInput(XInput&& other) = delete;
	XInput& operator=(const XInput& other) = delete;
	XInput& operator=(XInput&& other) = delete;

	void ProcessControllerInput()
	{
		CopyMemory(&m_PreviousState, &m_CurrentState, sizeof(XINPUT_STATE));
		ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));
		XInputGetState(m_ControllerIdx, &m_CurrentState);

		m_PreviousButtons = m_CurrentButtons;
		m_CurrentButtons = m_CurrentState.Gamepad.wButtons;

		if (m_CurrentState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			m_CurrentButtons |= static_cast<unsigned int>(Button::LeftTrigger);

		if (m_CurrentState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			m_CurrentButtons |= static_cast<unsigned int>(Button::RightTrigger);

		const auto buttonChanges = m_CurrentButtons ^ m_PreviousButtons;
		m_ButtonsPressedThisFrame = buttonChanges & m_CurrentButtons;
		m_ButtonsReleasedThisFrame = buttonChanges & (~m_CurrentButtons);
	}

	bool IsDownThisFrame(const unsigned int button) const { return m_ButtonsPressedThisFrame & button; }
	bool IsUpThisFrame(const unsigned int button) const { return m_ButtonsReleasedThisFrame & button; }
	bool IsPressed(const unsigned int button) const { return m_CurrentButtons & button; }

private:
	int m_ControllerIdx{};
	XINPUT_STATE m_PreviousState{};
	XINPUT_STATE m_CurrentState{};
	unsigned int m_PreviousButtons{};
	unsigned int m_CurrentButtons{};
	unsigned int m_ButtonsPressedThisFrame{};
	unsigned int m_ButtonsReleasedThisFrame{};
};

diji::Controller::Controller(int controllerIdx) 
	: m_XInputUPtr{ std::make_unique<XInput>(controllerIdx) }
{
}

diji::Controller::~Controller() noexcept = default; // has to be defined in cpp otherwise std:unique_ptr will complain

void diji::Controller::ProcessControllerInput() const
{
	m_XInputUPtr->ProcessControllerInput();
}

bool diji::Controller::IsKeyDownThisFrame(const Button& button) const
{
	return m_XInputUPtr->IsDownThisFrame(static_cast<unsigned int>(button));
}

bool diji::Controller::IsKeyUpThisFrame(const Button& button) const
{
	return m_XInputUPtr->IsUpThisFrame(static_cast<unsigned int>(button));
}

bool diji::Controller::IsPressed(const Button& button) const
{
	return m_XInputUPtr->IsPressed(static_cast<unsigned int>(button));
}
