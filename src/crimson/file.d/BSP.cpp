#include <crimson/file.d/BSP.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <mstd/misc>
#include <array>
#include <span>
#include <optional>
#include <bitset>

#include <cassert>

#include <imgui/imgui.h>

namespace ct {

struct IndexedFace : Plane {
	mstd::Size vertices[3];
};

struct Face : Plane {
	mstd::Vector3f vertices[3];
};

static mstd::Status import(
	const std::string& path,
	mstd::Arena& arena,
	std::span<std::span<mstd::U32>>& meshes,
	std::span<mstd::Vector3f>& vertices,
	std::span<mstd::Vector3f>& normals,
	std::span<mstd::U32>& elements
) {
	using namespace mstd;

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(
		path,
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_Triangulate
	);

	if (!scene) {
		std::cerr << errorText << "BSP: Could not load mesh: " << path << std::endl;
		return 1;
	}

	if (scene->mNumMeshes == 0) {
		return 1;
	}

	meshes = std::span(
		arena.reserve<std::span<U32>>(scene->mNumMeshes),
		scene->mNumMeshes
	);

	vertices = std::span(arena.tell<Vector3f>(), 0);
	for (Size m = 0; m < scene->mNumMeshes; ++m) {
		aiMesh* mesh = scene->mMeshes[m];
		arena.append((Vector3f*)mesh->mVertices, mesh->mNumVertices);
	}
	vertices = std::span(vertices.data(), arena.tell<Vector3f>());

	normals = std::span(arena.tell<Vector3f>(), 0);
	for (Size m = 0; m < scene->mNumMeshes; ++m) {
		aiMesh* mesh = scene->mMeshes[m];
		arena.append((Vector3f*)mesh->mNormals, mesh->mNumVertices);
	}
	normals = std::span(normals.data(), arena.tell<Vector3f>());

	elements = std::span(arena.tell<U32>(), 0);
	Size vertexCount = 0;
	for (Size m = 0; m < scene->mNumMeshes; ++m) {
		aiMesh* mesh = scene->mMeshes[m];

		meshes[m] = std::span(arena.tell<U32>(), 0);
		for (Size f = 0; f < mesh->mNumFaces; ++f) {
			aiFace& face = mesh->mFaces[f];
			
			U32* elements = arena.reserve<U32>(3);

			for (Size i = 0; i < 3; ++i) {
				elements[i] = face.mIndices[i] + vertexCount;
			}
		}
		meshes[m] = std::span(meshes[m].data(), arena.tell<U32>());

		vertexCount += mesh->mNumVertices;
	}

	elements = std::span(elements.data(), arena.tell<U32>());

	return 0;
}

static mstd::Size searchForVertex(std::span<mstd::Vector3f> vertices, mstd::Vector3f search) {
	using namespace mstd;
	
	constexpr F32 EPSILON = 1.0f / 4096.0f;

	constexpr auto isEqual = [](F32 x) {
		return x >= -EPSILON && x <= EPSILON;
	};

	for (Size i = 0; i < vertices.size(); ++i) {
		if (!isEqual(vertices[i].x - search.x)) {
			continue;
		}

		if (!isEqual(vertices[i].y - search.y)) {
			continue;
		}

		if (!isEqual(vertices[i].z - search.z)) {
			continue;
		}

		return i;
	}

	return vertices.size();
}

static void convertToMinkowski(
	mstd::Arena& vertexArena,
	mstd::Arena& faceArena,
	std::span<mstd::Vector3f> vertices,
	std::span<mstd::Vector3f> normals,
	std::span<mstd::U32> elements,
	std::span<mstd::Vector3f>& resultVertices,
	std::span<IndexedFace>& resultFaces
) {
	using namespace mstd;

	resultVertices = std::span(vertexArena.tell<Vector3f>(), 0);
	resultFaces = std::span(faceArena.reserve<IndexedFace>(elements.size() / 3), elements.size() / 3);
	for (Size e = 0; e < elements.size(); e += 3) {
		IndexedFace& face = resultFaces[e / 3];

		for (Size i = 0; i < 3; ++i) {
			Vector3f v = vertices[elements[e + i]];

			Size searchIndex = searchForVertex(resultVertices, v);
			if (searchIndex == resultVertices.size()) {
				resultVertices = std::span(resultVertices.data(), vertexArena.append(&v, 1) + 1);
			}

			face.vertices[i] = searchIndex;
		}

		face.normal = normals[elements[e]];
		face.d = dot(face.normal, resultVertices[face.vertices[0]]);
	}

	resultFaces = std::span(resultFaces.data(), faceArena.tell<IndexedFace>());
}

static void convertToFaces(
	mstd::Arena& arena,
	std::span<mstd::Vector3f> vertices,
	std::span<IndexedFace> faces,
	std::span<Face>& resultFaces
) {
	using namespace mstd;

	resultFaces = std::span(arena.reserve<Face>(faces.size()), faces.size());

	for (Size f = 0; f < faces.size(); ++f) {
		for (Size i = 0; i < 3; ++i) {
			resultFaces[f].vertices[i] = vertices[faces[f].vertices[i]];
		}

		resultFaces[f].normal = faces[f].normal;
		resultFaces[f].d = faces[f].d;
	}
}

static void minkowskiSum(
	mstd::Arena& vertexArena,
	mstd::Arena& faceArena,
	std::span<mstd::Vector3f>&  vertices,
	std::span<IndexedFace>& faces
) {
	using namespace mstd;

	static constexpr F32 w = 0.125f;
	static constexpr F32 l = 1.5f;
	static constexpr F32 h = 0.25f;
	constexpr Vector3f minAABB(-w, -w, -l);
	constexpr Vector3f maxAABB(w, w, h);

	constexpr F32 EPSILON = 1.0f / 4096.0f;
	constexpr F32 ONE = 1.0f - EPSILON;

	std::vector<U8> directions;
	directions.resize(vertices.size());

	for (Size f = 0; f < faces.size(); ++f) {
		const IndexedFace face = faces[f];
	
		U8 directionMask = 0;
		for (Size b = 0; b < 3; ++b) {
			directionMask <<= 2;
			if (face.normal[b] >= EPSILON) {
				directionMask |= 0b01;
			} else if (face.normal[b] <= -EPSILON) {
				directionMask |= 0b10;
			}
		}

		for (Size v = 0; v < 3; ++v) {
			Vector3f& vertex = vertices[face.vertices[v]];
			U8 y = directionMask;
			U8 n = directions[face.vertices[v]];

			for (I32 b = 2; b >= 0; --b) {
				if (y & 0b01 && !(n & 0b01)) {
					vertex[b] -= minAABB[b];
				}
				if (y & 0b10 && !(n & 0b10)) {
					vertex[b] -= maxAABB[b];
				}

				y >>= 2;
				n >>= 2;
			}
			directions[face.vertices[v]] |= directionMask;
		}
	}

	for (Size f = 0; f < faces.size(); ++f) {
		//Vector3f a = vertices[faces[f].vertices[2]] - vertices[faces[f].vertices[0]];
		//Vector3f b = vertices[faces[f].vertices[1]] - vertices[faces[f].vertices[0]];
		//faces[f].normal = normalize(cross(a, b));
		faces[f].d = dot(vertices[faces[f].vertices[0]], faces[f].normal);
	}

	return;
}

static mstd::U32 countSplits(
	const std::span<Face>& faces,
	Plane plane,
	mstd::U32& balance
) {
	using namespace mstd;

	I64 signedBalance = 0;
	
	U32 splits = 0;
	for (const auto& f : faces) {
		I8 status = 0;
		for (U32 i = 0; i < 3; ++i) {
			F32 d = plane.distance(f.vertices[i]);
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

static mstd::U32 bestTriangle(const std::span<Face>& faces) {
	using namespace mstd;

	U32 lowestScore = -1;
	U32 bestTriangle = 0;
	for (Size i = 0; i < faces.size(); ++i) {
		U32 balance;
		Plane p = faces[i];
		U32 splits = countSplits(faces, p, balance);
		U32 score = balance + splits;

		if (score < lowestScore) {
			lowestScore = score;
			bestTriangle = i;
		}
	}

	return bestTriangle;
}

static mstd::Bool splitTriangle(const Plane& plane, const Face& face, std::optional<Face> result[4]) {
	using namespace mstd;

	Vector3f frontVertices[4];
	Vector3f backVertices[4];
	U8 frontCount = 0;
	U8 backCount = 0;
	U8 zeroVerts = 0;

	constexpr F32 EPSILON = 1.0f / 4096.0f;
	
	const Vector3f* prev = face.vertices + 2;
	const Vector3f* curr;
	for (Size i = 0; i < 3; ++i) {
		curr = face.vertices + i;
		F32 prevDistance = plane.distance(*prev);
		if (prevDistance > EPSILON) {
			frontVertices[frontCount++] = *prev;
		} else if (prevDistance < -EPSILON) {
			backVertices[backCount++] = *prev;
		} else {
			frontVertices[frontCount++] = *prev;
			backVertices[backCount++] = *prev;
			++zeroVerts;
		}

		Vector3f split;
		if (plane.intersect(*prev, *curr, split)) {
			frontVertices[frontCount++] = split;
			backVertices[backCount++] = split;
		}

		prev = curr;
	}

	// Coplaner
	if (zeroVerts == 3) {
		return true;
	}

	if (frontCount == 3) {
		result[0] = {
			.vertices = {
				frontVertices[0],
				frontVertices[1],
				frontVertices[2]
			},
		};
		result[0]->normal = face.normal;
		result[0]->d = face.d;
	} else if (frontCount == 4) {
		result[0] = {
			.vertices = {
				frontVertices[0],
				frontVertices[1],
				frontVertices[2]
			}
		};
		result[1] = {
			.vertices = {
				frontVertices[0],
				frontVertices[2],
				frontVertices[3]
			}
		};
		result[0]->normal = face.normal;
		result[0]->d = face.d;
		result[1]->normal = face.normal;
		result[1]->d = face.d;
	}

	if (backCount == 3) {
		result[2] = {
			.vertices = {
				backVertices[0],
				backVertices[1],
				backVertices[2]
			}
		};
		result[2]->normal = face.normal;
		result[2]->d = face.d;
	} else if (backCount == 4) {
		result[2] = {
			.vertices = {
				backVertices[0],
				backVertices[1],
				backVertices[2]
			}
		};
		result[3] = {
			.vertices = {
				backVertices[0],
				backVertices[2],
				backVertices[3]
			}
		};
		result[2]->normal = face.normal;
		result[2]->d = face.d;
		result[3]->normal = face.normal;
		result[3]->d = face.d;
	}

	return false;
}

static void logPartition(mstd::Bool solid, BSP::Node* node, mstd::Size depth) {
	using namespace mstd;

	for (U32 i = 0; i < depth; ++i) {
		U32 color = (i % 7) + 31;
		std::string escapeCode;
		escapeCode = "\033[";
		escapeCode += std::to_string(color);
		escapeCode += "m";
		std::cout << escapeCode << '|';
	}

	if (node) {
		U32 color = (depth % 7) + 31;
		std::string escapeCode;
		escapeCode = "\033[";
		escapeCode += std::to_string(color);
		escapeCode += "m";
		std::cout
			<< escapeCode
			<< std::string(node->value.normal) << ", "
			<< node->value.d << std::endl;
		return;
	}

	for (U32 i = 0; i < depth; ++i) {
		U32 color = (i % 7) + 31;
		std::string escapeCode;
		escapeCode = "\033[";
		escapeCode += std::to_string(color);
		escapeCode += "m";
		std::cout << escapeCode << '|';
	}
	
	U32 color = (depth % 7) + 31;
	std::string escapeCode;
	escapeCode = "\033[";
	escapeCode += std::to_string(color);
	escapeCode += "m";
	if (solid) {
		std::cout << escapeCode << "SOLID" << std::endl;
	} else {
		std::cout << escapeCode << "EMPTY" << std::endl;
	}
}

std::vector<Face> finalFaces;

static void partition(
	mstd::Arena& arena,
	mstd::Arena& frontArena,
	mstd::Arena& backArena,
	const std::span<Face>& faces,
	BSP::Node& node
) {
	using namespace mstd;

	if (faces.empty()) {
		return;
	}

	U32 faceIndex = bestTriangle(faces);
	node.value = faces[faceIndex];
	
	Face* frontStart = frontArena.tell<Face>();
	Face* backStart = backArena.tell<Face>();
	for (Size i = 0; i < faces.size(); ++i) {
		std::optional<Face> result[4] = { std::nullopt };
		if (splitTriangle(node.value, faces[i], result)) {
			finalFaces.push_back(faces[i]);
			continue;
		}

		if (result[0].has_value()) {
			Face& f = result[0].value();
			frontArena.append<Face>(&f, 1);
			finalFaces.push_back(f);
			if (result[1].has_value()) {
				f = result[1].value();
				frontArena.append(&f, 1);
				finalFaces.push_back(f);
			}
		}

		if (result[2].has_value()) {
			Face& f = result[2].value();
			backArena.append(&f, 1);
			if (result[3].has_value()) {
				f = result[3].value();
				backArena.append(&f, 1);
			}
		}
	}

	std::span<Face> frontFaces = std::span(frontStart, frontArena.tell<Face>());
	std::span<Face> backFaces = std::span(backStart, backArena.tell<Face>());

	if (frontFaces.size()) {
		node[0] = arena.reserve<BSP::Node>(1);
		partition(arena, frontArena, backArena, frontFaces, *node[0]);
	} else {
		node[0] = nullptr;
	}

	if (backFaces.size()) {
		node[1] = arena.reserve<BSP::Node>(1);
		partition(arena, frontArena, backArena, backFaces, *node[1]);
	} else {
		node[1] = nullptr;
	}

	frontArena.truncate(frontFaces.data());
	backArena.truncate(backFaces.data());
}

static void merge(
	mstd::Arena& arena,
	mstd::Arena& frontArena,
	mstd::Arena& backArena,
	const std::span<Face>& faces,
	BSP::Node& node
) {
	using namespace mstd;

	if (faces.empty()) {
		return;
	}

	Face* frontStart = frontArena.tell<Face>();
	Face* backStart = backArena.tell<Face>();
	for (Size i = 0; i < faces.size(); ++i) {
		std::optional<Face> result[4] = { std::nullopt };
		if (splitTriangle(node.value, faces[i], result)) {
			continue;
		}

		if (result[0].has_value()) {
			Face& f = result[0].value();
			frontArena.append<Face>(&f, 1);
			if (result[1].has_value()) {
				f = result[1].value();
				frontArena.append(&f, 1);
			}
		}

		if (!node[1]) {
			continue;
		}

		if (result[2].has_value()) {
			Face& f = result[2].value();
			backArena.append(&f, 1);
			if (result[3].has_value()) {
				f = result[3].value();
				backArena.append(&f, 1);
			}
		}
	}

	std::span<Face> frontFaces = std::span(frontStart, frontArena.tell<Face>());
	std::span<Face> backFaces = std::span(backStart, backArena.tell<Face>());

	if (frontFaces.size()) {
		if (node[0]) {
			merge(arena, frontArena, backArena, frontFaces, *node[0]);
		} else {
			node[0] = arena.reserve<BSP::Node>(1);
			partition(arena, frontArena, backArena, frontFaces, *node[0]);
		}
	}

	if (backFaces.size() && node[1]) {
		merge(arena, frontArena, backArena, backFaces, *node[1]);
	}

	frontArena.truncate(frontFaces.data());
	backArena.truncate(backFaces.data());
}

static void optimizeTopology(
	const std::span<Face>& faces,
	std::vector<mstd::Vector3f>& vertices,
	std::vector<mstd::Vector3f>& normals,
	std::vector<mstd::U32>& indices
) {
	using namespace mstd;

	vertices.reserve(faces.size() * 3);
	normals.reserve(faces.size() * 3);
	indices.reserve(faces.size() * 3);
	for (const auto& f : faces) {
		std::optional<U32> triangle[3] = {std::nullopt};
		for (Size i = 0; i < vertices.size(); ++i) {
			if (f.normal != normals[i]) {
				continue;
			}
			for (Size j = 0; j < 3; ++j) {
				if (triangle[j]) {
					continue;
				}
				if (vertices[i] == f.vertices[j]) {
					triangle[j] = i;
				}
			}
		}

		for (Size j = 0; j < 3; ++j) {
			if (triangle[j].has_value()) {
				indices.push_back(triangle[j].value());
			} else {
				indices.push_back(vertices.size());
				vertices.push_back(f.vertices[j]);
				normals.push_back(f.normal);
			}
		}
	}
}

void BSP::build(const std::string& path, mstd::Arena& arena) {
	using namespace mstd;

	Arena faceArena((Size)1 << 24);

	std::span<std::span<U32>> meshes;
	std::span<Vector3f> vertices;
	std::span<Vector3f> normals;
	std::span<U32> elements;

	if (import(path, faceArena, meshes, vertices, normals, elements)) return;

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

	vertexArray = VertexArray(attributes, vertices.size(), elements.size(), sizeof(U32));
	vertexArray.writeAttributes(0, vertices.data(), vertices.size());
	vertexArray.writeAttributes(1, normals.data(), normals.size());
	vertexArray.writeElements(elements.data(), elements.size());
	elementCount = elements.size();

	Size totalSize = 0l;
	for (Size i = 0; i < meshes.size(); ++i) {
		totalSize += meshes[i].size();
	}
	totalSize /= 3;
	totalSize *= sizeof(Face);

	std::sort(
		meshes.begin(),
		meshes.end(),
		[](std::span<U32> a, std::span<U32> b) {
			return a.size() < b.size();
		}
	);

	Arena vertexArena(vertices.size() * 16);
	std::span<Vector3f> minkowskiVertices;
	std::span<IndexedFace> minkowskiFaces;
	convertToMinkowski(vertexArena, faceArena, vertices, normals, meshes[0], minkowskiVertices, minkowskiFaces);

	minkowskiSum(vertexArena, faceArena, minkowskiVertices, minkowskiFaces);

	std::span<Face> faces;
	convertToFaces(faceArena, minkowskiVertices, minkowskiFaces, faces);

	Arena frontArena(totalSize * 256);
	Arena backArena(totalSize * 256);
	partition(arena, frontArena, backArena, faces, rootSplit);

	for (Size m = 1; m < meshes.size(); ++m) {
		vertexArena.truncate(minkowskiVertices.data());
		faceArena.truncate(minkowskiFaces.data());
		convertToMinkowski(vertexArena, faceArena, vertices, normals, meshes[m], minkowskiVertices, minkowskiFaces);
		minkowskiSum(vertexArena, faceArena, minkowskiVertices, minkowskiFaces);
		convertToFaces(faceArena, minkowskiVertices, minkowskiFaces, faces);
		merge(arena, frontArena, backArena, faces, rootSplit);
	}
}

static mstd::Bool findNode(const BSP::Node& b, const mstd::Vector3f& p, const BSP::Node*& result) {
	if (b.value.distance(p) >= 0.0f) {
		if (b[0]) {
			return findNode(*b[0], p, result);
		}

		result = &b;

		return false;
	} else {
		if (b[1]) {
			return findNode(*b[1], p, result);
		}

		result = &b;

		return true;
	}
}

mstd::Bool BSP::colliding(const mstd::Vector3f& point) const {
	const BSP::Node* result;
	return findNode(rootSplit, point, result);
}

mstd::Bool BSP::isSolid(const mstd::Vector3f& point, const Node* node) const {
	using namespace mstd;

	while (1) {
		if (node->value.distance(point) >= 0.0f) {
			if (node->data()[0]) {
				node = node->data()[0];
			} else {
				return false;
			}
		} else {
			if (node->data()[1]) {
				node = node->data()[1];
			} else {
				return true;
			}
		}
	}
}

mstd::Bool BSP::clip(
	const mstd::Vector3f& from,
	const mstd::Vector3f& to,
	Trace& result
) const {
	return clip(from, to, result, rootSplit);
}

mstd::Bool BSP::clip(
	const mstd::Vector3f& from,
	const mstd::Vector3f& to,
	Trace& result,
	const Node& node
) const {
	using namespace mstd;

	const Plane& p = node.value;

	F32 fromDist = p.distance(from);
	F32 toDist = p.distance(to);

	if (fromDist >= 0.0f && toDist >= 0.0f) {
		if (node[0]) {
			return clip(from, to, result, *node[0]);
		} else {
			result.point = to;
			return false;
		}
	}

	if (fromDist < 0.0f && toDist < 0.0f) {
		if (node[1]) {
			return clip(from, to, result, *node[1]);
		} else {
			result.point = from;
			return true;
		}
	}

	constexpr F32 EPSILON = 1.0f / 4096.0f;

	Bool side = fromDist < 0.0f;
	F32 nearT, farT;
	if (side) {
		nearT = (fromDist + EPSILON) / (fromDist - toDist);
		farT = (fromDist - EPSILON) / (fromDist - toDist);
	} else {
		nearT = (fromDist - EPSILON) / (fromDist - toDist);
		farT = (fromDist + EPSILON) / (fromDist - toDist);
	}

	nearT = std::clamp(nearT, 0.0f, 1.0f);
	farT = std::clamp(farT, 0.0f, 1.0f);

	Vector3f nearSplit = from + (to - from) * nearT;
	Vector3f farSplit = from + (to - from) * farT;

	if (node[side]) {
		if (clip(from, nearSplit, result, *node[side])) {
			return true;
		}
	}

	if (node[side ^ 1]) {
		if (!isSolid(farSplit, node[side ^ 1])) {
			return clip(farSplit, to, result, *node[side ^ 1]);
		}
	}

	if (side) {
		result.point = to;
		return false;
	} else {
		result.plane = node.value;
		result.point = nearSplit;
		return true;
	}
}

void printNode(const BSP::Node& b, mstd::U32 pad) {
	using namespace mstd;

	for (U32 i = 0; i < pad; ++i) {
		U32 color = (i % 7) + 31;
		std::string escapeCode;
		escapeCode = "\033[";
		escapeCode += std::to_string(color);
		escapeCode += "m";
		std::cout << escapeCode << '|';
	}

	U32 color = (pad % 7) + 31;
	std::string escapeCode;
	escapeCode = "\033[";
	escapeCode += std::to_string(color);
	escapeCode += "m";
	std::cout << escapeCode << std::string(b.value.normal) << ", " << b.value.d << std::endl;

	if (b[0]) {
		printNode(*b[0], pad + 1);
	} else {
		for (U32 i = 0; i < pad + 1; ++i) {
			U32 color = (i % 7) + 31;
			std::string escapeCode;
			escapeCode = "\033[";
			escapeCode += std::to_string(color);
			escapeCode += "m";
			std::cout << escapeCode << '|';
		}
		U32 color = (pad % 7) + 31;
		std::string escapeCode;
		escapeCode = "\033[";
		escapeCode += std::to_string(color);
		escapeCode += "m";
		std::cout << escapeCode << "EMPTY" << std::endl;
	}

	if (b[1]) {
		printNode(*b[1], pad + 1);
	} else {
		++pad;
		for (U32 i = 0; i < pad + 1; ++i) {
			U32 color = (i % 7) + 31;
			std::string escapeCode;
			escapeCode = "\033[";
			escapeCode += std::to_string(color);
			escapeCode += "m";
			std::cout << escapeCode << '|';
		}
		U32 color = (pad % 7) + 31;
		std::string escapeCode;
		escapeCode = "\033[";
		escapeCode += std::to_string(color);
		escapeCode += "m";
		std::cout << escapeCode << "SOLID" << std::endl;
	}
}

void BSP::print() const {
	printNode(rootSplit, 0);
}

}
