#include "GamePadDirectInput.hpp"
#include <Utility.hpp>


namespace Grip {
namespace Input {


bool GamePadDirectInput::Create(LPDIRECTINPUTDEVICE8 pDevice, HWND hWnd, IInput* pInput, IGamePad** ppGamePad)
{
	if (!pDevice || !hWnd || !pInput || !ppGamePad) { return false; }

	GamePadDirectInput* pGamePad = new GamePadDirectInput();

	if (!pGamePad->Initialize(pDevice, hWnd, pInput))
	{
		SafeRelease(pGamePad);
		return false;
	}

	(*ppGamePad) = pGamePad;

	return true;
}


GamePadDirectInput::GamePadDirectInput()
	: m_RefCount(1)
	, m_pInput(nullptr)
	, m_pDevice(nullptr)
	, m_hWnd(nullptr)
{
	for (int i = 0; i < 15; ++i)
	{
		m_States[0][i] = m_States[1][i] = InputState::InputState_Released;
		m_PressedDurations[i] = 0.0;
	}

	for (int i = 0; i < AnalogInput::AnalogInput_Num; ++i)
	{
		m_Analogs[i] = 0.0;
	}
}


void GamePadDirectInput::AddRef()
{
	m_RefCount++;
}


void GamePadDirectInput::Release()
{
	m_RefCount--;
	if (m_RefCount == 0) { delete this; }
}


std::uint32_t GamePadDirectInput::GetCount() const
{
	return m_RefCount;
}


void GamePadDirectInput::GetInput(IInput** ppInput)
{
	(*ppInput) = m_pInput;
	if (m_pInput) { m_pInput->AddRef(); }
}


GamePadDirectInput::~GamePadDirectInput()
{
	Terminate();
}


bool GamePadDirectInput::Initialize(LPDIRECTINPUTDEVICE8 pDevice, HWND hWnd, IInput* pInput)
{
	if (!pDevice || !hWnd || !pInput) { return false; }

	m_pDevice = pDevice;
	m_pDevice->AddRef();

	m_pInput = pInput;
	m_pInput->AddRef();

	m_hWnd = hWnd;

	return true;
}


void GamePadDirectInput::Terminate()
{
	SafeRelease(m_pDevice);
	SafeRelease(m_pInput);
}


void GamePadDirectInput::Update(double deltaTime)
{
	if (!m_pDevice) { return; }

	HRESULT hr = m_pDevice->Acquire();
	if (hr != DI_OK && hr != S_FALSE) { return; }
	m_pDevice->Poll();

	DIJOYSTATE2 joystate = { 0 };
	hr = m_pDevice->GetDeviceState(sizeof(joystate), &joystate);
	if (FAILED(hr)) { return; }

	m_Analogs[AnalogInput::AnalogInput_LeftThumbStickX] = static_cast<double>(joystate.lX) / 1000.0;
	m_Analogs[AnalogInput::AnalogInput_LeftThumbStickY] = static_cast<double>(joystate.lY) / 1000.0;
	m_Analogs[AnalogInput::AnalogInput_RightThumbStickX] = static_cast<double>(joystate.lZ) / 1000.0;
	m_Analogs[AnalogInput::AnalogInput_RightThumbStickY] = static_cast<double>(joystate.lRx) / 1000.0;
	m_Analogs[AnalogInput::AnalogInput_LeftTrigger] = static_cast<double>(joystate.lRy) / 1000.0;
	m_Analogs[AnalogInput::AnalogInput_RightTrigger] = static_cast<double>(joystate.lRz) / 1000.0;

	for (std::uint8_t i = 0; i < GamePadButton::GamePadButton_Num + POV::POV_Num; ++i)
	{
		m_States[1][i] = m_States[0][i];

		std::uint8_t pressed = 0x00;

		if (i < GamePadButton::GamePadButton_Num)
		{
			pressed = joystate.rgbButtons[i] & 0x80;
		}
		else
		{
			int direction = (joystate.rgdwPOV[0] + 2200) % 36000 / 4500;
			const std::uint8_t pov[] = { 0x01, 0x09, 0x08, 0x0A, 0x02, 0x06, 0x04, 0x05, 0x00 };

			if (LOWORD(joystate.rgdwPOV[0]) == 0xFFFF)
			{
				// 何も入力されていない状態
				direction = 8;
			}
			pressed = pov[direction] & (0x01 << (i - GamePadButton::GamePadButton_Num));
		}

		if (pressed)
		{
			m_States[0][i] = InputState::InputState_Pressed;
			m_PressedDurations[i] += deltaTime;
		}
		else
		{
			m_States[0][i] = InputState::InputState_Released;
			m_PressedDurations[i] = 0.0;
		}
	}
}


bool GamePadDirectInput::IsAnyPressed() const
{
	for (int i = 0; i < 15; ++i)
	{
		if (m_States[0][i] == InputState::InputState_Pressed) { return true; }
	}
	return false;
}


bool GamePadDirectInput::IsPressed(GamePadButton button) const
{
	if (button >= GamePadButton::GamePadButton_Num) { return false; }
	return m_States[0][button] == InputState::InputState_Pressed;
}


bool GamePadDirectInput::IsPressed(POV pov) const
{
	if (pov >= POV::POV_Num) { return false; }
	return m_States[0][GamePadButton::GamePadButton_Num + pov] == InputState::InputState_Pressed;
}


bool GamePadDirectInput::IsReleased(GamePadButton button) const
{
	if (button >= GamePadButton::GamePadButton_Num) { return false; }
	return m_States[0][button] == InputState::InputState_Released;
}


bool GamePadDirectInput::IsReleased(POV pov) const
{
	if (pov >= POV::POV_Num) { return false; }
	return m_States[0][GamePadButton::GamePadButton_Num + pov] == InputState::InputState_Released;
}


bool GamePadDirectInput::IsFirstPressed(GamePadButton button) const
{
	if (button >= GamePadButton::GamePadButton_Num) { return false; }
	return m_States[0][button] == InputState::InputState_Pressed &&
		m_States[1][button] == InputState::InputState_Released;
}


bool GamePadDirectInput::IsFirstPressed(POV pov) const
{
	if (pov >= POV::POV_Num) { return false; }
	return m_States[0][GamePadButton::GamePadButton_Num + pov] == InputState::InputState_Pressed &&
		m_States[1][GamePadButton::GamePadButton_Num + pov] == InputState::InputState_Released;
}


bool GamePadDirectInput::IsFirstReleased(GamePadButton button) const
{
	if (button >= GamePadButton::GamePadButton_Num) { return false; }
	return m_States[0][button] == InputState::InputState_Released &&
		m_States[1][button] == InputState::InputState_Pressed;
}


bool GamePadDirectInput::IsFirstReleased(POV pov) const
{
	if (pov >= POV::POV_Num) { return false; }
	return m_States[0][GamePadButton::GamePadButton_Num + pov] == InputState::InputState_Released &&
		m_States[1][GamePadButton::GamePadButton_Num + pov] == InputState::InputState_Pressed;
}


double GamePadDirectInput::GetDurationPressed(GamePadButton button) const
{
	if (button >= GamePadButton::GamePadButton_Num) { return 0.0; }
	return m_PressedDurations[button];
}


double GamePadDirectInput::GetDurationPressed(POV pov) const
{
	if (pov >= POV::POV_Num) { return 0.0; }
	return m_PressedDurations[GamePadButton::GamePadButton_Num + pov];
}


double GamePadDirectInput::GetStickValue(AnalogInput analog) const
{
	if (analog >= AnalogInput::AnalogInput_Num) { return 0.0; }
	return m_Analogs[analog];
}


} // namespace Input
} // namespace Grip

