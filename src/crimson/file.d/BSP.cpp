#include <crimson/file.d/BSP.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <mstd/misc>
#include <array>
#include <span>
#include <optional>

#include <cassert>

namespace ct {

struct Face : Plane {
	mstd::Vector3f vertices[3];
};

static mstd::Status import(
	const std::string& path,
	mstd::Arena& arena,
	std::span<Face>& faces
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

	Face* start = arena.tell<Face>();
	for (Size m = 0; m < scene->mNumMeshes; ++m) {
		aiMesh* mesh = scene->mMeshes[m];
		for (Size f = 0; f < mesh->mNumFaces; ++f) {
			aiFace face = mesh->mFaces[f];
			
			Face result = {
				.vertices = {
					*(Vector3f*)&mesh->mVertices[face.mIndices[0]],
					*(Vector3f*)&mesh->mVertices[face.mIndices[1]],
					*(Vector3f*)&mesh->mVertices[face.mIndices[2]],
				},
			};
			result.normal = *(Vector3f*)&mesh->mNormals[face.mIndices[0]];
			result.d = dot(result.normal, result.vertices[0]);

			arena.append(1, &result);
		}
	}

	faces = std::span(start, arena.tell<Face>());

	return 0;
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
		U32 splits = countSplits(faces, faces[i], balance);
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
	
	const Vector3f* prev = face.vertices + 2;
	const Vector3f* curr;
	for (Size i = 0; i < 3; ++i) {
		curr = face.vertices + i;
		F32 prevDistance = plane.distance(*prev);
		if (prevDistance > 1.0f / 1024.0f) {
			frontVertices[frontCount++] = *prev;
		} else if (prevDistance < -1.0f / 1024.0f) {
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
		escapeCode = "\e[";
		escapeCode += std::to_string(color);
		escapeCode += "m";
		std::cout << escapeCode << '|';
	}

	if (node) {
		U32 color = (depth % 7) + 31;
		std::string escapeCode;
		escapeCode = "\e[";
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
		escapeCode = "\e[";
		escapeCode += std::to_string(color);
		escapeCode += "m";
		std::cout << escapeCode << '|';
	}
	
	U32 color = (depth % 7) + 31;
	std::string escapeCode;
	escapeCode = "\e[";
	escapeCode += std::to_string(color);
	escapeCode += "m";
	if (solid) {
		std::cout << escapeCode << "SOLID" << std::endl;
	} else {
		std::cout << escapeCode << "EMPTY" << std::endl;
	}
}

static void partition(
	mstd::Arena& arena,
	mstd::Arena& frontArena,
	mstd::Arena& backArena,
	const std::span<Face>& faces,
	BSP::Node& node,
	mstd::Size depth
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
			continue;
		}

		if (result[0].has_value()) {
			Face& f = result[0].value();
			frontArena.append<Face>(1, &f);
			if (result[1].has_value()) {
				f = result[1].value();
				frontArena.append(1, &f);
			}
		}

		if (result[2].has_value()) {
			Face& f = result[2].value();
			backArena.append(1, &f);
			if (result[3].has_value()) {
				f = result[3].value();
				backArena.append(1, &f);
			}
		}
	}

	std::span<Face> frontFaces = std::span(frontStart, frontArena.tell<Face>());
	std::span<Face> backFaces = std::span(backStart, backArena.tell<Face>());

	logPartition(false, &node, depth);

	if (frontFaces.size()) {
		node[0] = arena.reserve<BSP::Node>(1);
		partition(arena, frontArena, backArena, frontFaces, *node[0], depth + 1);
	} else {
		logPartition(false, nullptr, depth + 1);
		node[0] = nullptr;
	}

