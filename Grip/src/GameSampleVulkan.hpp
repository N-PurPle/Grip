#pragma once

#include <Grip.hpp>
#include <sstream>


class GameSampleVulkan : public Grip::IGame
{
public:
	GameSampleVulkan();

	void Startup(Grip::Framework* pFramework) override;

	void Shutdown() override;

	void Update(double) override;

	void RenderScene() override;

	void RenderUI() override;

	bool IsExit() const override;

private:
	Grip::Framework* m_pFramework;
	Grip::Input::IKeyboard* m_pKeyboard;
	std::uint32_t    m_Counter;
};

