#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Sample3DSceneRenderer.h"
#include "FpsTextRenderer.h"
#include "StrandRenderer.h"
#include "SimpleMath.h"
#include "VertexTypes.h"
#include "TextConsole.h"

using namespace DX;
using namespace RTHR;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;
using namespace DirectX;
using std::make_unique;
// Renders Direct2D and 3D content on the screen.
namespace RTHR
{
	class RTHRMain : public DX::IDeviceNotify
	{
	public:
		RTHRMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~RTHRMain();
		void CreateWindowSizeDependentResources();
		void Update();
		bool Render();

		// IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

	private:
		// The GPU device
		shared_ptr<DeviceResources> m_deviceResources;

		// TODO: Replace with your own content renderers.
		unique_ptr<SampleFpsTextRenderer> m_fpsTextRenderer;

		// Rendering loop timer.
		DX::StepTimer m_timer;

#ifdef _DEBUG
		// Create console for debug messages
		unique_ptr<TextConsole> m_console;
#endif
	};
}