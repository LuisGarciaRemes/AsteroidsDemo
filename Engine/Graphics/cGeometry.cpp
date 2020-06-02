#include "cGeometry.h"
#include <Engine\Platform\Platform.h>

eae6320::Assets::cManager<eae6320::Graphics::cGeometry> eae6320::Graphics::cGeometry::g_manager;

eae6320::cResult eae6320::Graphics::cGeometry::Load(const std::string& i_path, eae6320::Graphics::cGeometry*& o_cGeometry)
{
	eae6320::Platform::sDataFromFile dataFromFile;
	eae6320::Platform::LoadBinaryFile(i_path.c_str(),dataFromFile);

	uint16_t vertexCount = 0;
	uint16_t indexCount = 0;
	eae6320::Graphics::VertexFormats::s3dObject* vertexData = nullptr;
	uint16_t* indexData = nullptr;

	if (dataFromFile.data)
	{
		auto currentOffset = reinterpret_cast<uintptr_t>(dataFromFile.data);
		vertexCount = *reinterpret_cast<uint16_t*>(currentOffset);

		currentOffset += sizeof(vertexCount);

		indexCount = *reinterpret_cast<uint16_t*>(currentOffset);

		currentOffset += sizeof(indexCount);

		vertexData = new eae6320::Graphics::VertexFormats::s3dObject[vertexCount];

		memcpy(vertexData, reinterpret_cast<void*>(currentOffset), sizeof(float) * 3 * vertexCount);

		currentOffset += sizeof(float) * 3* vertexCount;

		indexData = new uint16_t[indexCount];

		memcpy(indexData, reinterpret_cast<void*>(currentOffset), sizeof(uint16_t) * indexCount);
	}

	return eae6320::Graphics::cGeometry::FactoryHelper(o_cGeometry,vertexCount,indexCount,vertexData,indexData);
}
