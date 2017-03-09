#include "KeyboardDirectInput.hpp"
#include <Utility.hpp>


namespace Grip {
namespace Input {


std::uint8_t KeyboardDirectInput::s_KeyMapping[Key::Key_Num] = { 255 };


void KeyboardDirectInput::BuildKeyMapping()
{
	s_KeyMapping[Key::Key_Escape] = DIK_ESCAPE;
	s_KeyMapping[Key::Key_Delete] = DIK_DELETE;
	s_KeyMapping[Key::Key_Space] = DIK_SPACE;
	s_KeyMapping[Key::Key_Back] = DIK_BACK;
	s_KeyMapping[Key::Key_Tab] = DIK_TAB;
	s_KeyMapping[Key::Key_Return] = DIK_RETURN;
	s_KeyMapping[Key::Key_LControl] = DIK_LCONTROL;
	s_KeyMapping[Key::Key_LAlt] = DIK_LALT;
	s_KeyMapping[Key::Key_LShift] = DIK_LSHIFT;
	s_KeyMapping[Key::Key_LWin] = DIK_LWIN;
	s_KeyMapping[Key::Key_RControl] = DIK_RCONTROL;
	s_KeyMapping[Key::Key_RAlt] = DIK_RALT;
	s_KeyMapping[Key::Key_RShift] = DIK_RSHIFT;
	s_KeyMapping[Key::Key_RWin] = DIK_RWIN;
	s_KeyMapping[Key::Key_Number1] = DIK_1;
	s_KeyMapping[Key::Key_Number2] = DIK_2;
	s_KeyMapping[Key::Key_Number3] = DIK_3;
	s_KeyMapping[Key::Key_Number4] = DIK_4;
	s_KeyMapping[Key::Key_Number5] = DIK_5;
	s_KeyMapping[Key::Key_Number6] = DIK_6;
	s_KeyMapping[Key::Key_Number7] = DIK_7;
	s_KeyMapping[Key::Key_Number8] = DIK_8;
	s_KeyMapping[Key::Key_Number9] = DIK_9;
	s_KeyMapping[Key::Key_Number0] = DIK_0;
	s_KeyMapping[Key::Key_F1] = DIK_F1;
	s_KeyMapping[Key::Key_F2] = DIK_F2;
	s_KeyMapping[Key::Key_F3] = DIK_F3;
	s_KeyMapping[Key::Key_F4] = DIK_F4;
	s_KeyMapping[Key::Key_F5] = DIK_F5;
	s_KeyMapping[Key::Key_F6] = DIK_F6;
	s_KeyMapping[Key::Key_F7] = DIK_F7;
	s_KeyMapping[Key::Key_F8] = DIK_F8;
	s_KeyMapping[Key::Key_F9] = DIK_F9;
	s_KeyMapping[Key::Key_F10] = DIK_F10;
	s_KeyMapping[Key::Key_F11] = DIK_F11;
	s_KeyMapping[Key::Key_F12] = DIK_F12;
	s_KeyMapping[Key::Key_A] = DIK_A;
	s_KeyMapping[Key::Key_B] = DIK_B;
	s_KeyMapping[Key::Key_C] = DIK_C;
	s_KeyMapping[Key::Key_D] = DIK_D;
	s_KeyMapping[Key::Key_E] = DIK_E;
	s_KeyMapping[Key::Key_F] = DIK_F;
	s_KeyMapping[Key::Key_G] = DIK_G;
	s_KeyMapping[Key::Key_H] = DIK_H;
	s_KeyMapping[Key::Key_I] = DIK_I;
	s_KeyMapping[Key::Key_J] = DIK_J;
	s_KeyMapping[Key::Key_K] = DIK_K;
	s_KeyMapping[Key::Key_L] = DIK_L;
	s_KeyMapping[Key::Key_M] = DIK_M;
	s_KeyMapping[Key::Key_N] = DIK_N;
	s_KeyMapping[Key::Key_O] = DIK_O;
	s_KeyMapping[Key::Key_P] = DIK_P;
	s_KeyMapping[Key::Key_Q] = DIK_Q;
	s_KeyMapping[Key::Key_R] = DIK_R;
	s_KeyMapping[Key::Key_S] = DIK_S;
	s_KeyMapping[Key::Key_T] = DIK_T;
	s_KeyMapping[Key::Key_U] = DIK_U;
	s_KeyMapping[Key::Key_V] = DIK_V;
	s_KeyMapping[Key::Key_W] = DIK_W;
	s_KeyMapping[Key::Key_X] = DIK_X;
	s_KeyMapping[Key::Key_Y] = DIK_Y;
	s_KeyMapping[Key::Key_Z] = DIK_Z;
	s_KeyMapping[Key::Key_Numpad1] = DIK_NUMPAD1;
	s_KeyMapping[Key::Key_Numpad2] = DIK_NUMPAD2;
	s_KeyMapping[Key::Key_Numpad3] = DIK_NUMPAD3;
	s_KeyMapping[Key::Key_Numpad4] = DIK_NUMPAD4;
	s_KeyMapping[Key::Key_Numpad5] = DIK_NUMPAD5;
	s_KeyMapping[Key::Key_Numpad6] = DIK_NUMPAD6;
	s_KeyMapping[Key::Key_Numpad7] = DIK_NUMPAD7;
	s_KeyMapping[Key::Key_Numpad8] = DIK_NUMPAD8;
	s_KeyMapping[Key::Key_Numpad9] = DIK_NUMPAD9;
	s_KeyMapping[Key::Key_Numpad0] = DIK_NUMPAD0;
	s_KeyMapping[Key::Key_NumpadEnter] = DIK_NUMPADENTER;
	s_KeyMapping[Key::Key_Minus] = DIK_MINUS;
	s_KeyMapping[Key::Key_Equals] = DIK_EQUALS;
	s_KeyMapping[Key::Key_Multiply] = DIK_MULTIPLY;
	s_KeyMapping[Key::Key_Subtract] = DIK_SUBTRACT;
	s_KeyMapping[Key::Key_Add] = DIK_ADD;
	s_KeyMapping[Key::Key_Divide] = DIK_DIVIDE;
	s_KeyMapping[Key::Key_Decimal] = DIK_DECIMAL;
	s_KeyMapping[Key::Key_LBracket] = DIK_LBRACKET;
	s_KeyMapping[Key::Key_RBracket] = DIK_RBRACKET;
	s_KeyMapping[Key::Key_Semicolon] = DIK_SEMICOLON;
	s_KeyMapping[Key::Key_Apostrophe] = DIK_APOSTROPHE;
	s_KeyMapping[Key::Key_Grave] = DIK_GRAVE;
	s_KeyMapping[Key::Key_BackSlash] = DIK_BACKSLASH;
	s_KeyMapping[Key::Key_Comma] = DIK_COMMA;
	s_KeyMapping[Key::Key_Period] = DIK_PERIOD;
	s_KeyMapping[Key::Key_Slash] = DIK_SLASH;
	s_KeyMapping[Key::Key_Up] = DIK_UP;
	s_KeyMapping[Key::Key_Down] = DIK_DOWN;
	s_KeyMapping[Key::Key_Left] = DIK_LEFT;
	s_KeyMapping[Key::Key_Right] = DIK_RIGHT;
	s_KeyMapping[Key::Key_PageUp] = DIK_PGUP;
	s_KeyMapping[Key::Key_PageDown] = DIK_PGDN;
	s_KeyMapping[Key::Key_End] = DIK_END;
	s_KeyMapping[Key::Key_SysRq] = DIK_SYSRQ;
	s_KeyMapping[Key::Key_Pause] = DIK_PAUSE;
	s_KeyMapping[Key::Key_Home] = DIK_HOME;
	s_KeyMapping[Key::Key_Capital] = DIK_CAPITAL;
	s_KeyMapping[Key::Key_NumLock] = DIK_NUMLOCK;
	s_KeyMapping[Key::Key_Scroll] = DIK_SCROLL;
	s_KeyMapping[Key::Key_Insert] = DIK_INSERT;
	s_KeyMapping[Key::Key_Apps] = DIK_APPS;
}


bool KeyboardDirectInput::Create(
	LPDIRECTINPUTDEVICE8 pDevice, HWND hWnd, IInput* pInput, IKeyboard** ppKeyboard)
{
	if (!pDevice || !hWnd || !pInput || !ppKeyboard) { return false; }

	KeyboardDirectInput* pKeyboard = new KeyboardDirectInput();

	if (!pKeyboard->Initialize(pDevice, hWnd, pInput))
	{
		SafeRelease(pKeyboard);
		return false;
	}

	(*ppKeyboard) = pKeyboard;

	return true;
}


void KeyboardDirectInput::AddRef()
{
	m_RefCount++;
}


void KeyboardDirectInput::Release()
{
	m_RefCount--;
	if (m_RefCount == 0) { delete this; }
}


std::uint32_t KeyboardDirectInput::GetCount() const
{
	return m_RefCount;
}


void KeyboardDirectInput::GetInput(IInput** ppInput)
{
	(*ppInput) = m_pInput;
	if (m_pInput) { m_pInput->AddRef(); }
}


KeyboardDirectInput::KeyboardDirectInput()
	: m_RefCount(1)
	, m_pInput(nullptr)
	, m_pDevice(nullptr)
	, m_hWnd(nullptr)
{
	for (int i = 0; i < Key::Key_Num; ++i)
	{
		m_States[0][i] = m_States[1][i] = InputState::InputState_Released;
		m_PressedDurations[i] = 0.0;
	}
}


KeyboardDirectInput::~KeyboardDirectInput()
{
	Terminate();
}


bool KeyboardDirectInput::Initialize(LPDIRECTINPUTDEVICE8 pDevice, HWND hWnd, IInput* pInput)
{
	if (!pDevice || !hWnd || !pInput) { return false; }

	m_pDevice = pDevice;
	m_pDevice->AddRef();

	m_pInput = pInput;
	m_pInput->AddRef();

	m_hWnd = hWnd;

	return true;
}


void KeyboardDirectInput::Terminate()
{
	SafeRelease(m_pDevice);
	SafeRelease(m_pInput);
}



void KeyboardDirectInput::Update(double deltaTime)
{
	if (!m_pDevice) { return; }

	HRESULT hr = m_pDevice->Acquire();
	if (hr != DI_OK && hr != S_FALSE) { return; }
	m_pDevice->Poll();

	HWND foreground = ::GetForegroundWindow();
	bool isWindowVisible = ::IsWindowVisible(m_hWnd) != 0;

	if (foreground != m_hWnd || !isWindowVisible) { return; }

	std::uint8_t keyBuffer[256] = { 0 };
	hr = m_pDevice->GetDeviceState(sizeof(keyBuffer), keyBuffer);
	if (FAILED(hr)) { return; }

	for (int i = 0; i < Key::Key_Num; ++i)
	{
		m_States[1][i] = m_States[0][i];

		if (s_KeyMapping[i] != 255 &&
			keyBuffer[s_KeyMapping[i]] & 0x80)
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


bool KeyboardDirectInput::IsAnyPressed() const
{
	for (int i = 0; i < Key::Key_Num; ++i)
	{
		if (m_States[0][i] == InputState::InputState_Pressed) { return true; }
	}
	return false;
}


bool KeyboardDirectInput::IsPressed(Key key) const
{
	if (key >= Key::Key_Num) { return false; }
	return m_States[0][key] == InputState::InputState_Pressed;
}


bool KeyboardDirectInput::IsReleased(Key key) const
{
	if (key >= Key::Key_Num) { return false; }
	return m_States[0][key] == InputState::InputState_Released;
}


bool KeyboardDirectInput::IsFirstPressed(Key key) const
{
	if (key >= Key::Key_Num) { return false; }
	return m_States[0][key] == InputState::InputState_Pressed &&
		m_States[1][key] == InputState::InputState_Released;
}


bool KeyboardDirectInput::IsFirstReleased(Key key) const
{
	if (key >= Key::Key_Num) { return false; }
	return m_States[0][key] == InputState::InputState_Released &&
		m_States[1][key] == InputState::InputState_Pressed;
}


double KeyboardDirectInput::GetDurationPressed(Key key) const
{
	if (key >= Key::Key_Num) { return false; }
	return m_PressedDurations[key];
}


} // namespace Input
} // namespace Grip

