#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Sample3DSceneRenderer.h"
#include "FpsTextRenderer.h"
#include "StrandRenderer.h"

#include "VertexTypes.h"

using namespace DX;

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
		std::shared_ptr<DeviceResources> mDeviceResources;

		// TODO: Replace with your own content renderers.
		std::unique_ptr<SampleFpsTextRenderer> m_fpsTextRenderer;

		// Rendering loop timer.
		DX::StepTimer m_timer;
	};
}