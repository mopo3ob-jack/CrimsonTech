#include <crimson/renderer.d/VertexArray.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <mstd/misc>
#include <mstd/geometry>
#include <fstream>

namespace ct {

VertexArray::VertexArray(
	std::span<const Attribute> attributes,
	mstd::Size vertexCount,
	mstd::Size elementCount,
	mstd::Size elementSize,
	const void* data
) {
	using namespace mstd;

	glCreateVertexArrays(1, &vao);
	glCreateBuffers(1, &vbo);
	glCreateBuffers(1, &ebo);

	attributeOffsets.resize(attributes.size());
	
	Size vertexSize = 0;
	for (const auto& i : attributes) {
		vertexSize += i.stride;
	}

	glNamedBufferStorage(vbo, vertexCount * vertexSize, data, GL_DYNAMIC_STORAGE_BIT);

	GLintptr offset = 0;
	for (U32 i = 0; i < attributes.size(); ++i) {
		glVertexArrayVertexBuffer(vao, i, vbo, offset, attributes[i].stride);
		glEnableVertexArrayAttrib(vao, i);
		if (attributes[i].integral) {
			glVertexArrayAttribIFormat(
				vao,
				i,
				attributes[i].size,
				attributes[i].type,
				0
			);
		} else {
			glVertexArrayAttribFormat(
				vao,
				i,
				attributes[i].size,
				attributes[i].type,
				attributes[i].normalized,
				0L
			);
		}
		glVertexArrayAttribBinding(vao, i, i);

		attributeOffsets[i] = offset;
		offset += attributes[i].stride * vertexCount;
	}

	glNamedBufferStorage(ebo, elementCount * elementSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vao, ebo);
}

template <>
void VertexArray::draw<mstd::U8>(GLsizei count, mstd::Size offset) const {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_BYTE, (void*)(offset * sizeof(mstd::U8)));
}

template <>
void VertexArray::draw<mstd::U16>(GLsizei count, mstd::Size offset) const {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, (void*)(offset * sizeof(mstd::U16)));
}

template <>
void VertexArray::draw<mstd::U32>(GLsizei count, mstd::Size offset) const {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)(offset * sizeof(mstd::U32)));
}

mstd::Status VertexArray::load(
	const mstd::C8* inputPath,
	mstd::U32& vertexCount,
	mstd::U32 elementCount,
	mstd::Arena& arena
) {
	using namespace mstd;

	std::fstream file(inputPath, std::ios::in | std::ios::binary);

	if (!file) {
		std::cerr << errorText << "VertexArray: Could not load model: " << inputPath << std::endl;
		return 1;
	}

	struct {
		U8 magic[4] = "VAC";
		U32 attributeCount = 0;
		U32 vertexChunkSize = 0;
		U32 elementChunkSize = 0;
	} header;

	file.read((C8*)&header, sizeof(header));
	std::cout << header.vertexChunkSize << std::endl;

	Attribute* attributes = arena.reserve<Attribute>(header.attributeCount);
	void* vertices = arena.reserve<C8>(header.vertexChunkSize);
	void* elements = arena.reserve<C8>(header.elementChunkSize);

	file.read((C8*)attributes, header.attributeCount * sizeof(Attribute));
	file.read((C8*)vertices, header.vertexChunkSize);
	file.read((C8*)elements, header.elementChunkSize);

	elementCount = header.elementChunkSize / sizeof(U32);

	Size vertexSize = 0;
	for (U32 i = 0; i < header.attributeCount; ++i) {
		vertexSize += attributes[i].stride;
	}

	vertexCount = header.vertexChunkSize / vertexSize;
	std::cout << header.vertexChunkSize << std::endl;
	std::cout << vertexCount << std::endl;
	std::cout << vertexSize << std::endl;

	this->~VertexArray();

	*this = std::move(VertexArray(std::span(attributes, header.attributeCount), vertexCount, header.elementChunkSize, 1, vertices));

	writeElements<U8>((U8*)elements, header.elementChunkSize, 0);

	Vector3f* positions = (Vector3f*)vertices;
	for (Size i = 0; i < vertexCount; ++i) {
		std::cout << std::string(positions[i]) << std::endl;
	}

	return 0;
}

