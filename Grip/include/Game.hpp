#pragma once


#define GRIP_GAME_ENTRY(game_class) \
	Grip::IGame* Grip::CreateGame() \
	{ \
		return new game_class();\
	}

namespace Grip {

class Framework;
struct IGame
{
	virtual ~IGame() = default;
	virtual void Startup(Framework* pFramework) = 0;
	virtual void Shutdown() = 0;
	virtual void Update(double deltaTime) = 0;
	virtual void RenderScene() = 0;
	virtual void RenderUI() = 0;
	virtual bool IsExit() const = 0;
};


IGame* CreateGame();


} // namespace Grip


