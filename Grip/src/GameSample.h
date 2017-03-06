#pragma once

#include <Grip.hpp>
#include <sstream>


class GameSample : public Grip::IGame
{
public:
	GameSample()
		: m_pFramework(nullptr)
	{}

	void Startup(Grip::Framework* pFramework) override
	{
		m_pFramework = pFramework;
	}

	void Term() override
	{
		m_pFramework = nullptr;
	}

	void Update(double) override
	{
		if (m_pFramework)
		{
			++m_Counter;

			std::uint32_t fps = m_pFramework->GetFps();
			if ((m_Counter % (fps > 0 ? fps : 60)) == 0)
			{
				std::stringstream ss;
				ss << "FPS " << fps << " : " << m_pFramework->GetDrawFps() << "\n";
				OutputDebugStringA(ss.str().c_str());
				m_Counter = 0;
			}
		}
	}

	void RenderScene() override {}

	void RenderUI() override {}

private:
	Grip::Framework* m_pFramework;
	std::uint32_t m_Counter;
};

