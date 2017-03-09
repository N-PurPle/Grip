#pragma once

#include <Input.hpp>
#include "DirectInput.hpp"
#include <atomic>
#include <cstdint>


namespace Grip {
namespace Input {


class KeyboardDirectInput : public IKeyboard
{
public:
	static void   BuildKeyMapping();

	static bool   Create(LPDIRECTINPUTDEVICE8 pDevice, HWND hWnd, IInput* pInput, IKeyboard** ppKeyboard);

	void          Update(double deltaTime) override;

	bool          IsAnyPressed() const override;

	bool          IsPressed(Key key) const override;

	bool          IsReleased(Key key) const override;

	bool          IsFirstPressed(Key key) const override;

	bool          IsFirstReleased(Key key) const override;

	double        GetDurationPressed(Key key) const override;

	void          AddRef() override;

	void          Release() override;

	std::uint32_t GetCount() const override;

	void          GetInput(IInput** ppInput) override;

private:
	KeyboardDirectInput();

	~KeyboardDirectInput();

	bool Initialize(LPDIRECTINPUTDEVICE8 pDevice, HWND hWnd, IInput* pInput);

	void Terminate();

private:
	std::atomic<std::uint32_t> m_RefCount;
	IInput*                    m_pInput;
	LPDIRECTINPUTDEVICE8       m_pDevice;
	HWND                       m_hWnd;
	InputState                 m_States[2][Key::Key_Num];
	double                     m_PressedDurations[Key::Key_Num];
	static std::uint8_t        s_KeyMapping[Key::Key_Num];
};


} // namespace Input
} // namespace Grip

