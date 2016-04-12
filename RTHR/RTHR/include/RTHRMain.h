#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "FpsTextRenderer.h"
#include "SimpleMath.h"
#include "VertexTypes.h"
#include "ConstantBuffer.h"
#ifdef _DEBUG
	#include "TextConsole.h"
#endif // DEBUG
#include "Hair.h"

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
	struct MatrixContants
	{
		Vector3 eyePosition;
		Matrix world;
		XMVECTOR worldInverseTranspose[3];
		Matrix worldViewProj;
		Vector2 renderTargetSize;
	};

	static_assert((sizeof(MatrixContants) % 16) == 0, "Constant buffer must be 16 bit aligned.");

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
		int dirtyFlags;

		// The GPU device
		shared_ptr<DeviceResources> deviceResources;

		// TODO: Replace with your own content renderers.
		unique_ptr<SampleFpsTextRenderer> fpsTextRenderer;

		// Rendering loop timer.
		DX::StepTimer stepTimer;

		// The hair object
		unique_ptr<Hair> hairObj;

		Vector3 eyePos;

		/**************************************************************/
		/*Effect Structures********************************************/
		/**************************************************************/
		// Used for eye, world, view, projection matrices
		EffectMatrices matrices;

		// Constant buffer
		unique_ptr<ConstantBuffer<MatrixContants>> constMatrices;
	};
}