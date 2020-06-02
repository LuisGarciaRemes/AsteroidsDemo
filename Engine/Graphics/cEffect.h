#pragma once
#include "sContext.h"
#include "cRenderState.h"
#include "cShader.h"
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine\Assets\ReferenceCountedAssets.h>

#if defined( EAE6320_PLATFORM_D3D )
#include "..//Graphics/Direct3D/Includes.h"
#include "cVertexFormat.h"
#elif defined( EAE6320_PLATFORM_GL )
#include "..//Graphics/OpenGL/Includes.h"
#endif

namespace eae6320
{
	namespace Graphics
	{
	class cEffect
	{
	public:
		static eae6320::cResult CreateEffect(const std::string& i_path, cEffect*& o_cEffect)
		{		
			return Load(i_path,o_cEffect);
		}
		void Bind();
		EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()
	private:
		static eae6320::cResult FactoryHelper(cEffect*& o_cEffect, std::string vertexEffect, std::string fragmentEffect, uint8_t renderBits)
		{
			o_cEffect = new cEffect();
			return o_cEffect->InitializeEffect(vertexEffect, fragmentEffect, renderBits);
		}
		static eae6320::cResult Load(const std::string& i_path, cEffect*& o_cEffect);
		eae6320::cResult CleanUp();
		eae6320::cResult InitializeEffect(std::string vertexEffect, std::string fragmentEffect, uint8_t renderBits);
		EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cEffect)
			EAE6320_ASSETS_DECLAREREFERENCECOUNT()
		cEffect();
		~cEffect()
		{
			CleanUp();
		}
#if defined( EAE6320_PLATFORM_GL )
		GLuint m_programId;
#endif
		eae6320::Graphics::cShader::Handle m_vertexShader;
		eae6320::Graphics::cShader::Handle m_fragmentShader;
		eae6320::Graphics::cRenderState::Handle m_renderState;
	};
	}
}