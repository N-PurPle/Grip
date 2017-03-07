#include "InputDirectInput.hpp"
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


namespace Grip {


IInput* CreateInput(HINSTANCE hInstance, HWND hWnd)
{
	return new InputDirectInput(hInstance, hWnd);
}


InputDirectInput::InputDirectInput(HINSTANCE hInstance, HWND hWnd)
	: m_pInput(nullptr)
	, m_hWnd(hWnd)
{
	HRESULT hr = DirectInput8Create(
		hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<LPVOID*>(&m_pInput),
		nullptr);
	if (FAILED(hr))
	{
		// TODO. 失敗処理
		return;
	}

	// keyboard
	hr = m_pInput->EnumDevices(DI8DEVCLASS_KEYBOARD, &InputDirectInput::EnumAndCreateKeyboard, this, DIEDFL_ATTACHEDONLY);
	if (FAILED(hr))
	{
		// TODO. 失敗処理
		return;
	}

	// mouse
	hr = m_pInput->EnumDevices(DI8DEVCLASS_POINTER, &InputDirectInput::EnumAndCreateMouse, this, DIEDFL_ATTACHEDONLY);
	if (FAILED(hr))
	{
		// TODO. 失敗処理
		return;
	}

	// joystick
	hr = m_pInput->EnumDevices(DI8DEVCLASS_GAMECTRL, &InputDirectInput::EnumAndCreateGamePad, this, DIEDFL_ATTACHEDONLY);
	if (FAILED(hr))
	{
		// TODO. 失敗処理
		return;
	}

	KeyboardDirectInput::BuildKeyMapping();
}


InputDirectInput::~InputDirectInput()
{
	for (auto& pKeyboard : m_Keyboard)
	{
		delete pKeyboard;
	}

	for (auto& pMouse : m_Mouse)
	{
		delete pMouse;
	}

	for (auto& pGamePad : m_GamePad)
	{
		delete pGamePad;
	}
}


void InputDirectInput::Update(double deltaTime)
{
	for (auto& pKeyboard : m_Keyboard)
	{
		pKeyboard->Update(deltaTime);
	}

	for (auto& pMouse : m_Mouse)
	{
		pMouse->Update(deltaTime);
	}

	for (auto& pGamePad : m_GamePad)
	{
		pGamePad->Update(deltaTime);
	}
}


IKeyboard* InputDirectInput::GetKeyboard(std::uint8_t index)
{
	if (index >= m_Keyboard.size()) return nullptr;
	return m_Keyboard[index];
}

IMouse*    InputDirectInput::GetMouse(std::uint8_t index)
{
	if (index >= m_Mouse.size()) return nullptr;
	return m_Mouse[index];
}

IGamePad*  InputDirectInput::GetJoyStick(std::uint8_t index)
{
	if (index >= m_GamePad.size()) return nullptr;
	return m_GamePad[index];
}


BOOL CALLBACK InputDirectInput::EnumAndCreateKeyboard(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	InputDirectInput* self = reinterpret_cast<InputDirectInput*>(pvRef);
	LPDIRECTINPUT8 pInput = self->m_pInput;
	LPDIRECTINPUTDEVICE8 pDevice = nullptr;
	HWND hWnd = self->m_hWnd;

	HRESULT hr = pInput->CreateDevice(lpddi->guidInstance, &pDevice, nullptr);
	if (FAILED(hr)) { return DIENUM_CONTINUE; }

	hr = pDevice->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr)) { pDevice->Release(); return DIENUM_CONTINUE; }

	hr = pDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) { pDevice->Release(); return DIENUM_CONTINUE; }

	DIPROPDWORD dipdw = { 0 };
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = 10;

	hr = pDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
	if (FAILED(hr)) { pDevice->Release(); return DIENUM_CONTINUE; }

	IKeyboard* pKeyboard = new KeyboardDirectInput(pDevice, hWnd);
	self->m_Keyboard.push_back(pKeyboard);

	return DIENUM_CONTINUE;
}


