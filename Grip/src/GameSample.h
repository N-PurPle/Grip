#pragma once

#include <Grip.hpp>
#include <sstream>


class GameSample : public Grip::IGame
{
public:
	GameSample()
		: m_pFramework(nullptr)
		, m_pKeyboard(nullptr)
	{}

	void Startup(Grip::Framework* pFramework) override
	{
		m_pFramework = pFramework;
		m_pKeyboard = m_pFramework->GetInput()->GetKeyboard(0);
	}

	void Shutdown() override
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

	bool IsExit() const override
	{
		if (m_pKeyboard && m_pKeyboard->IsFirstPressed(Grip::Key_Escape))
		{
			return true;
		}
		return false;
	}

private:
	Grip::Framework* m_pFramework;
	Grip::IKeyboard* m_pKeyboard;
	std::uint32_t m_Counter;
};

