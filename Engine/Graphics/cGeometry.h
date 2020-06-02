#pragma once
#include "sContext.h"
#include <Engine/Asserts/Asserts.h>
#if defined( EAE6320_PLATFORM_D3D )
#include "..//Graphics/Direct3D/Includes.h"
#include "cVertexFormat.h"
#elif defined( EAE6320_PLATFORM_GL )
#include "..//Graphics/OpenGL/Includes.h"
#endif
#include <Engine\Graphics\VertexFormats.h>
#include <Engine\Assets\ReferenceCountedAssets.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include "Engine/Assets/cManager.h"


namespace eae6320
{
	namespace Graphics
	{
		class cGeometry
		{
		public:
			
			// Assets
			//-------

			using Handle = Assets::cHandle<cGeometry>;
			static Assets::cManager<cGeometry> g_manager;
			static eae6320::cResult CreateGeometry(const std::string& i_path, eae6320::Assets::cHandle<eae6320::Graphics::cGeometry>& handle)
			{
				return eae6320::Graphics::cGeometry::g_manager.Load(i_path, handle);
			}
			//Vertexdata should be passed in right hand order
			void DrawGeometry();
			static eae6320::cResult Load(const std::string& i_path, eae6320::Graphics::cGeometry*& o_cGeometry);
			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()

		private:
			eae6320::cResult InitializeGeometry(uint16_t vertexCount, uint16_t indexCount, eae6320::Graphics::VertexFormats::s3dObject vertexData[], uint16_t indexData[]);
			eae6320::cResult CleanUp();
			cGeometry();
			~cGeometry() {
				CleanUp();
			}
			//Vertexdata should be passed in right hand order
			static eae6320::cResult FactoryHelper(cGeometry*& o_cGeometry, uint16_t vertexCount, uint16_t indexCount, eae6320::Graphics::VertexFormats::s3dObject vertexData[], uint16_t indexData[])
			{
				o_cGeometry = new cGeometry();
				return o_cGeometry->InitializeGeometry(vertexCount, indexCount, vertexData, indexData);
			}
			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cGeometry)
				EAE6320_ASSETS_DECLAREREFERENCECOUNT()
#if defined( EAE6320_PLATFORM_D3D )
				ID3D11Buffer* m_vertexBuffer;
			ID3D11Buffer* m_indexBuffer;
			eae6320::Graphics::cVertexFormat::Handle m_vertexFormat;
#elif defined( EAE6320_PLATFORM_GL )
				GLuint m_vertexBufferId;
			GLuint m_vertexArrayId;
			GLuint m_indexBufferId;
#endif

			uint16_t m_vertexCount;
			uint16_t m_indexCount;
		};
	}
}