BOOL CALLBACK InputDirectInput::EnumAndCreateMouse(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	InputDirectInput* self = reinterpret_cast<InputDirectInput*>(pvRef);
	LPDIRECTINPUT8 pInput = self->m_pInput;
	LPDIRECTINPUTDEVICE8 pDevice = nullptr;
	HWND hWnd = self->m_hWnd;

	HRESULT hr = pInput->CreateDevice(lpddi->guidInstance, &pDevice, nullptr);
	if (FAILED(hr)) { return DIENUM_CONTINUE; }

	hr = pDevice->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(hr)) { pDevice->Release(); return DIENUM_CONTINUE; }

	hr = pDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) { pDevice->Release(); return DIENUM_CONTINUE; }

	//	自動センタリング無効
	DIPROPDWORD dipdw = { 0 };
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = DIPROPAUTOCENTER_OFF;

	hr = pDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
	if (FAILED(hr)) { pDevice->Release(); return DIENUM_CONTINUE; }

	IMouse* pMouse = new MouseDirectInput(pDevice, hWnd);
	self->m_Mouse.push_back(pMouse);

	return DIENUM_CONTINUE;
}


BOOL CALLBACK InputDirectInput::EnumAndCreateGamePad(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	InputDirectInput* self = reinterpret_cast<InputDirectInput*>(pvRef);
	LPDIRECTINPUT8 pInput = self->m_pInput;
	LPDIRECTINPUTDEVICE8 pDevice = nullptr;
	HWND hWnd = self->m_hWnd;

	HRESULT hr = pInput->CreateDevice(lpddi->guidInstance, &pDevice, nullptr);
	if (FAILED(hr)) { return DIENUM_CONTINUE; }

	hr = pDevice->SetDataFormat(&c_dfDIJoystick2);
	if (FAILED(hr)) { pDevice->Release(); return DIENUM_CONTINUE; }

	hr = pDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) { pDevice->Release(); return DIENUM_CONTINUE; }

	//	自動センタリング無効
	DIPROPDWORD	dipdw = { 0 };
	dipdw.diph.dwSize = sizeof(dipdw);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = DIPROPAUTOCENTER_OFF;

	hr = pDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
	if (FAILED(hr)) { pDevice->Release(); return DIENUM_CONTINUE; }

	hr = pDevice->EnumObjects(
		&InputDirectInput::EnumAndSettingAxesCallback,
		reinterpret_cast<LPVOID>(&pDevice),
		DIDFT_AXIS);
	if (FAILED(hr)) { pDevice->Release(); return DIENUM_CONTINUE; }

	IGamePad* pGamePad = new GamePadDirectInput(pDevice);
	self->m_GamePad.push_back(pGamePad);

	return DIENUM_CONTINUE;
}


BOOL CALLBACK InputDirectInput::EnumAndSettingAxesCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
	// 軸範囲を設定（-1000～1000）
	DIPROPRANGE diprg = { 0 };

	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwObj = lpddoi->dwType;
	diprg.diph.dwHow = DIPH_BYID;
	diprg.lMin = -1000;
	diprg.lMax = +1000;

	LPDIRECTINPUTDEVICE8 pDevice = (LPDIRECTINPUTDEVICE8)pvRef;
	HRESULT hr = pDevice->SetProperty(DIPROP_RANGE, &diprg.diph);

	return (FAILED(hr)) ? DIENUM_STOP : DIENUM_CONTINUE;
}



std::uint8_t KeyboardDirectInput::s_KeyMapping[Key::Key_Num] = { 255 };
KeyboardDirectInput::KeyboardDirectInput(LPDIRECTINPUTDEVICE8 pDevice, HWND hWnd)
	: m_pDevice(pDevice)
	, m_hWnd(hWnd)
{
	for (int i = 0; i < Key::Key_Num; ++i)
	{
		m_States[0][i] = m_States[1][i] = InputState::InputState_Released;
	}
}


KeyboardDirectInput::~KeyboardDirectInput()
{
	if (m_pDevice)
	{
		m_pDevice->Release();
	}
}


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



MouseDirectInput::MouseDirectInput(LPDIRECTINPUTDEVICE8 pDevice, HWND hWnd)
	: m_pDevice(pDevice)
	, m_hWnd(hWnd)
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
	if (m_pDevice)
	{
		m_pDevice->Release();
	}
}


void MouseDirectInput::Update(double deltaTime)
{
	if (!m_pDevice) { return; }

	HRESULT hr = m_pDevice->Acquire();
	if (hr != DI_OK && hr != S_FALSE) { return; }
	m_pDevice->Poll();

	HWND foreground = ::GetForegroundWindow();
	bool isWindowVisible = ::IsWindowVisible(m_hWnd) != 0;

	if (foreground != m_hWnd || !isWindowVisible) { return; }

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



GamePadDirectInput::GamePadDirectInput(LPDIRECTINPUTDEVICE8 pDevice)
	: m_pDevice(pDevice)
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


GamePadDirectInput::~GamePadDirectInput()
{
	if (m_pDevice)
	{
		m_pDevice->Release();
	}
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


} // namespace Grip




