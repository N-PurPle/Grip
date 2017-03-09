
#include "InputDirectInput.hpp"
#include "KeyboardDirectInput.hpp"
#include "MouseDirectInput.hpp"
#include "GamePadDirectInput.hpp"
#include <Utility.hpp>





namespace Grip {
namespace Input {


bool CreateInput(HINSTANCE hInstance, HWND hWnd, IInput** ppInput)
{
	return InputDirectInput::Create(hInstance, hWnd, ppInput);
}


bool InputDirectInput::Create(HINSTANCE hInstance, HWND hWnd, IInput** ppInput)
{
	if (!hInstance || !hWnd || !ppInput) { return false; }

	InputDirectInput* pInput = new InputDirectInput();

	if (!pInput->Initialize(hInstance, hWnd))
	{
		SafeRelease(pInput);
		return false;
	}

	(*ppInput) = pInput;
	return true;
}


InputDirectInput::InputDirectInput()
	: m_RefCount(1)
	, m_pInput(nullptr)
	, m_hWnd(nullptr)
{
	
}


InputDirectInput::~InputDirectInput()
{
	Terminate();
}


bool InputDirectInput::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	if (!hInstance || !hWnd) { return false; }

	m_hWnd = hWnd;

	HRESULT hr = DirectInput8Create(
		hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<LPVOID*>(&m_pInput),
		nullptr);
	if (FAILED(hr))
	{
		// TODO. 失敗処理
		return false;
	}

	// keyboard
	hr = m_pInput->EnumDevices(DI8DEVCLASS_KEYBOARD, &InputDirectInput::EnumAndCreateKeyboard, this, DIEDFL_ATTACHEDONLY);
	if (FAILED(hr))
	{
		// TODO. 失敗処理
		return false;
	}

	// mouse
	hr = m_pInput->EnumDevices(DI8DEVCLASS_POINTER, &InputDirectInput::EnumAndCreateMouse, this, DIEDFL_ATTACHEDONLY);
	if (FAILED(hr))
	{
		// TODO. 失敗処理
		return false;
	}

	// joystick
	hr = m_pInput->EnumDevices(DI8DEVCLASS_GAMECTRL, &InputDirectInput::EnumAndCreateGamePad, this, DIEDFL_ATTACHEDONLY);
	if (FAILED(hr))
	{
		// TODO. 失敗処理
		return false;
	}

	KeyboardDirectInput::BuildKeyMapping();

	return true;
}


void InputDirectInput::Terminate()
{
	for (auto& pKeyboard : m_pKeyboardDevices)
	{
		SafeRelease(pKeyboard);
	}

	for (auto& pMouse : m_pMouseDevices)
	{
		SafeRelease(pMouse);
	}

	for (auto& pGamePad : m_pGamePadDevices)
	{
		SafeRelease(pGamePad);
	}
}


bool InputDirectInput::CreateKeyboard(std::uint8_t index, IKeyboard** ppKeyboard)
{
	if (index >= m_pKeyboardDevices.size()) { return false; }

	return KeyboardDirectInput::Create(
		m_pKeyboardDevices[index],
		m_hWnd,
		this,
		ppKeyboard
	);
}


bool InputDirectInput::CreateMouse(std::uint8_t index, IMouse** ppMouse)
{
	if (index >= m_pMouseDevices.size()) { return false; }

	return MouseDirectInput::Create(
		m_pMouseDevices[index],
		m_hWnd,
		this,
		ppMouse
	);
}


bool InputDirectInput::CreateGamePad(std::uint8_t index, IGamePad** ppGamePad)
{
	if (index >= m_pGamePadDevices.size()) { return false; }

	return GamePadDirectInput::Create(
		m_pGamePadDevices[index],
		m_hWnd,
		this,
		ppGamePad
	);
}


void InputDirectInput::AddRef()
{
	m_RefCount++;
}


void InputDirectInput::Release()
{
	m_RefCount--;
	if (m_RefCount == 0) { delete this; }
}


std::uint32_t InputDirectInput::GetCount() const
{
	return m_RefCount;
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
	if (FAILED(hr)) { SafeRelease(pDevice); return DIENUM_CONTINUE; }

	hr = pDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) { SafeRelease(pDevice); return DIENUM_CONTINUE; }

	DIPROPDWORD dipdw = { 0 };
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = 10;

	hr = pDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
	if (FAILED(hr)) { SafeRelease(pDevice); return DIENUM_CONTINUE; }

	self->m_pKeyboardDevices.push_back(pDevice);

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
	if (FAILED(hr)) { SafeRelease(pDevice); return DIENUM_CONTINUE; }

	hr = pDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) { SafeRelease(pDevice); return DIENUM_CONTINUE; }

	//	自動センタリング無効
	DIPROPDWORD dipdw = { 0 };
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = DIPROPAUTOCENTER_OFF;

	hr = pDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
	if (FAILED(hr)) { SafeRelease(pDevice); return DIENUM_CONTINUE; }

	self->m_pMouseDevices.push_back(pDevice);

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
	if (FAILED(hr)) { SafeRelease(pDevice); return DIENUM_CONTINUE; }

	hr = pDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) { SafeRelease(pDevice); return DIENUM_CONTINUE; }

	//	自動センタリング無効
	DIPROPDWORD	dipdw = { 0 };
	dipdw.diph.dwSize = sizeof(dipdw);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = DIPROPAUTOCENTER_OFF;

	hr = pDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
	if (FAILED(hr)) { SafeRelease(pDevice); return DIENUM_CONTINUE; }

	hr = pDevice->EnumObjects(
		&InputDirectInput::EnumAndSettingAxesCallback,
		reinterpret_cast<LPVOID>(&pDevice),
		DIDFT_AXIS);
	if (FAILED(hr)) { SafeRelease(pDevice); return DIENUM_CONTINUE; }

	self->m_pGamePadDevices.push_back(pDevice);

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





} // namespace Input
} // namespace Grip




