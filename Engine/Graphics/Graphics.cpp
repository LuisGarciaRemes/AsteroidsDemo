// Includes
//=========

#include "Graphics.h"
#include "GraphicsAssistant.h"
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
#include <MyGame_\MyGame\cGameObject.h>
#include "Engine/Graphics/cCamera.h"

// Static Data Initialization
//===========================

namespace
{	
	// Constant buffer object
	eae6320::Graphics::cConstantBuffer s_constantBuffer_frame(eae6320::Graphics::ConstantBufferTypes::Frame);
	eae6320::Graphics::cConstantBuffer s_constantBuffer_draw(eae6320::Graphics::ConstantBufferTypes::DrawCall);

	// Submission Data
	//----------------

	// This struct's data is populated at submission time;
	// it must cache whatever is necessary in order to render a frame
	struct sDataRequiredToRenderAFrame
	{
		eae6320::Graphics::ConstantBufferFormats::sFrame constantData_frame;
		eae6320::Graphics::ConstantBufferFormats::sDrawCall constantData_draw[NUMOBJECTS];
		float constantData_r = 0.0f;
		float constantData_g = 0.0f;
		float constantData_b = 0.0f;
		float constantData_a = 1.0f;
		eae6320::Graphics::cGeometry* constantData_Geometry[NUMOBJECTS];
		eae6320::Graphics::cEffect* constantData_Effect[NUMOBJECTS];
	};
	// In our class there will be two copies of the data required to render a frame:
	//	* One of them will be in the process of being populated by the data currently being submitted by the application loop thread
	//	* One of them will be fully populated and in the process of being rendered from in the render thread
	// (In other words, one is being produced while the other is being consumed)
	sDataRequiredToRenderAFrame s_dataRequiredToRenderAFrame[2];
	auto* s_dataBeingSubmittedByApplicationThread = &s_dataRequiredToRenderAFrame[0];
	auto* s_dataBeingRenderedByRenderThread = &s_dataRequiredToRenderAFrame[1];
	// The following two events work together to make sure that
	// the main/render thread and the application loop thread can work in parallel but stay in sync:
	// This event is signaled by the application loop thread when it has finished submitting render data for a frame
	// (the main/render thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenAllDataHasBeenSubmittedFromApplicationThread;
	// This event is signaled by the main/render thread when it has swapped render data pointers.
	// This means that the renderer is now working with all the submitted data it needs to render the next frame,
	// and the application loop thread can start submitting data for the following frame
	// (the application loop thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenDataForANewFrameCanBeSubmittedFromApplicationThread;
}

// Interface
//==========

// Submission
//-----------

void eae6320::Graphics::SubmitElapsedTime(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& constantData_frame = s_dataBeingSubmittedByApplicationThread->constantData_frame;
	constantData_frame.g_elapsedSecondCount_systemTime = i_elapsedSecondCount_systemTime;
	constantData_frame.g_elapsedSecondCount_simulationTime = i_elapsedSecondCount_simulationTime;
}

void eae6320::Graphics::SubmitBackBufferColor(const float m_r, const float m_g, const float m_b, const float m_a)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	s_dataBeingSubmittedByApplicationThread->constantData_r = m_r;
	s_dataBeingSubmittedByApplicationThread->constantData_g = m_g;
	s_dataBeingSubmittedByApplicationThread->constantData_b = m_b;
	s_dataBeingSubmittedByApplicationThread->constantData_a = m_a;
}

void eae6320::Graphics::SubmitGameObjectsToRender(eae6320::MyGame::cGameObject* gameobject[])
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);

	for (int i = 0; i < NUMOBJECTS; i++)
	{
		if (gameobject[i] != nullptr && gameobject[i]->IsEnabled())
		{
			if (gameobject[i]->GetGeometry() != nullptr)
			{
				s_dataBeingSubmittedByApplicationThread->constantData_Geometry[i] = gameobject[i]->GetGeometry();
				s_dataBeingSubmittedByApplicationThread->constantData_Geometry[i]->IncrementReferenceCount();
			}

			if (gameobject[i]->GetEffect() != nullptr)
			{
				s_dataBeingSubmittedByApplicationThread->constantData_Effect[i] = gameobject[i]->GetEffect();
				s_dataBeingSubmittedByApplicationThread->constantData_Effect[i]->IncrementReferenceCount();
			}

			s_dataBeingSubmittedByApplicationThread->constantData_draw[i].g_transform_localToWorld = gameobject[i]->GetMatrix();

		}
	}
}

