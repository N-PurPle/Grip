#pragma once
#include <cstdint>
#include <array>
#include <Windows.h>


namespace Grip {


enum InputState : bool
{
	InputState_Released = false,
	InputState_Pressed  = true,
};


enum Key : std::uint8_t
{
	Key_Escape = 0,
	Key_Delete,
	Key_Space,
	Key_Back,
	Key_Return,
	Key_Tab,

	Key_LControl,
	Key_LAlt,
	Key_LShift,
	Key_LWin,

	Key_RControl,
	Key_RAlt,
	Key_RShift,
	Key_RWin,

	Key_Number1,
	Key_Number2,
	Key_Number3,
	Key_Number4,
	Key_Number5,
	Key_Number6,
	Key_Number7,
	Key_Number8,
	Key_Number9,
	Key_Number0,

	Key_F1,
	Key_F2,
	Key_F3,
	Key_F4,
	Key_F5,
	Key_F6,
	Key_F7,
	Key_F8,
	Key_F9,
	Key_F10,
	Key_F11,
	Key_F12,

	Key_A,
	Key_B,
	Key_C,
	Key_D,
	Key_E,
	Key_F,
	Key_G,
	Key_H,
	Key_I,
	Key_J,
	Key_K,
	Key_L,
	Key_M,
	Key_N,
	Key_O,
	Key_P,
	Key_Q,
	Key_R,
	Key_S,
	Key_T,
	Key_U,
	Key_V,
	Key_W,
	Key_X,
	Key_Y,
	Key_Z,

	Key_Numpad1,
	Key_Numpad2,
	Key_Numpad3,
	Key_Numpad4,
	Key_Numpad5,
	Key_Numpad6,
	Key_Numpad7,
	Key_Numpad8,
	Key_Numpad9,
	Key_Numpad0,
	Key_NumpadEnter,

	Key_Minus,
	Key_Equals,
	Key_Multiply,
	Key_Subtract,
	Key_Add,
	Key_Divide,
	Key_Decimal,

	Key_LBracket,
	Key_RBracket,
	Key_Semicolon,
	Key_Apostrophe,
	Key_Grave,
	Key_BackSlash,
	Key_Comma,
	Key_Period,
	Key_Slash,

	Key_Up,
	Key_Down,
	Key_Left,
	Key_Right,

	Key_PageUp,
	Key_PageDown,
	Key_End,

	Key_SysRq,
	Key_Pause,
	Key_Home,
	Key_Capital,
	Key_NumLock,
	Key_Scroll,
	Key_Insert,
	Key_Apps,

	Key_Num,
};

enum MouseButton : std::uint8_t
{
	MouseButton_0 = 0,
	MouseButton_1,
	MouseButton_2,
	MouseButton_3,
	MouseButton_4,
	MouseButton_5,
	MouseButton_6,
	MouseButton_7,
	MouseButton_Num,
};
enum MouseAxis : std::uint8_t
{
	MouseAxis_X = 0,
	MouseAxis_Y,
	MouseAxis_Scroll,
	MouseAxis_Num,
};
struct IKeyboard
{
	virtual ~IKeyboard() = default;
	virtual void Update(double deltaTime) = 0;
	virtual bool IsAnyPressed() const = 0;
	virtual bool IsPressed(Key key) const = 0;
	virtual bool IsReleased(Key key) const = 0;
	virtual bool IsFirstPressed(Key key) const = 0;
	virtual bool IsFirstReleased(Key key) const = 0;
	virtual double GetDurationPressed(Key key) const = 0;
};
struct IMouse
{
	virtual ~IMouse() = default;
	virtual void Update(double deltaTime) = 0;
	virtual bool IsAnyPressed() const = 0;
	virtual bool IsPressed(MouseButton button) const = 0;
	virtual bool IsReleased(MouseButton button) const = 0;
	virtual bool IsFirstPressed(MouseButton button) const = 0;
	virtual bool IsFirstReleased(MouseButton button) const = 0;
	virtual double GetDurationPressed(MouseButton button) const = 0;
	virtual std::int32_t GetAxisValue(MouseAxis axis) const = 0;
};
enum AnalogInput : std::uint8_t
{
	AnalogInput_LeftThumbStickX = 0,
	AnalogInput_LeftThumbStickY,
	AnalogInput_RightThumbStickX,
	AnalogInput_RightThumbStickY,
	AnalogInput_LeftTrigger,
	AnalogInput_RightTrigger,
	AnalogInput_Num,
};
struct IGamePad
{
	virtual ~IGamePad() = default;
	virtual void Update(double deltaTime) = 0;
	virtual bool IsAnyPressed() const = 0;
	virtual bool IsPressed(std::uint8_t index) const = 0;
	virtual bool IsReleased(std::uint8_t index) const = 0;
	virtual bool IsFirstPressed(std::uint8_t index) const = 0;
	virtual bool IsFirstReleased(std::uint8_t index) const = 0;
	virtual double GetDurationPressed(std::uint8_t index) const = 0;
	virtual double GetStickValue(AnalogInput analog) const = 0;
};
struct IInput
{
	virtual ~IInput() = default;
	virtual void Update(double deltaTime) = 0;
	virtual IKeyboard* GetKeyboard(std::uint8_t index) = 0;
	virtual IMouse*    GetMouse(std::uint8_t index) = 0;
	virtual IGamePad*  GetJoyStick(std::uint8_t index) = 0;
};

IInput* CreateInput(HINSTANCE hInstance, HWND hWnd);

} // namespace Grip


