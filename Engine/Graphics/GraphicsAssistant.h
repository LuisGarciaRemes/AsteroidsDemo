#pragma once
#include "cGeometry.h"
#include "cEffect.h"
#include "cConstantBuffer.h"
#include "ConstantBufferFormats.h"
#include "cRenderState.h"
#include "cShader.h"
#include "sContext.h"
#include "VertexFormats.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Concurrency/cEvent.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Time/Time.h>
#include <Engine/UserOutput/UserOutput.h>
#include <utility>
#include <Engine\Graphics\cVertexFormat.h>

namespace eae6320
{
	namespace GraphicsAssistant
	{

#if defined( EAE6320_PLATFORM_D3D )
		// In Direct3D "views" are objects that allow a texture to be used a particular way:
		// A render target view allows a texture to have color rendered to it
		extern ID3D11RenderTargetView* s_renderTargetView;
		// A depth/stencil view allows a texture to have depth rendered to it
		extern ID3D11DepthStencilView* s_depthStencilView;
#endif

		void ClearFrame(float r, float b, float g, float a);
		void SwapImages();
		eae6320::cResult InitializeAssetManagers();
		eae6320::cResult InitializeViews(const eae6320::Graphics::sInitializationParameters& i_initializationParameters);
		eae6320::cResult CleanUpAssetManagers();
		void CleanUpViews();
	}
}

