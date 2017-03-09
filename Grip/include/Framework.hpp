#pragma once
#include <chrono>
#include <cstdint>
#include <Windows.h>


namespace Grip {

struct IGame;
namespace Input {
struct IInput;
}

class Framework
{
	typedef std::chrono::duration<double, std::milli>    MilliSeconds;
	typedef std::chrono::duration<double>                Seconds;
	typedef std::chrono::high_resolution_clock           Clock;
	typedef std::chrono::time_point<Clock, MilliSeconds> TimePoint;

public:
	Framework(HINSTANCE hInstance, HWND hWnd);
	~Framework();

	void SetMaxFps(std::uint32_t fps) { m_frameTime = Seconds(1.0 / fps); }
	void SetEnableSlow(bool enable) { m_enableSlow = enable; }
	void SetEnableFrameLimit(bool enable) { m_enableFrameLimit = enable; }

	std::uint32_t GetFps() const { return m_fps; }
	std::uint32_t GetDrawFps() const { return m_drawFps; }
	Input::IInput* GetInput() { return m_pInput; }
	HWND GetHWND() const { return m_hWnd; }

	bool IsExit() const;
	bool Update();
	void Draw();

private:
	HWND m_hWnd;
	IGame* m_pGame;
	Input::IInput* m_pInput;;
	MilliSeconds m_frameTime;
	TimePoint m_targetTimePoint;
	TimePoint m_secondCounter;
	std::uint32_t m_frameCounter;
	std::uint32_t m_drawFrameCounter;
	std::uint32_t m_fps;
	std::uint32_t m_drawFps;
	std::uint64_t m_totalFrameCounter;
	bool m_skipDrawing;

	bool m_enableSlow;       // スローモード
	bool m_enableFrameLimit; // フレーム制限
};


} // namespace Grip







