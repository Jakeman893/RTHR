#include "pch.h"
#include "StrandRenderer.h"

#include "..\Common\DirectXHelper.h"

using namespace RTHR;

using namespace DirectX;
using namespace Windows::Foundation;

StrandRenderer::StrandRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_indexCount(0),
	m_tracking(false),
	m_deviceResources(deviceResources)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void StrandRenderer::CreateDeviceDependentResources()
{
}

void StrandRenderer::CreateWindowSizeDependentResources()
{
}

void StrandRenderer::ReleaseDeviceDependentResources()
{
}

void StrandRenderer::Update(DX::StepTimer const & timer)
{
}

void StrandRenderer::Render()
{
}

void StrandRenderer::StartTracking()
{
}

void StrandRenderer::TrackingUpdate(float positionX)
{
}

void StrandRenderer::StopTracking()
{
}
