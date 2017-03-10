#include "MouseDirectInput.hpp"
#include <Utility.hpp>


namespace Grip {
namespace Input {


bool MouseDirectInput::Create(LPDIRECTINPUTDEVICE8 pDevice, HWND hWnd, IInput* pInput, IMouse** ppMouse)
{
	if (!pDevice || !hWnd || !pInput || !ppMouse) { return false; }

	MouseDirectInput* pMouse = new MouseDirectInput();

	if (!pMouse->Initialize(pDevice, hWnd, pInput))
	{
		SafeRelease(pMouse);
		return false;
	}

	(*ppMouse) = pMouse;

	return true;
}


void MouseDirectInput::AddRef()
{
	m_RefCount++;
}


void MouseDirectInput::Release()
{
	m_RefCount--;
	if (m_RefCount == 0) { delete this; }
}


std::uint32_t MouseDirectInput::GetCount() const
{
	return m_RefCount;
}


void MouseDirectInput::GetInput(IInput** ppInput)
{
	(*ppInput) = m_pInput;
	if (m_pInput) { m_pInput->AddRef(); }
}


MouseDirectInput::MouseDirectInput()
	: m_RefCount(1)
	, m_pInput(nullptr)
	, m_pDevice(nullptr)
	, m_hWnd(nullptr)
{
	for (int i = 0; i < MouseButton::MouseButton_Num; ++i)
	{
		m_States[0][i] = m_States[1][i] = InputState::InputState_Released;
		m_PressedDurations[i] = 0.0;
	}

	for (int i = 0; i < MouseAxis::MouseAxis_Num; ++i)
	{
		m_Axes[i] = 0;
	}
}


MouseDirectInput::~MouseDirectInput()
{
	Terminate();
}


bool MouseDirectInput::Initialize(LPDIRECTINPUTDEVICE8 pDevice, HWND hWnd, IInput* pInput)
{
	if (!pDevice || !hWnd || !pInput) { return false; }

	m_pDevice = pDevice;
	m_pDevice->AddRef();

	m_pInput = pInput;
	m_pInput->AddRef();

	m_hWnd = hWnd;

	return true;
}


void MouseDirectInput::Terminate()
{
	SafeRelease(m_pDevice);
	SafeRelease(m_pInput);
}


void MouseDirectInput::Update(double deltaTime)
{
	if (!m_pDevice) { return; }

	HWND foreground = ::GetForegroundWindow();
	bool isWindowVisible = ::IsWindowVisible(m_hWnd) != 0;

	if (foreground != m_hWnd || !isWindowVisible) { return; }

	HRESULT hr = m_pDevice->Acquire();
	if (hr != DI_OK && hr != S_FALSE) { return; }
	m_pDevice->Poll();

	DIMOUSESTATE2 mousestate2 = { 0 };
	hr = m_pDevice->GetDeviceState(sizeof(mousestate2), &mousestate2);
	if (FAILED(hr)) { return; }

	for (int i = 0; i < MouseButton::MouseButton_Num; ++i)
	{
		m_States[1][i] = m_States[0][i];

		if (mousestate2.rgbButtons[i] & 0x80)
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


bool MouseDirectInput::IsAnyPressed() const
{
	for (int i = 0; i < MouseButton::MouseButton_Num; ++i)
	{
		if (m_States[0][i] == InputState::InputState_Pressed) { return true; }
	}
	return false;
}


bool MouseDirectInput::IsPressed(MouseButton button) const
{
	if (button >= MouseButton::MouseButton_Num) { return false; }
	return m_States[0][button] == InputState::InputState_Pressed;
}


bool MouseDirectInput::IsReleased(MouseButton button) const
{
	if (button >= MouseButton::MouseButton_Num) { return false; }
	return m_States[0][button] == InputState::InputState_Released;
}


bool MouseDirectInput::IsFirstPressed(MouseButton button) const
{
	if (button >= MouseButton::MouseButton_Num) { return false; }
	return m_States[0][button] == InputState::InputState_Pressed &&
		m_States[1][button] == InputState::InputState_Released;
}


bool MouseDirectInput::IsFirstReleased(MouseButton button) const
{
	if (button >= MouseButton::MouseButton_Num) { return false; }
	return m_States[0][button] == InputState::InputState_Released &&
		m_States[1][button] == InputState::InputState_Pressed;
}


double MouseDirectInput::GetDurationPressed(MouseButton button) const
{
	if (button >= MouseButton::MouseButton_Num) { return 0.0; }
	return m_PressedDurations[button];
}


std::int32_t MouseDirectInput::GetAxisValue(MouseAxis axis) const
{
	if (axis >= MouseAxis::MouseAxis_Num) return 0;
	return m_Axes[axis];
}


} // namespace Input
} // namespace Grip

