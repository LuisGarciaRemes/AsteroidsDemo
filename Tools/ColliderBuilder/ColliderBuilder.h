#pragma once
/*
	This class builds effects
*/

#ifndef EAE6320_COLLIDERBUILDER_H
#define EAE6320_COLLIDERBUILDER_H

// Includes
//=========

#include "../..//Tools/AssetBuildLibrary/cbBuilder.h"
#include "../../External/Lua/Includes.h"

// Class Declaration
//==================

namespace eae6320
{

	namespace ColliderBuilderAssistant
	{
		eae6320::cResult LoadTableValues(lua_State& io_luaState);

		eae6320::cResult LoadTableValues_Radius(lua_State& io_luaState);
		eae6320::cResult LoadTableValues_Width(lua_State& io_luaState);
		eae6320::cResult LoadTableValues_Height(lua_State& io_luaState);
		eae6320::cResult LoadTableValues_Layer(lua_State& io_luaState);
	}

	namespace Assets
	{
		class ColliderBuilder : public cbBuilder
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

#endif	// EAE6320_COLLIDERBUILDER_H
