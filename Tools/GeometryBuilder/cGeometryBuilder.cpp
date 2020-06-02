// GeometryBuilder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "cGeometryBuilder.h"
#include "Engine/Platform/Platform.h"
#include "Engine/Results/Results.h"
#include <Engine/ScopeGuard/cScopeGuard.h>
#include "Tools/AssetBuildLibrary/Functions.h"
#include <iostream>
#include "./Engine/Graphics/VertexFormats.h"
#include <Engine/Asserts/Asserts.h>
#include <fstream> 

eae6320::Graphics::VertexFormats::s3dObject* lua_vertexData = nullptr;
uint16_t* lua_indexData = nullptr;
uint16_t lua_vertexCount = 0;
uint16_t lua_indexCount = 0;


eae6320::cResult eae6320::Assets::cGeometryBuilder::Build(const std::vector<std::string>& i_arguments)
{
	auto result = eae6320::Results::Success;

	std::string errorMessage;

	// Create a new Lua state
	lua_State* luaState = nullptr;
	eae6320::cScopeGuard scopeGuard_onExit([&luaState]
	{
		if (luaState)
		{
			// If I haven't made any mistakes
			// there shouldn't be anything on the stack
			// regardless of any errors
			EAE6320_ASSERT(lua_gettop(luaState) == 0);

			lua_close(luaState);
			luaState = nullptr;
		}
	});
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			result = eae6320::Results::OutOfMemory;
			std::cerr << "Failed to create a new Lua state" << std::endl;
			return result;
		}
	}

	// Load the asset file as a "chunk",
	// meaning there will be a callable function at the top of the stack
	const auto stackTopBeforeLoad = lua_gettop(luaState);
	{
		const auto luaResult = luaL_loadfile(luaState, m_path_source);
		if (luaResult != LUA_OK)
		{
			result = eae6320::Results::Failure;
			std::cerr << lua_tostring(luaState, -1) << std::endl;
			// Pop the error message
			lua_pop(luaState, 1);
			return result;
		}
	}
	// Execute the "chunk", which should load the asset
	// into a table at the top of the stack
	{
		constexpr int argumentCount = 0;
		constexpr int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
		constexpr int noMessageHandler = 0;
		const auto luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
		if (luaResult == LUA_OK)
		{
			// A well-behaved asset file will only return a single value
			const auto returnedValueCount = lua_gettop(luaState) - stackTopBeforeLoad;
			if (returnedValueCount == 1)
			{
				// A correct asset file _must_ return a table
				if (!lua_istable(luaState, -1))
				{
					result = eae6320::Results::InvalidFile;
					std::cerr << "Asset files must return a table (instead of a " <<
						luaL_typename(luaState, -1) << ")" << std::endl;
					// Pop the returned non-table value
					lua_pop(luaState, 1);
					return result;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "Asset files must return a single table (instead of " <<
					returnedValueCount << " values)" << std::endl;
				// Pop every value that was returned
				lua_pop(luaState, returnedValueCount);
				return result;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			std::cerr << lua_tostring(luaState, -1) << std::endl;
			// Pop the error message
			lua_pop(luaState, 1);
			return result;
		}
	}

	// If this code is reached the asset file was loaded successfully,
	// and its table is now at index -1
	eae6320::cScopeGuard scopeGuard_popAssetTable([luaState]
	{
		lua_pop(luaState, 1);
	});
	result = eae6320::GeometryBuilderAssistant::LoadTableValues(*luaState);

	//Change winding order before writing binary file if Direct3D
#if defined( EAE6320_PLATFORM_D3D )
	for (uint16_t i = 1; i < lua_indexCount; i += 3)
	{
		uint16_t temp = lua_indexData[i];
		lua_indexData[i] = lua_indexData[i + 1];
		lua_indexData[i + 1] = temp;
	}
#endif

	m_path_target[strlen(m_path_target) - 3] = 'b';
	m_path_target[strlen(m_path_target) - 2] = 'i';
	m_path_target[strlen(m_path_target) - 1] = 'n';

	std::ofstream outfile(m_path_target, std::ofstream::binary);

	outfile.write((char*)& lua_vertexCount, sizeof(uint16_t));
	outfile.write((char*)& lua_indexCount, sizeof(uint16_t));
	outfile.write((char*) lua_vertexData, (sizeof(float) * 3)* lua_vertexCount);
	outfile.write((char*) lua_indexData, sizeof(uint16_t) * lua_indexCount);

	outfile.close();

	lua_vertexData = nullptr;
	lua_indexData = nullptr;
	lua_vertexCount = 0;
	lua_indexCount = 0;

	return result;
}

// Static Data Initialization
//===========================


eae6320::cResult eae6320::GeometryBuilderAssistant::LoadTableValues(lua_State& io_luaState)
{
	auto result = eae6320::Results::Success;

	if (!(result = LoadTableValues_VertexData(io_luaState)))
	{
		return result;
	}
	if (!(result = LoadTableValues_IndexData(io_luaState)))
	{
		return result;
	}

	return result;
}

eae6320::cResult eae6320::GeometryBuilderAssistant::LoadTableValues_VertexData(lua_State& io_luaState)
{
	auto result = eae6320::Results::Success;

	// Right now the asset table is at -1.
	// After the following table operation it will be at -2
	// and the "VertexData" table will be at -1:
	constexpr auto* const key = "VertexData";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	// It can be hard to remember where the stack is at
	// and how many values to pop.
	// There are two ways that I suggest making it easier to keep track of this:
	//	1) Use scope guards to pop things automatically
	//	2) Call a different function when you are at a new level
	// Right now we know that we have an original table at -2,
	// and a new one at -1,
	// and so we _know_ that we always have to pop at least _one_
	// value before leaving this function
	// (to make the original table be back to index -1).
	// We can create a scope guard immediately as soon as the new table has been pushed
	// to guarantee that it will be popped when we are done with it:
	eae6320::cScopeGuard scopeGuard_popVertexData([&io_luaState]
	{
		lua_pop(&io_luaState, 1);
	});
	// Additionally, I try not to do any further stack manipulation in this function
	// and call other functions that assume the "textures" table is at -1
	// but don't know or care about the rest of the stack
	if (lua_istable(&io_luaState, -1))
	{
		if (!(result = LoadTableValues_VertexData_Values(io_luaState)))
		{
			return result;
		}
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		return result;
	}

	return result;
}

eae6320::cResult eae6320::GeometryBuilderAssistant::LoadTableValues_VertexData_Values(lua_State& io_luaState)
{
	// Right now the asset table is at -2
		// and the textures table is at -1.
		// NOTE, however, that it doesn't matter to me in this function
		// that the asset table is at -2.
		// Because I've carefully called a new function for every "stack level"
		// The only thing I care about is that the textures table that I care about
		// is at the top of the stack.
		// As long as I make sure that when I leave this function it is _still_
		// at -1 then it doesn't matter to me at all what is on the stack below it.

	auto result = eae6320::Results::Success;

	std::cout << "Iterating through every texture path:" << std::endl;

	if ((int)luaL_len(&io_luaState, -1) > 65535)
	{
		result = eae6320::Results::Failure;
		std::cerr << "Failed to load! Vertex count exceeded maximum" << std::endl;
		return result;
	}
	else
	{
		lua_vertexCount = (int)luaL_len(&io_luaState, -1);
	}

	lua_vertexData = new eae6320::Graphics::VertexFormats::s3dObject[lua_vertexCount];

	for (uint16_t i = 1; i <= lua_vertexCount; ++i)
	{
		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popTexturePath([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});

		lua_pushinteger(&io_luaState, 1);
		lua_gettable(&io_luaState, -2);
		lua_vertexData[i - 1].x = (float)lua_tonumber(&io_luaState, -1);
		lua_pop(&io_luaState, 1);

		lua_pushinteger(&io_luaState, 2);
		lua_gettable(&io_luaState, -2);
		lua_vertexData[i - 1].y = (float)lua_tonumber(&io_luaState, -1);
		lua_pop(&io_luaState, 1);

		lua_pushinteger(&io_luaState, 3);
		lua_gettable(&io_luaState, -2);
		lua_vertexData[i - 1].z = (float)lua_tonumber(&io_luaState, -1);
		lua_pop(&io_luaState, 1);
	}

	return result;
}

eae6320::cResult eae6320::GeometryBuilderAssistant::LoadTableValues_IndexData(lua_State& io_luaState)
{
	auto result = eae6320::Results::Success;

	// Right now the asset table is at -1.
	// After the following table operation it will be at -2
	// and the "IndexData" table will be at -1:
	constexpr auto* const key = "IndexData";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	eae6320::cScopeGuard scopeGuard_popParameters([&io_luaState]
	{
		lua_pop(&io_luaState, 1);
	});
	if (lua_istable(&io_luaState, -1))
	{
		if (!(result = LoadTableValues_IndexData_Values(io_luaState)))
		{
			return result;
		}
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		return result;
	}

	return result;
}

eae6320::cResult eae6320::GeometryBuilderAssistant::LoadTableValues_IndexData_Values(lua_State& io_luaState)
{
	// Right now the IndexData table is at -1.
		// Every time the while() statement is executed it will be at -2
		// and the next key will be at -1.
		// Inside the block the table will be at -3,
		// the current key will be at -2,
		// and the value will be at -1.
		// (You may want to review LoadTableValues_allKeys()
		// in the ReadTopLevelTableValues example,
		// but remember that you don't need to know how to do this technique)

	auto result = eae6320::Results::Success;

	std::cout << "Iterating through the parameters:" << std::endl;

	if ((int)luaL_len(&io_luaState, -1) > 65535)
	{
		result = eae6320::Results::Failure;
		std::cerr << "Failed to load! Index count exceeded maximum" << std::endl;
		return result;
	}
	else
	{
		lua_indexCount = (int)luaL_len(&io_luaState, -1);
	}

	lua_indexData = new uint16_t[lua_indexCount];
	lua_pushnil(&io_luaState);
	int i = 0;
	while (lua_next(&io_luaState, -2))
	{
		lua_indexData[i] = (uint16_t)lua_tointeger(&io_luaState, -1);
		lua_pop(&io_luaState, 1);
		i++;
	}

	return result;
}
