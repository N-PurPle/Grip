#pragma once

#include <Input.hpp>
#include "DirectInput.hpp"
#include <atomic>
#include <cstdint>


namespace Grip {
namespace Input {


class MouseDirectInput : public IMouse
{
public:
	static bool   Create(LPDIRECTINPUTDEVICE8 pDevice, HWND hWnd, IInput* pInput, IMouse** ppMouse);

	void          Update(double deltaTime) override;

	bool          IsAnyPressed() const override;

	bool          IsPressed(MouseButton button) const override;

	bool          IsReleased(MouseButton button) const override;

	bool          IsFirstPressed(MouseButton button) const override;

	bool          IsFirstReleased(MouseButton button) const override;

	double        GetDurationPressed(MouseButton button) const override;

	std::int32_t  GetAxisValue(MouseAxis axis) const override;

	void          AddRef() override;

	void          Release() override;

	std::uint32_t GetCount() const override;

	void          GetInput(IInput** ppInput) override;

private:
	MouseDirectInput();

	~MouseDirectInput();

	bool Initialize(LPDIRECTINPUTDEVICE8 pDevice, HWND hWnd, IInput* pInput);

	void Terminate();

private:
	std::atomic<std::uint32_t> m_RefCount;
	IInput*                    m_pInput;
	LPDIRECTINPUTDEVICE8       m_pDevice;
	HWND                       m_hWnd;
	InputState                 m_States[2][MouseButton::MouseButton_Num];
	double                     m_PressedDurations[MouseButton::MouseButton_Num];
	std::int32_t               m_Axes[MouseAxis::MouseAxis_Num];
};


} // namespace Input
} // namespace Grip

