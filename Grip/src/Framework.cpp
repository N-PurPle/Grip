#include <Framework.hpp>
#include <Game.hpp>
#include <Input.hpp>


namespace Grip {


Framework::Framework(IGame* pGame, HINSTANCE hInstance, HWND hWnd)
	: m_pGame(pGame)
	, m_pInput(nullptr)
	, m_targetTimePoint()
	, m_secondCounter()
	, m_frameCounter(0)
	, m_drawFrameCounter(0)
	, m_fps(0)
	, m_drawFps(0)
	, m_totalFrameCounter(0)
	, m_skipDrawing(false)
	, m_enableSlow(false)
	, m_enableFrameLimit(true)
{
	m_targetTimePoint = Clock::now();
	SetMaxFps(60);
	m_pInput = CreateInput(hInstance, hWnd);

	if (m_pGame)
	{
		m_pGame->Startup(this);
	}
}


Framework::~Framework()
{
	if (m_pGame)
	{
		m_pGame->Term();
	}
}


bool Framework::Update()
{
	constexpr MilliSeconds maxDeltaTime = MilliSeconds(0.20);

	TimePoint currentTimePoint = Clock::now();

	// フレーム制限
	if (m_enableFrameLimit)
	{
		if (currentTimePoint < m_targetTimePoint + m_frameTime) { return false; }
	}

	// 経過時間
	MilliSeconds deltaTime = m_targetTimePoint - currentTimePoint;
	m_skipDrawing = deltaTime > maxDeltaTime;
	if (m_skipDrawing)
	{
		m_targetTimePoint = currentTimePoint;
	}

	// フレーム時間更新
	if (m_enableSlow)
	{
		constexpr MilliSeconds slowFrameTime = MilliSeconds(30.0);
		m_targetTimePoint += slowFrameTime;
	}
	m_targetTimePoint += m_frameTime;

	// 秒間フレーム数保存
	if (m_secondCounter < currentTimePoint)
	{
		constexpr Seconds second = Seconds(1.0);
		m_fps = m_frameCounter;
		m_drawFps = m_drawFrameCounter;
		m_frameCounter = m_drawFrameCounter = 0;
		m_secondCounter = currentTimePoint + second;
	}

	// フレーム数更新
	++m_frameCounter;
	++m_totalFrameCounter;

	double frameDeltaTime = m_frameTime.count();

	// 入力更新
	if (m_pInput)
	{
		m_pInput->Update(frameDeltaTime);
	}

	// ゲーム更新処理
	if (m_pGame)
	{
		m_pGame->Update(frameDeltaTime);
	}

	return true;
}



void Framework::Draw()
{
	if (m_skipDrawing) return;

	// ゲーム描画処理
	if (m_pGame)
	{
		m_pGame->RenderScene();
		m_pGame->RenderUI();
	}

	// 描画フレーム数更新
	++m_drawFrameCounter;
}



} // namespace Grip
