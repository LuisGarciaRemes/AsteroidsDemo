// ColliderBuilder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "ColliderBuilder.h"
#include "Engine/Platform/Platform.h"
#include "Engine/Results/Results.h"
#include <Engine/ScopeGuard/cScopeGuard.h>
#include "Tools/AssetBuildLibrary/Functions.h"
#include <iostream>
#include <Engine/Asserts/Asserts.h>
#include <fstream> 

bool lua_circle = true;
float lua_width = 0.0f;
float lua_height = 0.0f;
float lua_radius = 0.0f;
uint16_t lua_layer = 0;



eae6320::cResult eae6320::Assets::ColliderBuilder::Build(const std::vector<std::string>& i_arguments)
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

	result = eae6320::ColliderBuilderAssistant::LoadTableValues(*luaState);

	m_path_target[strlen(m_path_target) - 3] = 'b';
	m_path_target[strlen(m_path_target) - 2] = 'i';
	m_path_target[strlen(m_path_target) - 1] = 'n';

	std::ofstream outfile(m_path_target, std::ofstream::binary);

	outfile.write((char*)& lua_circle, sizeof(bool));

	if (lua_circle)
	{
		outfile.write((char*) &lua_radius, sizeof(float));
	}
	else
	{
		outfile.write((char*)& lua_width, sizeof(float));
		outfile.write((char*)& lua_height, sizeof(float));
	}
	outfile.write((char*)& lua_layer, sizeof(uint16_t));
	outfile.close();

	lua_circle = true;
	lua_width = 0.0f;
	lua_height = 0.0f;
	lua_radius = 0.0f;
	lua_layer = 0;

	return result;
}

eae6320::cResult eae6320::ColliderBuilderAssistant::LoadTableValues(lua_State& io_luaState)
{
	auto result = eae6320::Results::Success;

	if (!(result = LoadTableValues_Radius(io_luaState)))
	{
		lua_circle = false;
		if (!(result = LoadTableValues_Width(io_luaState)))
		{
			return result;
		}
		if (!(result = LoadTableValues_Height(io_luaState)))
		{
			return result;
		}
	}
	if (!(result = LoadTableValues_Layer(io_luaState)))
	{
		return result;
	}

	return result;
}

eae6320::cResult eae6320::ColliderBuilderAssistant::LoadTableValues_Width(lua_State& io_luaState)
{
	auto result = eae6320::Results::Success;

	// Right now the asset table is at -1.
	// After the following table operation it will be at -2
	// and the "VertexData" table will be at -1:
	constexpr auto* const key = "Width";
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
	std::string error_message;

	if (lua_isnumber(&io_luaState, -1))
	{
		lua_width = (float)lua_tonumber(&io_luaState, -1);
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be a float "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		return result;
	}

	return result;
}

eae6320::cResult eae6320::ColliderBuilderAssistant::LoadTableValues_Height(lua_State& io_luaState)
{
	auto result = eae6320::Results::Success;

	// Right now the asset table is at -1.
	// After the following table operation it will be at -2
	// and the "VertexData" table will be at -1:
	constexpr auto* const key = "Height";
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
	std::string error_message;

	if (lua_isnumber(&io_luaState, -1))
	{
		lua_height = (float)lua_tonumber(&io_luaState, -1);
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be a float "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		return result;
	}

	return result;
}

eae6320::cResult eae6320::ColliderBuilderAssistant::LoadTableValues_Radius(lua_State& io_luaState)
{
	auto result = eae6320::Results::Success;

	// Right now the asset table is at -1.
	// After the following table operation it will be at -2
	// and the "VertexData" table will be at -1:
	constexpr auto* const key = "Radius";
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
	std::string error_message;

	if (lua_isnumber(&io_luaState, -1))
	{
		lua_radius = (float)lua_tonumber(&io_luaState, -1);
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be a float "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		return result;
	}

	return result;
}


eae6320::cResult eae6320::ColliderBuilderAssistant::LoadTableValues_Layer(lua_State& io_luaState)
{
	auto result = eae6320::Results::Success;

	// Right now the asset table is at -1.
	// After the following table operation it will be at -2
	// and the "VertexData" table will be at -1:
	constexpr auto* const key = "Layer";
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
	std::string error_message;

	if (lua_isinteger(&io_luaState, -1))
	{
		lua_layer = (uint16_t)lua_tointeger (&io_luaState, -1);
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be an integer "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		return result;
	}

	return result;
}
