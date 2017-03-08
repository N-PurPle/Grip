#include "GameSampleD3D11.hpp"

#include <vector>
#include <map>

#include <d3d11_4.h>
#include <dxgi1_5.h>

#pragma comment(lib,"d3d11.lib")

namespace {

typedef std::vector<IDXGIAdapter3*>               IDXGIAdapter3Array;
typedef std::vector<IDXGIOutput5*>                IDXGIOutput5Array;
typedef std::map<std::uint8_t, IDXGIOutput5Array> IDXGIOutput5ArrayMap;


IDXGIFactory5*             g_pDXGIFactory = nullptr;
IDXGIAdapter3Array         g_apDXGIAdapters;
IDXGIOutput5ArrayMap       g_apDXGIOutputs;
ID3D11Device4*             g_pD3D11Device = nullptr;
ID3D11DeviceContext3*      g_pD3D11DeviceContext = nullptr;
D3D_FEATURE_LEVEL          g_FeatureLevel = D3D_FEATURE_LEVEL_11_0;
IDXGISwapChain4*           g_pDXGISwapChain = nullptr;
ID3D11RenderTargetView1*   g_pD3D11RenderTarget = nullptr;


constexpr UINT kScreenWidth = 1280;
constexpr UINT kScreenHeight = 720;


} // unnamed namespace

using namespace Grip;


// 初期化
bool D3D11Startup(HWND hWnd, bool enableDebug)
{
	// ファクトリを生成
	{
		UINT flags = 0;
		if (enableDebug) { flags |= DXGI_CREATE_FACTORY_DEBUG; }
		HRESULT hr = CreateDXGIFactory2(flags, IID_PPV_ARGS(&g_pDXGIFactory));
		if (FAILED(hr)) { return false; }
	}

	// アダプター取得
	{
		UINT i = 0;
		IDXGIAdapter1* pDXGIAdapter1 = nullptr;
		while (SUCCEEDED(g_pDXGIFactory->EnumAdapters1(i, &pDXGIAdapter1)))
		{
			if (pDXGIAdapter1)
			{
				IDXGIAdapter3* pDXGIAdapter3 = nullptr;
				HRESULT hr = pDXGIAdapter1->QueryInterface(IID_PPV_ARGS(&pDXGIAdapter3));
				if (SUCCEEDED(hr))
				{
					g_apDXGIAdapters.push_back(pDXGIAdapter3);
				}
			}
			SafeRelease(pDXGIAdapter1);
			++i;
		}

		if (g_apDXGIAdapters.empty())
		{
			i = 0;
			pDXGIAdapter1 = nullptr;
			while (SUCCEEDED(g_pDXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&pDXGIAdapter1))))
			{
				if (pDXGIAdapter1)
				{
					IDXGIAdapter3* pDXGIAdapter3 = nullptr;
					HRESULT hr = pDXGIAdapter1->QueryInterface(IID_PPV_ARGS(&pDXGIAdapter3));
					if (SUCCEEDED(hr))
					{
						g_apDXGIAdapters.push_back(pDXGIAdapter3);
					}
				}
				SafeRelease(pDXGIAdapter1);
			}
		}
		if (g_apDXGIAdapters.empty()) { return false; }
	}

	// ディスプレイ取得
	{
		for (std::uint8_t iAdapterIndex = 0; iAdapterIndex < g_apDXGIAdapters.size(); ++iAdapterIndex)
		{
			IDXGIAdapter3* pDXGIAdapter3 = g_apDXGIAdapters[iAdapterIndex];

			UINT i = 0;
			IDXGIOutput* pDXGIOutput = nullptr;
			IDXGIOutput5Array apDXGIOutput5Array;
			while (SUCCEEDED(pDXGIAdapter3->EnumOutputs(i, &pDXGIOutput)))
			{
				IDXGIOutput5* pDXGIOutput5 = nullptr;
				HRESULT hr = pDXGIOutput->QueryInterface(IID_PPV_ARGS(&pDXGIOutput5));
				if (SUCCEEDED(hr))
				{
					apDXGIOutput5Array.push_back(pDXGIOutput5);
				}
				SafeRelease(pDXGIOutput);
				++i;
			}

			if (!apDXGIOutput5Array.empty())
			{
				g_apDXGIOutputs.insert(
					IDXGIOutput5ArrayMap::value_type(iAdapterIndex, apDXGIOutput5Array)
				);
			}
		}
		if (g_apDXGIOutputs.empty()) { return false; }
	}
	
	// デバイス作成
	{
		// 機能レベル
		D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		if (enableDebug) { createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG; }

		ID3D11Device* pD3D11Device = nullptr;
		ID3D11DeviceContext* pD3D11DeviceContext = nullptr;
		HRESULT hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			createDeviceFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			&pD3D11Device,
			&g_FeatureLevel,
			&pD3D11DeviceContext);
		if (FAILED(hr)) { return false; }

		hr = pD3D11Device->QueryInterface(IID_PPV_ARGS(&g_pD3D11Device));
		SafeRelease(pD3D11Device);
		if (FAILED(hr)) { SafeRelease(pD3D11DeviceContext); return false; }

		hr = pD3D11DeviceContext->QueryInterface(IID_PPV_ARGS(&g_pD3D11DeviceContext));
		SafeRelease(pD3D11DeviceContext);
		if (FAILED(hr)) { return false; }
	}

	// スワップチェインの作成
	{
		IDXGISwapChain* pDXGISwapChain = nullptr;

		DXGI_SWAP_CHAIN_DESC desc = { 0 };
		desc.BufferDesc.Width = kScreenWidth;
		desc.BufferDesc.Height = kScreenHeight;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
		desc.BufferCount = 2;
		desc.Windowed = TRUE;
		desc.OutputWindow = hWnd;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		HRESULT hr = g_pDXGIFactory->CreateSwapChain(g_pD3D11Device, &desc, &pDXGISwapChain);
		if (FAILED(hr)) { return false; }

		hr = pDXGISwapChain->QueryInterface(IID_PPV_ARGS(&g_pDXGISwapChain));
		SafeRelease(pDXGISwapChain);
		if (FAILED(hr)) { return false; }
	}

	// バックバッファ取得
	{
		ID3D11Texture2D1* pD3D11Texture2D = nullptr;
		HRESULT hr = g_pDXGISwapChain->GetBuffer(0, IID_PPV_ARGS(&pD3D11Texture2D));
		if (FAILED(hr)) { return false; }

		hr = g_pD3D11Device->CreateRenderTargetView1(pD3D11Texture2D, nullptr, &g_pD3D11RenderTarget);
		SafeRelease(pD3D11Texture2D);
		if (FAILED(hr)) { return false; }
	}

	return true;
}


