#include "pch.h"
#include "RTHRMain.h"
#include "Common\DirectXHelper.h"

using namespace RTHR;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;
using namespace DirectX;

// Loads and initializes application assets when the application is loaded.
RTHRMain::RTHRMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	mDeviceResources(deviceResources)
{
	// Register to be notified if the Device is lost or recreated
	mDeviceResources->RegisterDeviceNotify(this);

	// TODO: Replace this with your app's content initialization.
	m_fpsTextRenderer = std::unique_ptr<SampleFpsTextRenderer>(new SampleFpsTextRenderer(mDeviceResources));
}

RTHRMain::~RTHRMain()
{
	// Deregister device notification
	mDeviceResources->RegisterDeviceNotify(nullptr);
}

// Updates application state when the window size changes (e.g. device orientation change)
void RTHRMain::CreateWindowSizeDependentResources() 
{
	// TODO: Replace this with the size-dependent initialization of your app's content.
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

	auto context = mDeviceResources->GetD3DDeviceContext();

	// Reset the viewport to target the whole screen.
	auto viewport = mDeviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	// Reset render targets to the screen.
	ID3D11RenderTargetView *const targets[1] = { mDeviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, mDeviceResources->GetDepthStencilView());

	// Clear the back buffer and depth stencil view.
	context->ClearRenderTargetView(mDeviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(mDeviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Render the scene objects.
	// TODO: Replace this with your app's content rendering functions.
	m_fpsTextRenderer->Render();

	return true;
}

// Notifies renderers that device resources need to be released.
void RTHRMain::OnDeviceLost()
{
	m_fpsTextRenderer->ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void RTHRMain::OnDeviceRestored()
{
	m_fpsTextRenderer->CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}