	if (backFaces.size()) {
		node[1] = arena.reserve<BSP::Node>(1);
		partition(arena, frontArena, backArena, backFaces, *node[1], depth + 1);
	} else {
		logPartition(true, nullptr, depth + 1);
		node[1] = nullptr;
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

void BSP::build(const std::string& path) {
	using namespace mstd;

	Arena bspArena(1L << 24);

	std::span<Face> faces;

	if (import(path, bspArena, faces)) return;

	Arena frontArena(faces.size_bytes() * 256);
	Arena backArena(faces.size_bytes() * 256);

	partition(arena, frontArena, backArena, faces, rootSplit, 0);

	std::vector<Vector3f> vertices;
	std::vector<Vector3f> normals;
	std::vector<U32> indices;
	optimizeTopology(faces, vertices, normals, indices);

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

	indexCount = indices.size();
	vertexArray.allocateElements(indices.size() * sizeof(U32));
	vertexArray.writeElements(indices.data(), indices.size());
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

mstd::Bool BSP::intersect(
	const mstd::Vector3f& from,
	const mstd::Vector3f& to,
	BSP::Intersection& result
) const {
	return intersect(from, to, result, rootSplit);
}

mstd::Bool BSP::intersect(
	const mstd::Vector3f& from,
	const mstd::Vector3f& to,
	BSP::Intersection& result,
	const BSP::Node& node
) const {
	using namespace mstd;

	F32 distA = node.value.distance(from);
	F32 distB = node.value.distance(to);

	if (distA >= 0.0f && distB >= 0.0f) {
		if (node[0]) {
			return intersect(from, to, result, *(node[0]));
		} else {
			result.point = to;
			return false;
		}
	}

	if (distA <= 0.0f && distB <= 0.0f) {
		if (node[1]) {
			return intersect(from, to, result, *(node[1]));
		} else {
			result.point = from;
			return true;
		}
	}

	static constexpr F32 epsilon = 1.0f / 256.0f;

	F32 frontT = (distA - epsilon) / (distA - distB);
	F32 backT = (distA + epsilon) / (distA - distB);

	Bool negative = distA < 0.0f;
	Vector3f nearSplit, farSplit;
	if (negative) {
		nearSplit = from + (to - from) * backT;
		farSplit = from + (to - from) * frontT;
	} else {
		nearSplit = from + (to - from) * frontT;
		farSplit = from + (to - from) * backT;
	}
	
	result.plane = node.value;
	
	if (node[negative]) {
		if (intersect(from, nearSplit, result, *(node[negative]))) {
			return true;
		}
	}

	if (colliding(farSplit)) {
		result.point = nearSplit;
		return true;
	}

	if (node[1 - negative]) {
		return intersect(farSplit, to, result, *(node[1 - negative]));
	} else {
		return true;
	}
}

void printNode(const BSP::Node& b, mstd::U32 pad) {
	using namespace mstd;

	for (U32 i = 0; i < pad; ++i) {
		U32 color = (i % 7) + 31;
		std::string escapeCode;
		escapeCode = "\e[";
		escapeCode += std::to_string(color);
		escapeCode += "m";
		std::cout << escapeCode << '|';
	}

	U32 color = (pad % 7) + 31;
	std::string escapeCode;
	escapeCode = "\e[";
	escapeCode += std::to_string(color);
	escapeCode += "m";
	std::cout << escapeCode << std::string(b.value.normal) << ", " << b.value.d << std::endl;

	if (b[0]) {
		printNode(*b[0], pad + 1);
	} else {
		for (U32 i = 0; i < pad + 1; ++i) {
			U32 color = (i % 7) + 31;
			std::string escapeCode;
			escapeCode = "\e[";
			escapeCode += std::to_string(color);
			escapeCode += "m";
			std::cout << escapeCode << '|';
		}
		U32 color = (pad % 7) + 31;
		std::string escapeCode;
		escapeCode = "\e[";
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
			escapeCode = "\e[";
			escapeCode += std::to_string(color);
			escapeCode += "m";
			std::cout << escapeCode << '|';
		}
		U32 color = (pad % 7) + 31;
		std::string escapeCode;
		escapeCode = "\e[";
		escapeCode += std::to_string(color);
		escapeCode += "m";
		std::cout << escapeCode << "SOLID" << std::endl;
	}
}

void BSP::print() const {
	printNode(rootSplit, 0);
}

}