// 解放
void D3D11Shutdown()
{
	for (auto& pDXGIOutputPair : g_apDXGIOutputs)
	{
		for (auto& pDXGIOutput : pDXGIOutputPair.second)
		{
			SafeRelease(pDXGIOutput);
		}
	}

	for (auto& pDXGIAdapter : g_apDXGIAdapters)
	{
		SafeRelease(pDXGIAdapter);
	}

	SafeRelease(g_pD3D11RenderTarget);
	SafeRelease(g_pDXGIFactory);
	SafeRelease(g_pDXGISwapChain);
	SafeRelease(g_pD3D11DeviceContext);
	SafeRelease(g_pD3D11Device);
}


// 描画開始
void D3D11BeginDraw()
{
	// バックバッファをクリアする
	const float kClearColor[] = { 0.0f, 0.0f, 0.6f, 1.0f };
	g_pD3D11DeviceContext->ClearRenderTargetView(g_pD3D11RenderTarget, kClearColor);
}


// 描画
void D3D11DrawScene()
{

}


// 描画終了
void D3D11EndDraw()
{

}


// コマンド実行 と Present
void D3D11Present()
{
	g_pDXGISwapChain->Present(0, 0);
}


// 描画完了を待つ
void D3D11WaitDrawDone()
{

}


GameSampleD3D11::GameSampleD3D11()
	: m_pFramework(nullptr)
	, m_pKeyboard(nullptr)
	, m_Counter(0)
{}


void GameSampleD3D11::Startup(Grip::Framework* pFramework)
{
	m_pFramework = pFramework;
	m_pKeyboard = m_pFramework->GetInput()->GetKeyboard(0);
	bool result = D3D11Startup(m_pFramework->GetHWND(), true);
	OutputDebugStringA(result ? "Succeeded\n" : "Failed\n");
}


void GameSampleD3D11::Shutdown()
{
	m_pFramework = nullptr;
	D3D11Shutdown();
}


void GameSampleD3D11::Update(double)
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


void GameSampleD3D11::RenderScene()
{
	D3D11BeginDraw();
	D3D11DrawScene();
	D3D11EndDraw();
	D3D11Present();
	D3D11WaitDrawDone();
}


void GameSampleD3D11::RenderUI()
{

}


bool GameSampleD3D11::IsExit() const
{
	if (m_pKeyboard && m_pKeyboard->IsFirstPressed(Grip::Key_Escape))
	{
		return true;
	}
	return false;
}



