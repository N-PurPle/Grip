#pragma once

#include <chrono>


namespace Grip {


class Timer
{
public:
	typedef std::chrono::duration<double, std::nano>        NanoSeconds;
	typedef std::chrono::duration<double, std::milli>       MicroSeconds;
	typedef std::chrono::duration<double, std::milli>       MilliSeconds;
	typedef std::chrono::duration<double>                   Seconds;
	typedef std::chrono::duration<double, std::ratio<60>>   Minutes;
	typedef std::chrono::duration<double, std::ratio<3600>> Hours;
	typedef std::chrono::duration<double>                   Seconds;
	typedef std::chrono::high_resolution_clock              Clock;
	typedef std::chrono::time_point<Clock, MilliSeconds>    TimePoint;

public:
	Timer()
	{
		Reset();
	}

	void Start()
	{
		if (m_Start == m_Last)
		{
			m_Start = Clock::now();
		}
	}

	void Stop()
	{
		m_Last = Clock::now();
	}

	void Reset()
	{
		m_Start = m_Last = Clock::now();
	}

	NanoSeconds  GetNanoSeconds() const { return m_Last - m_Start; }
	MicroSeconds GetMicroSeconds() const { return m_Last - m_Start; }
	MilliSeconds GetMilliSeconds() const { return m_Last - m_Start; }
	Seconds      GetSeconds() const { return m_Last - m_Start; }
	Minutes      GetMinutes() const { return m_Last - m_Start; }
	Minutes      GetHours() const { return m_Last - m_Start; }

private:
	TimePoint m_Start;
	TimePoint m_Last;
};


} // namespace Grip

