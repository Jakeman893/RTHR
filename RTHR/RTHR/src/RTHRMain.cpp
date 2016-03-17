#include "pch.h"
#include "RTHRMain.h"
#include "Common\DirectXHelper.h"

// Loads and initializes application assets when the application is loaded.
RTHRMain::RTHRMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources)
{
	// Register to be notified if the Device is lost or recreated
	m_deviceResources->RegisterDeviceNotify(this);

	// TODO: Replace this with your app's content initialization.
	m_fpsTextRenderer = std::unique_ptr<SampleFpsTextRenderer>(new SampleFpsTextRenderer(m_deviceResources));

#ifdef _DEBUG
	m_console->RestoreDevice(deviceResources->GetD3DDeviceContext(), L"Assets\\Fonts\\consolas.spritefont");
#endif

	CreateWindowSizeDependentResources();

#ifdef _DEBUG
	m_console->WriteLine(L"Starting Up...");
#endif

	m_hair = make_unique<Hair>(L"Assets\\HairResources\\hair_vertices.txt", 2, 5);
}

RTHRMain::~RTHRMain()
{
	// Deregister device notification
	m_deviceResources->RegisterDeviceNotify(nullptr);
}

// Updates application state when the window size changes (e.g. device orientation change)
void RTHRMain::CreateWindowSizeDependentResources() 
{
	// TODO: Replace this with the size-dependent initialization of your app's content.
#ifdef _DEBUG
	auto size = m_deviceResources->GetOutputSize();
	m_console->SetWindow(SimpleMath::Viewport::ComputeTitleSafeArea(size.Width, size.Height));
#endif
}

// Updates the application state once per frame.
void RTHRMain::Update() 
{
	// Update scene objects.
	m_timer.Tick([&]()
	{
		// TODO: Replace this with your app's content update functions.
		m_fpsTextRenderer->Update(m_timer);
	});
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool RTHRMain::Render() 
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return false;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	// Reset the viewport to target the whole screen.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	// Reset render targets to the screen.
	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());

	// Clear the back buffer and depth stencil view.
	context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Render the scene objects.
	// TODO: Replace this with your app's content rendering functions.
	m_fpsTextRenderer->Render();

#ifdef _DEBUG
	m_console->Render();
#endif

	return true;
}

// Notifies renderers that device resources need to be released.
void RTHRMain::OnDeviceLost()
{
#ifdef _DEBUG
	m_console->ReleaseDevice();
#endif
	m_fpsTextRenderer->ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void RTHRMain::OnDeviceRestored()
{
#ifdef _DEBUG
	m_console->RestoreDevice(m_deviceResources->GetD3DDeviceContext(), L"../../Fonts/consolas.spritefont");
#endif
	m_fpsTextRenderer->CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}
