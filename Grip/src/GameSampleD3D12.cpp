#include "GameSampleD3D12.hpp"

#include <vector>
#include <map>
#include <d3d12.h>
#include <dxgi1_5.h>
#include <d3dcompiler.h>


#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

namespace {

typedef std::vector<IDXGIAdapter3*>               IDXGIAdapter3Array;
typedef std::vector<IDXGIOutput5*>                IDXGIOutput5Array;
typedef std::map<std::uint8_t, IDXGIOutput5Array> IDXGIOutput5ArrayMap;

bool                       g_bTearingSupport = false;
IDXGIFactory5*             g_pDXGIFactory = nullptr;
IDXGIAdapter3Array         g_apDXGIAdapters;
IDXGIOutput5ArrayMap       g_apDXGIOutputs;
ID3D12Device1*             g_pD3D12Device = nullptr;
ID3D12CommandQueue*        g_pD3D12CommandQueue = nullptr;
ID3D12CommandAllocator*    g_pD3D12CommandAllocator = nullptr;
ID3D12GraphicsCommandList* g_pD3D12GraphicsCommandList = nullptr;
IDXGISwapChain4*           g_pDXGISwapChain = nullptr;
UINT                       g_iFrameBufferIndex = 0;
ID3D12DescriptorHeap*      g_pD3D12RTVHeap = nullptr;
UINT                       g_iRTVDescriptorSize = 0;
ID3D12Resource*            g_apD3D12RenderTargets[2] = { nullptr };
D3D12_VIEWPORT             g_D3D12Viewport = { 0 };
D3D12_RECT                 g_D3D12ScissorRect = { 0 };
ID3D12Fence*               g_pD3D12Fence = nullptr;
UINT64                     g_iFenceValue = 0;
HANDLE                     g_pFenceEvent = nullptr;


constexpr UINT kScreenWidth = 1280;
constexpr UINT kScreenHeight = 720;

} // unnamed namespace

using namespace Grip;