void eae6320::Graphics::SubmitCamera(eae6320::Graphics::cCamera* i_camera)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);

	s_dataBeingSubmittedByApplicationThread->constantData_frame.g_transform_cameraToProjected = i_camera->GetCameraToProjected();
	s_dataBeingSubmittedByApplicationThread->constantData_frame.g_transform_worldToCamera = i_camera->GetWorldToCamera();
}

eae6320::cResult eae6320::Graphics::WaitUntilDataForANewFrameCanBeSubmitted(const unsigned int i_timeToWait_inMilliseconds)
{
	return Concurrency::WaitForEvent(s_whenDataForANewFrameCanBeSubmittedFromApplicationThread, i_timeToWait_inMilliseconds);
}

eae6320::cResult eae6320::Graphics::SignalThatAllDataForAFrameHasBeenSubmitted()
{
	return s_whenAllDataHasBeenSubmittedFromApplicationThread.Signal();
}

// Render
//-------

void eae6320::Graphics::RenderFrame()
{
	// Wait for the application loop to submit data to be rendered
	{
		const auto result = Concurrency::WaitForEvent(s_whenAllDataHasBeenSubmittedFromApplicationThread);
		if (result)
		{
			// Switch the render data pointers so that
			// the data that the application just submitted becomes the data that will now be rendered
			std::swap(s_dataBeingSubmittedByApplicationThread, s_dataBeingRenderedByRenderThread);
			// Once the pointers have been swapped the application loop can submit new data
			const auto result = s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Signal();
			if (!result)
			{
				EAE6320_ASSERTF(false, "Couldn't signal that new graphics data can be submitted");
				Logging::OutputError("Failed to signal that new render data can be submitted");
				UserOutput::Print("The renderer failed to signal to the application that new graphics data can be submitted."
					" The application is probably in a bad state and should be exited");
				return;
			}
		}
		else
		{
			EAE6320_ASSERTF(false, "Waiting for the graphics data to be submitted failed");
			Logging::OutputError("Waiting for the application loop to submit data to be rendered failed");
			UserOutput::Print("The renderer failed to wait for the application to submit data to be rendered."
				" The application is probably in a bad state and should be exited");
			return;
		}
	}
	

	EAE6320_ASSERT(s_dataBeingRenderedByRenderThread);
	eae6320::GraphicsAssistant::ClearFrame(s_dataBeingRenderedByRenderThread->constantData_r, s_dataBeingRenderedByRenderThread->constantData_g, s_dataBeingRenderedByRenderThread->constantData_b, s_dataBeingRenderedByRenderThread->constantData_a);

	// Update the frame constant buffer
	{
		// Copy the data from the system memory that the application owns to GPU memory
		auto& constantData_frame = s_dataBeingRenderedByRenderThread->constantData_frame;
		s_constantBuffer_frame.Update(&constantData_frame);		
	}

	for (int i = 0; i < NUMOBJECTS; i++)
	{
		if (s_dataBeingRenderedByRenderThread->constantData_Effect[i] != nullptr && s_dataBeingRenderedByRenderThread->constantData_Geometry[i] != nullptr)
		{
			auto& constantData_draw = s_dataBeingRenderedByRenderThread->constantData_draw;
			s_constantBuffer_draw.Update(&constantData_draw[i]);
			s_dataBeingRenderedByRenderThread->constantData_Effect[i]->Bind();
			s_dataBeingRenderedByRenderThread->constantData_Geometry[i]->DrawGeometry();
		}
	}

	eae6320::GraphicsAssistant::SwapImages();

	for (int i = 0; i < NUMOBJECTS; i++)
	{
		if (s_dataBeingRenderedByRenderThread->constantData_Effect[i] != nullptr && s_dataBeingRenderedByRenderThread->constantData_Geometry[i] != nullptr)
		{
			s_dataBeingRenderedByRenderThread->constantData_Effect[i]->DecrementReferenceCount();
			s_dataBeingRenderedByRenderThread->constantData_Effect[i] = nullptr;
			s_dataBeingRenderedByRenderThread->constantData_Geometry[i]->DecrementReferenceCount();
			s_dataBeingRenderedByRenderThread->constantData_Geometry[i] = nullptr;
		}
	}
}

// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::Initialize(const sInitializationParameters& i_initializationParameters)
{
	auto result = Results::Success;

	// Initialize the platform-specific context
	if (!(result = sContext::g_context.Initialize(i_initializationParameters)))
	{
		EAE6320_ASSERTF(false, "Can't initialize Graphics without context");
		return result;
	}
	// Initialize the asset managers
	{
		result = eae6320::GraphicsAssistant::InitializeAssetManagers();
	}
	// Initialize the platform-independent graphics objects
	{
		if (result = s_constantBuffer_frame.Initialize())
		{
			// There is only a single frame constant buffer that is reused
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_frame.Bind(
				// In our class both vertex and fragment shaders use per-frame constant data
				ShaderTypes::Vertex | ShaderTypes::Fragment);
		}
		else
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without frame constant buffer");
			return result;
		}
	}
	// Initialize the platform-independent graphics objects
	{
		if (result = s_constantBuffer_draw.Initialize())
		{
			// There is only a single frame constant buffer that is reused
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_draw.Bind(
				// In our class both vertex and fragment shaders use per-frame constant data
				ShaderTypes::Vertex | ShaderTypes::Fragment);
		}
		else
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without frame constant buffer");
			return result;
		}
	}
	// Initialize the events
	{
		if (!(result = s_whenAllDataHasBeenSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without event for when data has been submitted from the application thread");
			return result;
		}
		if (!(result = s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled,
			Concurrency::EventState::Signaled)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without event for when data can be submitted from the application thread");
			return result;
		}
	}
	// Initialize the views
	{
		result = eae6320::GraphicsAssistant::InitializeViews(i_initializationParameters);
	}
	
	return result;
}

eae6320::cResult eae6320::Graphics::CleanUp()
{
	auto result = Results::Success;

	eae6320::GraphicsAssistant::CleanUpViews();

	for (int i = 0; i < NUMOBJECTS; i++)
	{
		if (s_dataBeingRenderedByRenderThread->constantData_Effect[i] != nullptr && s_dataBeingRenderedByRenderThread->constantData_Geometry[i] != nullptr)
		{
			s_dataBeingRenderedByRenderThread->constantData_Effect[i]->DecrementReferenceCount();
			s_dataBeingRenderedByRenderThread->constantData_Geometry[i]->DecrementReferenceCount();
		}
	}

	for (int i = 0; i < NUMOBJECTS; i++)
	{
		if (s_dataBeingSubmittedByApplicationThread->constantData_Effect[i] != nullptr && s_dataBeingSubmittedByApplicationThread->constantData_Geometry[i] != nullptr)
		{
			s_dataBeingSubmittedByApplicationThread->constantData_Effect[i]->DecrementReferenceCount();
			s_dataBeingSubmittedByApplicationThread->constantData_Geometry[i]->DecrementReferenceCount();
		}
	}

	{
		const auto result_constantBuffer_frame = s_constantBuffer_frame.CleanUp();
		if (!result_constantBuffer_frame)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_constantBuffer_frame;
			}
		}
	}

	{
		const auto result_constantBuffer_draw = s_constantBuffer_draw.CleanUp();
		if (!result_constantBuffer_draw)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_constantBuffer_draw;
			}
		}
	}

	{
		result = eae6320::GraphicsAssistant::CleanUpAssetManagers();
	}

	{
		const auto result_context = sContext::g_context.CleanUp();
		if (!result_context)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_context;
			}
		}
	}

	return result;
}


