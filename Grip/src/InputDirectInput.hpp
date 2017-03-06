#pragma once
#include <Input.hpp>
#define DIRECTINPUT_VERSION  0x0800
#include <dinput.h>
#include <vector>
#include <Windows.h>


namespace Grip {


class InputDirectInput : public IInput
{
public:
	InputDirectInput(HINSTANCE hInstance, HWND hWnd);

	~InputDirectInput();

	void Update(double deltaTime) override;

	IKeyboard* GetKeyboard(std::uint8_t index) override;

	IMouse*    GetMouse(std::uint8_t index) override;

	IGamePad*  GetJoyStick(std::uint8_t index) override;

private:
	static BOOL CALLBACK EnumAndCreateKeyboard(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

	static BOOL CALLBACK EnumAndCreateMouse(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

	static BOOL CALLBACK EnumAndCreateGamePad(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

	static BOOL CALLBACK EnumAndSettingAxesCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);

private:
	LPDIRECTINPUT8          m_pInput;
	HWND                    m_hWnd;
	std::vector<IKeyboard*> m_Keyboard;
	std::vector<IMouse*>    m_Mouse;
	std::vector<IGamePad*>  m_GamePad;
};

class KeyboardDirectInput : public IKeyboard
{
public:
	KeyboardDirectInput(LPDIRECTINPUTDEVICE8 pDevice, HWND hWnd);

	~KeyboardDirectInput();

	static void BuildKeyMapping();

	void Update(double deltaTime) override;

	bool IsAnyPressed() const override;

	bool IsPressed(Key key) const override;

	bool IsReleased(Key key) const override;

	bool IsFirstPressed(Key key) const override;

	bool IsFirstReleased(Key key) const override;

	double GetDurationPressed(Key key) const override;

private:
	LPDIRECTINPUTDEVICE8 m_pDevice;
	HWND                 m_hWnd;
	InputState           m_States[2][Key::Key_Num];
	double               m_PressedDurations[Key::Key_Num];
	static std::uint8_t  s_KeyMapping[Key::Key_Num];
};

class MouseDirectInput : public IMouse
{
public:
	MouseDirectInput(LPDIRECTINPUTDEVICE8 pDevice, HWND hWnd);

	~MouseDirectInput();

	void Update(double deltaTime) override;

	bool IsAnyPressed() const override;

	bool IsPressed(MouseButton button) const override;

	bool IsReleased(MouseButton button) const override;

	bool IsFirstPressed(MouseButton button) const override;

	bool IsFirstReleased(MouseButton button) const override;

	double GetDurationPressed(MouseButton button) const override;

	std::int32_t GetAxisValue(MouseAxis axis) const override;

private:
	LPDIRECTINPUTDEVICE8 m_pDevice;
	HWND                 m_hWnd;
	InputState           m_States[2][MouseButton::MouseButton_Num];
	double               m_PressedDurations[MouseButton::MouseButton_Num];
	std::int32_t         m_Axes[MouseAxis::MouseAxis_Num];
};

class GamePadDirectInput : public IGamePad
{
public:
	GamePadDirectInput(LPDIRECTINPUTDEVICE8 pDevice);

	~GamePadDirectInput();

	void Update(double deltaTime) override;

	bool IsAnyPressed() const override;

	bool IsPressed(std::uint8_t index) const override;

	bool IsReleased(std::uint8_t index) const override;

	bool IsFirstPressed(std::uint8_t index) const override;

	bool IsFirstReleased(std::uint8_t index) const override;

	double GetDurationPressed(std::uint8_t index) const override;

	double GetStickValue(AnalogInput analog) const override;

private:
	LPDIRECTINPUTDEVICE8 m_pDevice;
	double               m_Analogs[AnalogInput::AnalogInput_Num]; // -1.0 ~ 1.0
	InputState           m_States[2][15];
	double               m_PressedDurations[15];
};








} // namespace Grip