// 初期化
bool D3D12Startup(HWND hWnd, bool enableDebug)
{
	// デバッグレイヤーを有効化
	if(enableDebug)
	{
		ID3D12Debug1* pD3D12Debug = nullptr;
		HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&pD3D12Debug));
		if (SUCCEEDED(hr))
		{
			pD3D12Debug->EnableDebugLayer();
		}
		SafeRelease(pD3D12Debug);
	}

	// DXGIファクトリを生成
	{
		UINT flags = 0;
		if (enableDebug) { flags |= DXGI_CREATE_FACTORY_DEBUG; }
		HRESULT hr = CreateDXGIFactory2(flags, IID_PPV_ARGS(&g_pDXGIFactory));
		if (FAILED(hr)) { return false; }
	}

	// ティアリング許可
	{
		BOOL allowTearing = FALSE;
		HRESULT hr = g_pDXGIFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING,&allowTearing,sizeof(allowTearing));
		if (SUCCEEDED(hr)) { g_bTearingSupport = (allowTearing == TRUE); }
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
		HRESULT hr = D3D12CreateDevice(g_apDXGIAdapters[0], D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&g_pD3D12Device));
		if (FAILED(hr)) { return false; }
	}

	// コマンドキュー作成
	{
		// 最初なので直接コマンドキュー
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;  // GPUタイムアウトが有効
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;  // 直接コマンドキュー

		/*
		通常は D3D12_COMMAND_LIST_TYPE_DIRECT を設定しますが、別のパイプラインを使用する際には
		D3D12_COMMAND_LIST_TYPE_COMPUTE や D3D12_COMMAND_LIST_TYPE_COPY を設定します。
		*/
		HRESULT hr = g_pD3D12Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pD3D12CommandQueue));
		if (FAILED(hr)) { return false; }
	}

	// コマンドアロケータ作成
	{
		HRESULT hr = g_pD3D12Device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_pD3D12CommandAllocator));
		if (FAILED(hr)) { return false; }
	}

	// グラフィックスコマンドリスト作成
	{
		HRESULT hr = g_pD3D12Device->CreateCommandList(
			0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_pD3D12CommandAllocator, nullptr, IID_PPV_ARGS(&g_pD3D12GraphicsCommandList));
		if (FAILED(hr)) { return false; }
		
		// コマンドリストを一旦クローズしておく
		// ループ先頭がコマンドリストクローズ状態として処理されているため？
		g_pD3D12GraphicsCommandList->Close();
	}

	// スワップチェインを作成
	{
		DXGI_SWAP_CHAIN_DESC desc = {};
		desc.BufferCount = 2;   // フレームバッファとバックバッファで2枚
		desc.BufferDesc.Width = kScreenWidth;
		desc.BufferDesc.Height = kScreenHeight;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.OutputWindow = hWnd;
		desc.SampleDesc.Count = 1;
		desc.Windowed = true;

		IDXGISwapChain* pDXGISwapChain = nullptr;
		HRESULT hr = g_pDXGIFactory->CreateSwapChain(g_pD3D12CommandQueue, &desc, &pDXGISwapChain);
		if (FAILED(hr)) { return false; }

		hr = pDXGISwapChain->QueryInterface(IID_PPV_ARGS(&g_pDXGISwapChain));
		SafeRelease(pDXGISwapChain);
		if (FAILED(hr)) { return false; }

		g_iFrameBufferIndex = g_pDXGISwapChain->GetCurrentBackBufferIndex();
	}

	// スワップチェインをRenderTargetとして使用するためのDescriptorHeapを作成
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = 2;  // フレームバッファとバックバッファ
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;  // RenderTargetView
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // シェーダからアクセスしないのでNONEでOK

		HRESULT hr = g_pD3D12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pD3D12RTVHeap));
		if (FAILED(hr)) { return false; }

		g_iRTVDescriptorSize = g_pD3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// スワップチェインのバッファを先に作成したDescriptorHeapに登録する
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = g_pD3D12RTVHeap->GetCPUDescriptorHandleForHeapStart();
		for (int i = 0; i < 2; i++)
		{
			HRESULT hr = g_pDXGISwapChain->GetBuffer(i, IID_PPV_ARGS(&g_apD3D12RenderTargets[i]));
			if (FAILED(hr)) { return false; }

			g_pD3D12Device->CreateRenderTargetView(g_apD3D12RenderTargets[i], nullptr, handle);
			handle.ptr += g_iRTVDescriptorSize;
		}
	}

	// 同期をとるためのフェンスを作成する
	{
		HRESULT hr = g_pD3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_pD3D12Fence));
		if (FAILED(hr)) { return false; }
		g_iFenceValue = 1;

		g_pFenceEvent = ::CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		if (!g_pFenceEvent) { return false; }
	}

	return true;
}


// 解放
void D3D12Shutdown()
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

	SafeRelease(g_pD3D12CommandQueue);
	SafeRelease(g_pD3D12CommandAllocator);
	SafeRelease(g_pD3D12GraphicsCommandList);
	SafeRelease(g_pDXGISwapChain);
	SafeRelease(g_pD3D12Device);
	SafeRelease(g_pD3D12Fence);
	SafeRelease(g_apD3D12RenderTargets[0]);
	SafeRelease(g_apD3D12RenderTargets[1]);
	SafeRelease(g_pD3D12RTVHeap);
	SafeRelease(g_pDXGIFactory);
	g_pFenceEvent = nullptr;
}


// 描画開始
void D3D12BeginDraw()
{
	HRESULT hr;

	// コマンドアロケータをリセット
	hr = g_pD3D12CommandAllocator->Reset();
	if (FAILED(hr)) { return; }

	// コマンドリストをリセット
	hr = g_pD3D12GraphicsCommandList->Reset(g_pD3D12CommandAllocator, nullptr);
	if (FAILED(hr)) { return; }

	// バックバッファが描画ターゲットとして使用できるようになるまで待つ
	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;					// バリアはリソースの状態遷移に対して設置
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = g_apD3D12RenderTargets[g_iFrameBufferIndex];			// リソースは描画ターゲット
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;			// 遷移前はPresent
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;		// 遷移後は描画ターゲット
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	g_pD3D12GraphicsCommandList->ResourceBarrier(1, &barrier);

	// バックバッファを描画ターゲットとして設定する
	D3D12_CPU_DESCRIPTOR_HANDLE handle = g_pD3D12RTVHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += (g_iFrameBufferIndex * g_iRTVDescriptorSize);
	g_pD3D12GraphicsCommandList->OMSetRenderTargets(1, &handle, false, nullptr);

	// ビューポートとシザーボックスを設定
	g_pD3D12GraphicsCommandList->RSSetViewports(1, &g_D3D12Viewport);
	g_pD3D12GraphicsCommandList->RSSetScissorRects(1, &g_D3D12ScissorRect);

	// バックバッファをクリアする
	const float kClearColor[] = { 0.0f, 0.0f, 0.6f, 1.0f };
	g_pD3D12GraphicsCommandList->ClearRenderTargetView(handle, kClearColor, 0, nullptr);
}


