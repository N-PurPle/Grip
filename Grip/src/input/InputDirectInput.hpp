#pragma once

#include <Input.hpp>
#include "DirectInput.hpp"
#include <vector>
#include <atomic>
#include <cstdint>


namespace Grip {
namespace Input {


class InputDirectInput : public IInput
{
public:
	static bool   Create(HINSTANCE hInstance, HWND hWnd, IInput** ppInput);

	bool          CreateKeyboard(std::uint8_t index, IKeyboard** ppKeyboard);

	bool          CreateMouse(std::uint8_t index, IMouse** ppMouse);

	bool          CreateGamePad(std::uint8_t index, IGamePad** ppGamePad);

	void          AddRef() override;

	void          Release() override;
	
	std::uint32_t GetCount() const override;

private:
	static BOOL CALLBACK EnumAndCreateKeyboard(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

	static BOOL CALLBACK EnumAndCreateMouse(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

	static BOOL CALLBACK EnumAndCreateGamePad(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

	static BOOL CALLBACK EnumAndSettingAxesCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);

private:
	InputDirectInput();

	~InputDirectInput();

	bool Initialize(HINSTANCE hInstance, HWND hWnd);

	void Terminate();

private:
	std::atomic<std::uint32_t>        m_RefCount;
	LPDIRECTINPUT8                    m_pInput;
	HWND                              m_hWnd;
	std::vector<LPDIRECTINPUTDEVICE8> m_pKeyboardDevices;
	std::vector<LPDIRECTINPUTDEVICE8> m_pMouseDevices;
	std::vector<LPDIRECTINPUTDEVICE8> m_pGamePadDevices;
};


} // namespace Input
} // namespace Grip

