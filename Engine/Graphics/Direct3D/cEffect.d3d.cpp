#include "../cEffect.h"

eae6320::Graphics::cEffect::cEffect()
{
}

eae6320::cResult eae6320::Graphics::cEffect::InitializeEffect(std::string vertexEffect, std::string fragmentEffect, uint8_t renderBits)
{
	auto result = eae6320::Results::Success;

	if (!(result = eae6320::Graphics::cShader::s_manager.Load(vertexEffect,
		m_vertexShader, eae6320::Graphics::ShaderTypes::Vertex)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without vertex shader");
		return result;
	}
	if (!(result = eae6320::Graphics::cShader::s_manager.Load(fragmentEffect,
		m_fragmentShader, eae6320::Graphics::ShaderTypes::Fragment)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without fragment shader");
		return result;
	}
	{

		if (!(result = eae6320::Graphics::cRenderState::s_manager.Load(renderBits, m_renderState)))
		{
			EAE6320_ASSERTF(false, "Can't initialize shading data without render state");
			return result;
		}
	}
	return result;
}

eae6320::cResult eae6320::Graphics::cEffect::CleanUp()
{
	auto result = eae6320::Results::Success;

	if (m_vertexShader)
	{
		const auto result_vertexShader = eae6320::Graphics::cShader::s_manager.Release(m_vertexShader);
		if (!result_vertexShader)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_vertexShader;
			}
		}
	}
	if (m_fragmentShader)
	{
		const auto result_fragmentShader = eae6320::Graphics::cShader::s_manager.Release(m_fragmentShader);
		if (!result_fragmentShader)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_fragmentShader;
			}
		}
	}
	if (m_renderState)
	{
		const auto result_renderState = eae6320::Graphics::cRenderState::s_manager.Release(m_renderState);
		if (!result_renderState)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_renderState;
			}
		}
	}

	return result;
}

void eae6320::Graphics::cEffect::Bind()
{
	auto* const direct3dImmediateContext = eae6320::Graphics::sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT(direct3dImmediateContext);
	{
		constexpr ID3D11ClassInstance* const* noInterfaces = nullptr;
		constexpr unsigned int interfaceCount = 0;
		// Vertex shader
		{
			EAE6320_ASSERT(m_vertexShader);
			auto* const shader = eae6320::Graphics::cShader::s_manager.Get(m_vertexShader);
			EAE6320_ASSERT(shader && shader->m_shaderObject.vertex);
			if(shader)
				direct3dImmediateContext->VSSetShader(shader->m_shaderObject.vertex, noInterfaces, interfaceCount);

		}
		// Fragment shader
		{
			EAE6320_ASSERT(m_fragmentShader);
			auto* const shader = eae6320::Graphics::cShader::s_manager.Get(m_fragmentShader);
			EAE6320_ASSERT(shader && shader->m_shaderObject.fragment);
			if (shader)
				direct3dImmediateContext->PSSetShader(shader->m_shaderObject.fragment, noInterfaces, interfaceCount);
		}
	}
	// Render state
	{
		EAE6320_ASSERT(m_renderState);
		auto* const renderState = eae6320::Graphics::cRenderState::s_manager.Get(m_renderState);
		EAE6320_ASSERT(renderState);
		if (renderState)
			renderState->Bind();
	}
}