// 描画
void D3D12DrawScene()
{

}


// 描画終了
void D3D12EndDraw()
{
	// バックバッファの描画完了を待つためのバリアを設置
	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;					// バリアはリソースの状態遷移に対して設置
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = g_apD3D12RenderTargets[g_iFrameBufferIndex];			// リソースは描画ターゲット
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	// 遷移前は描画ターゲット
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;			// 遷移後はPresent
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	g_pD3D12GraphicsCommandList->ResourceBarrier(1, &barrier);

	// コマンドリストをクローズする
	g_pD3D12GraphicsCommandList->Close();
}


// コマンド実行 と Present
void D3D12Present()
{
	// コマンドリストを実行する
	ID3D12CommandList* ppD3D12CommandLists[] = { g_pD3D12GraphicsCommandList };
	g_pD3D12CommandQueue->ExecuteCommandLists(_countof(ppD3D12CommandLists), ppD3D12CommandLists);

	// スワップチェインのPresent
	g_pDXGISwapChain->Present(1, 0);
}


// 描画完了を待つ
void D3D12WaitDrawDone()
{
	// 現在のFence値がコマンド終了後にFenceに書き込まれるようにする
	UINT64 fvalue = g_iFenceValue;
	g_pD3D12CommandQueue->Signal(g_pD3D12Fence, fvalue);
	g_iFenceValue++;

	// まだコマンドキューが終了していないことを確認する
	// ここまででコマンドキューが終了してしまうとイベントが一切発火されなくなるのでチェックしている
	if (g_pD3D12Fence->GetCompletedValue() < fvalue)
	{
		// このFenceにおいて、fvalue の値になったらイベントを発火させる
		g_pD3D12Fence->SetEventOnCompletion(fvalue, g_pFenceEvent);
		// イベントが発火するまで待つ
		::WaitForSingleObject(g_pFenceEvent, INFINITE);
	}
	g_iFrameBufferIndex = g_pDXGISwapChain->GetCurrentBackBufferIndex();
}


GameSampleD3D12::GameSampleD3D12()
	: m_pFramework(nullptr)
	, m_pKeyboard(nullptr)
	, m_Counter(0)
{}


void GameSampleD3D12::Startup(Grip::Framework* pFramework)
{
	m_pFramework = pFramework;
	m_pFramework->GetInput()->CreateKeyboard(0, &m_pKeyboard);
	bool result = D3D12Startup(m_pFramework->GetHWND(), true);
	OutputDebugStringA(result ? "Succeeded\n" : "Failed\n");
}


void GameSampleD3D12::Shutdown()
{
	m_pFramework = nullptr;
	SafeRelease(m_pKeyboard);
	D3D12Shutdown();
}


void GameSampleD3D12::Update(double deltaTime)
{
	if (m_pKeyboard)
	{
		m_pKeyboard->Update(deltaTime);
	}

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


void GameSampleD3D12::RenderScene()
{
	D3D12BeginDraw();
	D3D12DrawScene();
	D3D12EndDraw();
	D3D12Present();
	D3D12WaitDrawDone();
}


void GameSampleD3D12::RenderUI()
{

}


bool GameSampleD3D12::IsExit() const
{
	if (m_pKeyboard && m_pKeyboard->IsFirstPressed(Grip::Input::Key_Escape))
	{
		return true;
	}
	return false;
}