static constexpr mstd::Size OPTION_COUNT = sizeof(VertexArray::ConvertOptions) / sizeof(mstd::I32);

void sortOptions(const VertexArray::ConvertOptions& options, mstd::U32 result[OPTION_COUNT]);
void loadPositions(const aiScene* scene, mstd::Arena& arena);
void loadMaterials(const aiScene* scene, mstd::Arena& arena);
void loadUVs(const aiScene* scene, mstd::Arena& arena, const char* inputPath);
void loadNormals(const aiScene* scene, mstd::Arena& arena);
void loadTangents(const aiScene* scene, mstd::Arena& arena);
void loadBitangents(const aiScene* scene, mstd::Arena& arena);

mstd::Status VertexArray::convert(
	const mstd::C8 *inputPath,
	const mstd::C8 *outputPath,
	ConvertOptions options,
	mstd::Arena& arena
) {
	using namespace mstd;

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(inputPath, 
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_Triangulate
	);

	if (!scene) {
		std::cerr << errorText << "VertexArray: Failed to load file: " << inputPath << std::endl;
		return 1;
	}

	std::fstream output(outputPath, std::ios::binary | std::ios::out | std::ios::trunc);
	
	if (!output) {
		std::cerr << errorText << "VertexArray: Failed to open file: " << outputPath << std::endl;
		return 1;
	}

	if (options.materials) {
		for (Size m = 0; m < scene->mNumMaterials; ++m) {
			aiMaterial* mat = scene->mMaterials[m];
		}
	}

	U32 vertexCount = 0;
	for (Size m = 0; m < scene->mNumMeshes; ++m) {
		vertexCount += scene->mMeshes[m]->mNumVertices;
	}

	struct {
		U8 magic[4] = "VAC";
		U32 attributeCount = 0;
		U32 vertexChunkSize = 0;
		U32 elementChunkSize = 0;
	} header;

	U32 vertexSize = sizeof(Vector3f);

	mstd::U32 optionArray[OPTION_COUNT];
	constexpr mstd::Size optionSize[] = {
		sizeof(Vector3f), sizeof(U32), sizeof(Vector2f), sizeof(Vector3f), sizeof(Vector3f), sizeof(Vector3f)
	};
	static_assert(sizeof(optionSize) / sizeof(optionSize[0]) == OPTION_COUNT);

	sortOptions(options, optionArray);

	enum {
		POSITIONS = 0,
		MATERIALS = 1,
		UVS = 2,
		NORMALS = 3,
		TANGENTS = 4,
		BITANGENTS = 5,
	};

	Attribute* attributes = arena.reserve<Attribute>(OPTION_COUNT);

	C8* vertexStart = arena.tell<C8>();
	for (Size i = 0; i < OPTION_COUNT; ++i) {
		if (optionArray[i] == -1) {
			break;
		}

		switch(optionArray[i]) {
		case POSITIONS:
			loadPositions(scene, arena);
			attributes[i] = {
				.stride = sizeof(Vector3f),
				.size = 3,
				.type = GL_FLOAT,
				.normalized = false
			};
			break;
		case MATERIALS:
			loadMaterials(scene, arena);
			attributes[i] = {
				sizeof(U32),
				1,
				GL_UNSIGNED_INT,
				GL_FALSE,
				true
			};
			break;
		case UVS:
			loadUVs(scene, arena, inputPath);
			attributes[i] = {
				.stride = sizeof(Vector2f),
				.size = 2,
				.type = GL_FLOAT,
				.normalized = false
			};
			break;
		case NORMALS:
			loadNormals(scene, arena);
			attributes[i] = {
				.stride = sizeof(Vector3f),
				.size = 3,
				.type = GL_FLOAT,
				.normalized = false
			};
			break;
		case TANGENTS:
			loadTangents(scene, arena);
			attributes[i] = {
				.stride = sizeof(Vector3f),
				.size = 3,
				.type = GL_FLOAT,
				.normalized = false
			};
			break;
		case BITANGENTS:
			loadBitangents(scene, arena);
			attributes[i] = {
				.stride = sizeof(Vector3f),
				.size = 3,
				.type = GL_FLOAT,
				.normalized = false
			};
			break;
		}

		++header.attributeCount;
	}

	C8* elementStart = arena.tell<C8>();
	for (Size m = 0; m < scene->mNumMeshes; ++m) {
		aiMesh* mesh = scene->mMeshes[m];

		for (Size f = 0; f < mesh->mNumFaces; ++f) {
			aiFace face = mesh->mFaces[f];
			arena.append(face.mIndices, 3);
		}
	}

	header.vertexChunkSize = elementStart - vertexStart;
	header.elementChunkSize = arena.tell<C8>() - elementStart;
	
	// These could all be optimized into a single write but I really don't care.
	output.write((C8*)&header, sizeof(header));
	output.write((C8*)attributes, header.attributeCount * sizeof(Attribute));
	output.write((C8*)vertexStart, header.vertexChunkSize);
	output.write((C8*)elementStart, header.elementChunkSize);

	Vector3f* positions = (Vector3f*)vertexStart;
	for (Size i = 0; i < vertexCount; ++i) {
		std::cout << std::string(positions[i]) << std::endl;
	}

	arena.truncate(attributes);

	return 0;
}

