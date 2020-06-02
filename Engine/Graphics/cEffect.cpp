#include "cEffect.h"
#include <Engine\Platform\Platform.h>

	eae6320::cResult eae6320::Graphics::cEffect::Load(const std::string& i_path, eae6320::Graphics::cEffect*& o_cEffect)
	{
		eae6320::Platform::sDataFromFile dataFromFile;
		eae6320::Platform::LoadBinaryFile(i_path.c_str(), dataFromFile);

		char* vertexShader = nullptr;
		char* fragmentShader = nullptr;
		uint8_t renderBits = 0;

		if (dataFromFile.data)
		{
			auto currentOffset = reinterpret_cast<uintptr_t>(dataFromFile.data);
			vertexShader = reinterpret_cast<char*>(currentOffset);

			currentOffset += strlen(vertexShader) + 1;

			fragmentShader = reinterpret_cast<char*>(currentOffset);

			currentOffset += strlen(fragmentShader) + 1;

			renderBits = *reinterpret_cast<uint8_t*>(currentOffset);
		}

		if (vertexShader == nullptr || fragmentShader == nullptr)
		{
			return eae6320::Results::Failure;
		}

		std::string vertexString(vertexShader);
		std::string fragmentString(fragmentShader);

		return eae6320::Graphics::cEffect::FactoryHelper(o_cEffect,vertexString,fragmentString, renderBits);
	}
