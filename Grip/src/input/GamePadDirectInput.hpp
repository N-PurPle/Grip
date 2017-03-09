#pragma once

#include <Input.hpp>
#include "DirectInput.hpp"
#include <atomic>
#include <cstdint>


namespace Grip {
namespace Input {


class GamePadDirectInput : public IGamePad
{
public:
	static bool   Create(LPDIRECTINPUTDEVICE8 pDevice, HWND hWnd, IInput* pInput, IGamePad** ppGamePad);

	void          Update(double deltaTime) override;

	bool          IsAnyPressed() const override;

	bool          IsPressed(GamePadButton button) const override;
	bool          IsPressed(POV pov) const override;

	bool          IsReleased(GamePadButton button) const override;
	bool          IsReleased(POV pov) const override;

	bool          IsFirstPressed(GamePadButton button) const override;
	bool          IsFirstPressed(POV pov) const override;

	bool          IsFirstReleased(GamePadButton button) const override;
	bool          IsFirstReleased(POV pov) const override;

	double        GetDurationPressed(GamePadButton button) const override;
	double        GetDurationPressed(POV pov) const override;

	double        GetStickValue(AnalogInput analog) const override;

	void          AddRef() override;

	void          Release() override;

	std::uint32_t GetCount() const override;

	void          GetInput(IInput** ppInput) override;

private:
	GamePadDirectInput();

	~GamePadDirectInput();

	bool Initialize(LPDIRECTINPUTDEVICE8 pDevice, HWND hWnd, IInput* pInput);

	void Terminate();

private:
	std::atomic<std::uint32_t> m_RefCount;
	IInput*                    m_pInput;
	LPDIRECTINPUTDEVICE8       m_pDevice;
	HWND                       m_hWnd;
	double                     m_Analogs[AnalogInput::AnalogInput_Num]; // -1.0 ~ 1.0
	InputState                 m_States[2][GamePadButton::GamePadButton_Num + POV::POV_Num];
	double                     m_PressedDurations[GamePadButton::GamePadButton_Num + POV::POV_Num];
};


} // namespace Input
} // namespace Grip