void sortOptions(const VertexArray::ConvertOptions& options, mstd::U32 result[OPTION_COUNT]) {
	using namespace mstd;

	const I32* optionsArray = (I32*)&options;

	I32 minFilter = -1;
	for (Size i = 0; i < OPTION_COUNT; ++i) {
		I32 lowest = 100;
		U32 lowestIndex = -1;
		for (Size j = 0; j < OPTION_COUNT; ++j) {
			I32 option = optionsArray[j];
			if (option > minFilter && option < lowest) {
				lowest = option;
				lowestIndex = j;
			}
		}

		result[i] = lowestIndex;

		if (lowestIndex == -1) {
			return;
		}

		minFilter = lowest;
	}
}

void loadPositions(const aiScene* scene, mstd::Arena& arena) {
	for (mstd::Size m = 0; m < scene->mNumMeshes; ++m) {
		aiMesh* mesh = scene->mMeshes[m];
		arena.append(mesh->mVertices, mesh->mNumVertices);
	}
}

void loadMaterials(const aiScene* scene, mstd::Arena& arena) {
	for (mstd::Size m = 0; m < scene->mNumMeshes; ++m) {
		aiMesh* mesh = scene->mMeshes[m];
		mstd::U32 materialIndex = mesh->mMaterialIndex;
		mstd::U32* materials = arena.reserve<mstd::U32>(mesh->mNumVertices);
		std::fill(materials, materials + mesh->mNumVertices, materialIndex);
	}
}

void loadUVs(const aiScene* scene, mstd::Arena& arena, const char* inputPath) {
	static_assert(sizeof(aiVector3D::x) == sizeof(mstd::F32));
	for (mstd::Size m = 0; m < scene->mNumMeshes; ++m) {
		aiMesh* mesh = scene->mMeshes[m];

		if (mesh->mNumUVComponents[0] != 2) {
			std::cout << warningText
				<< "VertexArray: Expected 2 UV components for mesh: " << mesh->mName.C_Str()
				<< " in file: " << inputPath
				<< std::endl;
			continue;
		}

		mstd::Vector2f* uvs = arena.reserve<mstd::Vector2f>(mesh->mNumVertices);
		for (mstd::Size i = 0; i < mesh->mNumVertices; ++i) {
			aiVector3D uv = mesh->mTextureCoords[0][i];
			std::memcpy(uvs + i, &uv, sizeof(mstd::Vector2f));
		}
	}
}

void loadNormals(const aiScene* scene, mstd::Arena& arena) {
	for (mstd::Size m = 0; m < scene->mNumMeshes; ++m) {
		aiMesh* mesh = scene->mMeshes[m];
		arena.append(mesh->mNormals, mesh->mNumVertices);
	}
}

void loadTangents(const aiScene* scene, mstd::Arena& arena) {
	for (mstd::Size m = 0; m < scene->mNumMeshes; ++m) {
		aiMesh* mesh = scene->mMeshes[m];
		arena.append(mesh->mTangents, mesh->mNumVertices);
	}
}

void loadBitangents(const aiScene* scene, mstd::Arena& arena) {
	for (mstd::Size m = 0; m < scene->mNumMeshes; ++m) {
		aiMesh* mesh = scene->mMeshes[m];
		arena.append(mesh->mBitangents, mesh->mNumVertices);
	}
}

}
