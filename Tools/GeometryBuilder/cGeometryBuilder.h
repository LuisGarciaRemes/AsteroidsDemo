#pragma once
/*
	This class builds geometry
*/

#ifndef EAE6320_CGEOMETRYBUILDER_H
#define EAE6320_CCGEOMETRYBUILDER_H

// Includes
//=========

#include "../..//Tools/AssetBuildLibrary/cbBuilder.h"
#include "../../External/Lua/Includes.h"

// Class Declaration
//==================

namespace eae6320
{

	namespace GeometryBuilderAssistant
	{
		eae6320::cResult LoadTableValues(lua_State& io_luaState);

		eae6320::cResult LoadTableValues_VertexData(lua_State& io_luaState);
		eae6320::cResult LoadTableValues_VertexData_Values(lua_State& io_luaState);

		eae6320::cResult LoadTableValues_IndexData(lua_State& io_luaState);
		eae6320::cResult LoadTableValues_IndexData_Values(lua_State& io_luaState);
	}

	namespace Assets
	{
		class cGeometryBuilder : public cbBuilder
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

#endif	// EAE6320_CGEOMETRYBUILDER_H
