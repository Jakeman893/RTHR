#include "pch.h"
#include "RTHRMain.h"
#include "Common\DirectXHelper.h"

// Loads and initializes application assets when the application is loaded.
RTHRMain::RTHRMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	deviceResources(deviceResources)
{
	// Register to be notified if the Device is lost or recreated
	deviceResources->RegisterDeviceNotify(this);

	// TODO: Replace this with your app's content initialization.
	fpsTextRenderer = std::unique_ptr<SampleFpsTextRenderer>(new SampleFpsTextRenderer(deviceResources));

#ifdef _DEBUG
	m_console->RestoreDevice(deviceResources->GetD3DDeviceContext(), L"Assets\\Fonts\\consolas.spritefont");
#endif

	CreateWindowSizeDependentResources();

#ifdef _DEBUG
	m_console->WriteLine(L"Starting Up...");
	m_console->WriteLine(L"Making geometry");
#endif

	matrices.world = Matrix::Identity;

	eyePos = Vector3(2, 2, 2);

	matrices.view = Matrix::CreateLookAt(eyePos,
										Vector3::Zero,
										Vector3::UnitY);

	hairObj = make_unique<Hair>(GeometryType::SPHERE, deviceResources, 1.0f, 4, 5);
}

RTHRMain::~RTHRMain()
{
	// Deregister device notification
	deviceResources->RegisterDeviceNotify(nullptr);
}

// Updates application state when the window size changes (e.g. device orientation change)
void RTHRMain::CreateWindowSizeDependentResources() 
{
	// TODO: Replace this with the size-dependent initialization of your app's content.
	auto size = deviceResources->GetOutputSize();

#ifdef _DEBUG
	m_console->SetWindow(SimpleMath::Viewport::ComputeTitleSafeArea(size.Width, size.Height));
#endif


	matrices.projection = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f, (size.Width/size.Height), FLT_EPSILON, 10.f);
	constMatrices = make_unique<ConstantBuffer<MatrixContants>>(deviceResources->GetD3DDevice());
	
	dirtyFlags = EffectDirtyFlags::WorldViewProj;
}

// Updates the application state once per frame.
void RTHRMain::Update() 
{
	// Update scene objects.
	stepTimer.Tick([&]()
	{
		// If the world view projection matrix is dirty and needs to be cleaned
		if (dirtyFlags & EffectDirtyFlags::WorldViewProj)
		{
			XMMATRIX worldViewProjConstant;
			matrices.SetConstants(dirtyFlags, worldViewProjConstant);

			MatrixContants update = MatrixContants();
			update.eyePosition = eyePos;
			update.world = matrices.world;
			update.worldViewProj = worldViewProjConstant;
			auto size = deviceResources->GetOutputSize();
			update.renderTargetSize = Vector2(size.Width, size.Height);

			constMatrices->SetData(deviceResources->GetD3DDeviceContext(), update);
		}

		// TODO: Replace this with your app's content update functions.
		fpsTextRenderer->Update(stepTimer);
	});
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool RTHRMain::Render() 
{
#pragma region RenderInit
	// Don't try to render anything before the first Update.
	if (stepTimer.GetFrameCount() == 0)
	{
		return false;
	}

	auto context = deviceResources->GetD3DDeviceContext();

	// Reset the viewport to target the whole screen.
	auto viewport = deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	// Reset render targets to the screen.
	ID3D11RenderTargetView *const targets[1] = { deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, deviceResources->GetDepthStencilView());

	// Clear the back buffer and depth stencil view.
	context->ClearRenderTargetView(deviceResources->GetBackBufferRenderTargetView(), Colors::MintCream);
	context->ClearDepthStencilView(deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
#pragma endregion

#pragma region Render Calls
	// Render the scene objects.
	fpsTextRenderer->Render();

#ifdef _DEBUG
	m_console->Render();
#endif

	hairObj->Draw(&matrices, eyePos, constMatrices->GetBufferAddress(), Colors::RosyBrown);

#pragma endregion
	return true;
}

// Notifies renderers that device resources need to be released.
void RTHRMain::OnDeviceLost()
{
	hairObj->Reset();
#ifdef _DEBUG
	m_console->WriteLine(L"Device Lost");
	m_console->ReleaseDevice();
#endif
	fpsTextRenderer->ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void RTHRMain::OnDeviceRestored()
{
#ifdef _DEBUG
	m_console->RestoreDevice(deviceResources->GetD3DDeviceContext(), L"../../Fonts/consolas.spritefont");
	m_console->WriteLine(L"Device Restored");
#endif
	fpsTextRenderer->CreateDeviceDependentResources();
	hairObj->CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}
