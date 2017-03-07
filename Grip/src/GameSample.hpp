#pragma once

#include <Grip.hpp>
#include <sstream>


class GameSample : public Grip::IGame
{
public:
	GameSample();

	void Startup(Grip::Framework* pFramework) override;

	void Shutdown() override;

	void Update(double) override;

	void RenderScene() override;

	void RenderUI() override;

	bool IsExit() const override;

private:
	Grip::Framework* m_pFramework;
	Grip::IKeyboard* m_pKeyboard;
	std::uint32_t    m_Counter;
};

