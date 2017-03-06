#pragma once


namespace Grip {

class Framework;
class IGame
{
public:
	virtual ~IGame() = default;
	virtual void Startup(Framework* pFramework) = 0;
	virtual void Term() = 0;
	virtual void Update(double deltaTime) = 0;
	virtual void RenderScene() = 0;
	virtual void RenderUI() = 0;
};


} // namespace Grip


