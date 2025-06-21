#include <crimson/file.d/BSP.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <mstd/misc>
#include <array>
#include <span>

#include <cassert>

namespace ct {

using Triangle = std::array<mstd::U32, 3>;

static mstd::Status import(
	const std::string& path,
	mstd::Arena& arena,
	std::span<mstd::Vector3f>& vertices,
	std::span<mstd::Vector3f>& normals,
	std::span<Triangle>& triangles
) {
	using namespace mstd;

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(
		path,
		aiProcess_CalcTangentSpace |
		aiProcess_MakeLeftHanded |
		aiProcess_OptimizeGraph |
		aiProcess_OptimizeMeshes |
		aiProcess_JoinIdenticalVertices |
		aiProcess_Triangulate
	);

	if (!scene) {
		std::cerr << errorText << "BSP: Could not load mesh: " << path << std::endl;
		return 1;
	}

	Vector3f* nStart = arena.tell<Vector3f>();
	for (Size m = 0; m < scene->mNumMeshes; ++m) {
		aiMesh* mesh = scene->mMeshes[m];
		arena.append(mesh->mNumVertices, mesh->mNormals);
	}

	Triangle* tStart = arena.tell<Triangle>();
	U32 vCount = 0;
	U32 tCount = 0;
	for (Size m = 0; m < scene->mNumMeshes; ++m) {
		aiMesh* mesh = scene->mMeshes[m];

		for (Size f = 0; f < mesh->mNumFaces; ++f) {
			aiFace face = mesh->mFaces[f];

			Triangle result = {
				face.mIndices[0] + vCount,
				face.mIndices[1] + vCount,
				face.mIndices[2] + vCount,
			};
			arena.append<Triangle>(1, &result);
		}
		tCount += mesh->mNumFaces;

		vCount += mesh->mNumVertices;
	}

	Vector3f* vStart = arena.tell<Vector3f>();
	for (Size m = 0; m < scene->mNumMeshes; ++m) {
		aiMesh* mesh = scene->mMeshes[m];
		arena.append(mesh->mNumVertices, mesh->mVertices);
	}

	normals = std::span(nStart, vCount);
	triangles = std::span(tStart, tCount);
	vertices = std::span(vStart, vCount);

	return 0;
}

static void constructPlanes(
	mstd::Arena& arena,
	const std::span<mstd::Vector3f>& vertices,
	const std::span<mstd::Vector3f>& normals,
	const std::span<Triangle>& triangles,
	std::span<Plane>& planes
) {
	using namespace mstd;

	planes = std::span(arena.reserve<Plane>(triangles.size()), triangles.size());

	for (Size i = 0; i < planes.size(); ++i) {
		Vector3f a = vertices[triangles[i][0]];

		planes[i].normal = normals[triangles[i][0]];
		planes[i].d = dot(planes[i].normal, a);
	}
}

static mstd::U32 countSplits(
	const std::span<mstd::Vector3f>& vertices,
	const std::span<Triangle>& triangles,
	Plane plane,
	mstd::U32& balance
) {
	using namespace mstd;

	I64 signedBalance = 0;
	
	U32 splits = 0;
	for (const auto& t : triangles) {
		I8 status = 0;
		for (U32 i = 0; i < 3; ++i) {
			F32 d = plane.distance(vertices[t[0]]);
			if (d > 0.0f) {
				if (status == -1) {
					status = 0;
					++splits;
					break;
				}
				status = 1;
			} else if (d < 0.0f) {
				if (status == 1) {
					status = 0;
					++splits;
					break;
				}
				status = -1;
			}
		}

		if (status == 1) {
			++signedBalance;
		} else if (status == -1) {
			--signedBalance;
		}
	}

	balance = std::abs(signedBalance);

	return splits;
}

static mstd::U32 bestTriangle(
	const std::span<mstd::Vector3f>& vertices,
	const std::span<Triangle>& triangles,
	const std::span<Plane>& planes
) {
	using namespace mstd;

	U32 leastSplits = std::numeric_limits<U32>::max();
	U32 lowestBalance = std::numeric_limits<U32>::max();
	U32 bestTriangle;
	for (Size i = 0; i < triangles.size(); ++i) {
		U32 balance;
		U32 splits = countSplits(vertices, triangles, planes[i], balance);

		if (splits < leastSplits) {
			leastSplits = splits;
			lowestBalance = balance;
			bestTriangle = i;
		} else if (splits == leastSplits && balance < lowestBalance) {
			lowestBalance = balance;
			bestTriangle = i;
		}
	}

	return bestTriangle;
}

static mstd::U8 splitTriangle(
	mstd::Arena& arena,
	const Plane& plane,
	std::span<mstd::Vector3f>& vertices,
	Triangle triangles[4]
) {
	using namespace mstd;

	Vector3f* frontVertices[4];
	Vector3f* backVertices[4];
	U8 frontCount = 0;
	U8 backCount = 0;
	
	Vector3f* prev = vertices.data() + triangles[0][2];
	Vector3f* curr;
	for (Size i = 0; i < 3; ++i) {
		curr = vertices.data() + triangles[0][i];
		F32 prevDistance = plane.distance(*prev);
		if (prevDistance > 0.0f) {
			frontVertices[frontCount++] = prev;
		} else if (prevDistance < 0.0f) {
			backVertices[backCount++] = prev;
		} else {
			frontVertices[frontCount++] = prev;
			backVertices[backCount++] = prev;
		}

		Vector3f split;
		if (plane.intersect(*prev, *curr, split)) {
			frontVertices[frontCount] = arena.append(1, &split);
			backVertices[backCount] = frontVertices[frontCount];
			vertices = std::span(vertices.begin().base(), frontVertices[frontCount]);
			++frontCount;
			++backCount;
		}

		prev = curr;
	}

	U8 validTriangles = 0;

	if (frontCount == 3) {
		triangles[0] = {
			U32((Size)frontVertices[0] - (Size)vertices.data()),
			U32((Size)frontVertices[1] - (Size)vertices.data()),
			U32((Size)frontVertices[2] - (Size)vertices.data()),
		};
		validTriangles |= 0b1;
	} else if (frontCount == 4) {
		triangles[0] = {
			U32((Size)frontVertices[0] - (Size)vertices.data()),
			U32((Size)frontVertices[1] - (Size)vertices.data()),
			U32((Size)frontVertices[2] - (Size)vertices.data()),
		};
		triangles[1] = {
			U32((Size)frontVertices[0] - (Size)vertices.data()),
			U32((Size)frontVertices[2] - (Size)vertices.data()),
			U32((Size)frontVertices[3] - (Size)vertices.data()),
		};
		validTriangles |= 0b11;
	}

	if (backCount == 3) {
		triangles[2] = {
			U32((Size)backVertices[0] - (Size)vertices.data()),
			U32((Size)backVertices[1] - (Size)vertices.data()),
			U32((Size)backVertices[2] - (Size)vertices.data()),
		};
		validTriangles |= 0b100;
	} else if (frontCount == 4) {
		triangles[2] = {
			U32((Size)backVertices[0] - (Size)vertices.data()),
			U32((Size)backVertices[1] - (Size)vertices.data()),
			U32((Size)backVertices[2] - (Size)vertices.data()),
		};
		triangles[3] = {
			U32((Size)backVertices[0] - (Size)vertices.data()),
			U32((Size)backVertices[2] - (Size)vertices.data()),
			U32((Size)backVertices[3] - (Size)vertices.data()),
		};
		validTriangles |= 0b1100;
	}

	return validTriangles;
}

struct BSPNode {
	Plane p;
};

static mstd::Tree<BSPNode, 2> partition(
	mstd::Arena& bspArena,
	mstd::Arena& geometryArena,
	std::span<mstd::Vector3f>& vertices,
	std::span<mstd::Vector3f>& normals,
	std::span<Triangle>& triangles
) {
	using namespace mstd;

	std::span<Plane> planes;
	constructPlanes(geometryArena, vertices, normals, triangles, planes);

	for (Size i = 0; i < triangles.size(); ++i) {
		Triangle result[4];
		result[0] = triangles[i];
		U8 validTriangles = splitTriangle(geometryArena, planes[0], vertices, result);

		
	}

	return {};
}

void BSP::build(const std::string& path) {
	using namespace mstd;

	Arena geometryArena(1L << 24);
	Arena bspArena(1L << 24);

	std::span<Vector3f> vertices;
	std::span<Vector3f> normals;
	std::span<Triangle> triangles;
	if (import(path, geometryArena, vertices, normals, triangles)) return;

	std::vector<VertexArray::Attribute> attributes = {
		{
			.stride = sizeof(Vector3f),
			.size = 3,
			.type = GL_FLOAT,
			.normalized = false,
		},
		{
			.stride = sizeof(Vector3f),
			.size = 3,
			.type = GL_FLOAT,
			.normalized = false,
		},
	};
	
	vertexArray.allocateAttributes(attributes, vertices.size());
	vertexArray.writeAttributes(0, vertices.data(), vertices.size());
	vertexArray.writeAttributes(1, normals.data(), normals.size());

	vertexArray.allocateElements(triangles.size_bytes());
	vertexArray.writeElements((U32*)triangles.data(), triangles.size() * 3);

	partition(bspArena, geometryArena, vertices, normals, triangles);
}

}
