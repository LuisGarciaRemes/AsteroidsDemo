#pragma once
/*
	This class builds effects
*/

#ifndef EAE6320_CEFFECTBUILDER_H
#define EAE6320_CEFFECTBUILDER_H

// Includes
//=========

#include "../..//Tools/AssetBuildLibrary/cbBuilder.h"
#include "../../External/Lua/Includes.h"

// Class Declaration
//==================

namespace eae6320
{

	namespace EffectBuilderAssistant
	{
		eae6320::cResult LoadTableValues(lua_State& io_luaState);

		eae6320::cResult LoadTableValues_VertexShader(lua_State& io_luaState);
		eae6320::cResult LoadTableValues_FragmentShader(lua_State& io_luaState);
		eae6320::cResult LoadTableValues_DepthWriting(lua_State& io_luaState);
		eae6320::cResult LoadTableValues_DepthTesting(lua_State& io_luaState);
		void CheckRenderBits();
	}

	namespace Assets
	{
		class cEffectBuilder : public cbBuilder
		{
			// Inherited Implementation
			//=========================

		private:

			// Build
			//------

			virtual cResult Build(const std::vector<std::string>& i_arguments) override;
		};
	}
}

#endif	// EAE6320_CEFFECTBUILDER_H
