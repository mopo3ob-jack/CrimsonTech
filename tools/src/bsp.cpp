#include "../../crimson.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace hdb;
using namespace hdb::ct;

struct Face;

struct BSPNode {
	BSPNode* front;
	BSPNode* back;
	Face* polygon;
}* nodes;

unsigned int nodeCount;

struct Face {
	Plane plane;
	Vector3f* vertices;
	BSPNode* node;
	unsigned int count;
	bool isProcessed;
}* faces;

unsigned int faceCount;
unsigned int maxFaces;

Vector3f* vertices;
unsigned int vertexCount;
unsigned int maxVertices;

namespace ClipResult {

std::vector<Vector3f> front;
std::vector<Vector3f> back;

}

void import(char* path, float vertexRatio) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, 0);

	vertexCount = 0;
	faceCount = 0;
	for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
		aiMesh* mesh = scene->mMeshes[m];
		faceCount += mesh->mNumFaces;

		for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
			vertexCount += mesh->mFaces[f].mNumIndices;
		}
	}

	maxVertices = vertexCount * vertexRatio;
	maxFaces = faceCount * vertexRatio;
	hdb::alloc(vertices, maxVertices);
	hdb::alloc(faces, maxFaces, 8);
	hdb::alloc(nodes, maxFaces);

	unsigned int vertexIndex = 0;
	unsigned int faceIndex = 0;
	for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
		aiMesh* mesh = scene->mMeshes[m];

		for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
			aiFace face = mesh->mFaces[f];

			unsigned int oldVertexIndex = vertexIndex;
			for (unsigned int v = 0; v < face.mNumIndices; ++v) {
				vertices[vertexIndex] = *(Vector3f*)(mesh->mVertices + face.mIndices[v]);
				float temp = vertices[vertexIndex].y;
				vertices[vertexIndex].y = vertices[vertexIndex].z;
				vertices[vertexIndex++].z = -temp;
			}

			faces[faceIndex++] = {
				Plane(vertices + oldVertexIndex),
				vertices + oldVertexIndex,
				nodes,
				face.mNumIndices,
				false
			};
		}
	}
}

void clipPolygon(const Plane& plane, const Vector3f* vertices, unsigned int vertexCount) {
	ClipResult::front.clear();
	ClipResult::back.clear();

	float distanceA = plane.distanceToPlane(vertices[0]);

	for (unsigned int i = 1; i < vertexCount; ++i) {
		const Vector3f& a = vertices[i - 1];
		const Vector3f& b = vertices[i];
		float distanceB = plane.distanceToPlane(b);

		if (distanceA >= 0.0f) {
			ClipResult::front.push_back(a);
			if (distanceB < 0.0f) {
				Vector3f p = plane.intersection(a, b);
				ClipResult::front.push_back(p);
				ClipResult::back.push_back(p);
			}
		} else {
			ClipResult::back.push_back(a);
			if (distanceB >= 0.0f) {
				Vector3f p = plane.intersection(a, b);
				ClipResult::front.push_back(p);
				ClipResult::back.push_back(p);
			}
		}

		distanceA = distanceB;
	}

	const Vector3f& a = vertices[vertexCount - 1];
	const Vector3f& b = vertices[0];
	float distanceB = plane.distanceToPlane(b);

	if (distanceA >= 0.0f) {
		ClipResult::front.push_back(a);
		if (distanceB < 0.0f) {
			Vector3f p = plane.intersection(a, b);
			ClipResult::front.push_back(p);
			ClipResult::back.push_back(p);
		}
	} else {
		ClipResult::back.push_back(a);
		if (distanceB >= 0.0f) {
			Vector3f p = plane.intersection(a, b);
			ClipResult::front.push_back(p);
			ClipResult::back.push_back(p);
		}
	}
}

enum PlaneState {
	coincidental,
	front,
	back
};

PlaneState intersects(const Plane& plane, const Face& face) {
	bool reference = plane.distanceToPlane(face.vertices[0]) >= 0;
	if (reference) {
		for (unsigned int i = 1; i < face.count; ++i) {
			if (plane.distanceToPlane(face.vertices[i]) < -FLT_EPSILON)
				return coincidental;
		}
	} else {
		for (unsigned int i = 1; i < face.count; ++i) {
			if (plane.distanceToPlane(face.vertices[i]) > FLT_EPSILON)
				return back;
		}
	}

	return reference ? front : back;
}

Face* selectBestFace(BSPNode* filter) {
	unsigned int lowestScore = 0xFFFFFFFF;
	Face* bestFace = nullptr;
	for (unsigned int i = 0; i < faceCount; ++i) {
		Face* face = faces + i;
		if (face->node == filter && !face->isProcessed) {
			unsigned int score = 0;
			for (unsigned int j = 0; j < faceCount; ++j) {
				PlaneState intersection = intersects(face->plane, faces[j]);
				if (intersection == PlaneState::coincidental)
					score += 2;
				else if (intersection == PlaneState::front)
					++score;
				else
					--score;
			}

			if (score < lowestScore) {
				bestFace = faces + i;
				lowestScore = score;
			}
		}
	}

	return bestFace;
}

void buildBSPNode(BSPNode* node) {
	node->polygon = selectBestFace(node);
	if (node->polygon == nullptr) {
		return;
	}
	node->polygon->isProcessed = true;

	node->front = nodes + nodeCount++;
	node->back = nodes + nodeCount++;

	Plane plane = node->polygon->plane;
	for (unsigned int i = 0; i < faceCount; ++i) {
		if (faces[i].node == node && faces[i].isProcessed == false) {
			Face* face = faces + i;
			bool reference = plane.distanceToPlane(face->vertices[0]) >= 0;
			if (reference) {
				for (unsigned int i = 1; i < face->count; ++i) {
					if (plane.distanceToPlane(face->vertices[i]) < -FLT_EPSILON)
						goto intersection;
				}
			} else {
				for (unsigned int i = 1; i < face->count; ++i) {
					if (plane.distanceToPlane(face->vertices[i]) > FLT_EPSILON)
						goto intersection;
				}
			}

			//The polygon does NOT intersect the current plane

			if (reference) {
				face->node = node->front;
			} else {
				face->node = node->back;
			}

			continue;

			intersection: //The polygon intersects the current plane

			clipPolygon(plane, face->vertices, face->count);

			face->vertices = vertices + vertexCount;
			face->count = ClipResult::front.size();
			for (unsigned int v = 0; v < face->count; ++v) {
				vertices[vertexCount++] = ClipResult::front[v];
			}
			face->node = node->front;
			face->isProcessed = false;

			Face* back = faces + faceCount++;
			back->vertices = vertices + vertexCount;
			back->count = ClipResult::back.size();
			for (unsigned int v = 0; v < back->count; ++v) {
				vertices[vertexCount++] = ClipResult::back[v];
			}
			back->node = node->back;
			back->isProcessed = false;
		}
	}

	buildBSPNode(node->front);
	buildBSPNode(node->back);
}

void printBSPNode(BSPNode* node, unsigned int depth = 0) {
	for (unsigned int i = 0; i < depth; ++i) {
		printf("\e[%dm|\t", (i % 8) + 30);
	}

	Plane plane = node->polygon->plane;
	printf("\e[%dm(%f, %f, %f), %f\t%d\n", (depth % 8) + 30, plane.normal.x, plane.normal.y, plane.normal.z, plane.distance, node->polygon->count);

	++depth;
	if (!node->front->polygon) {
		for (unsigned int i = 0; i < depth; ++i) {
			printf("\e[%dm|\t", (i % 8) + 30);
		}

		printf("Empty\n");
	} else {
		printBSPNode(node->front, depth);
	}

	if (!node->back->polygon) {
		for (unsigned int i = 0; i < depth; ++i) {
			printf("\e[%dm|\t", (i % 8) + 30);
		}

		printf("Solid\n");
	} else {
		printBSPNode(node->back, depth);
	}
}

int main(int argc, char** argv) {
	float vertexRatio = 2.0f;

	char* inputPath = nullptr;
	char* outputPath = nullptr;

	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] != '-') {
			inputPath = argv[i];
		} else if (!strcmp("-ratio", argv[i]))
			vertexRatio = strtof(argv[++i], nullptr);
		else if (!strcmp("-o", argv[i]))
			outputPath = argv[++i];
		else
			hdb::error("Unknown Option: %s\n", argv[i]);
	}

	if (inputPath == nullptr) {
		hdb::error("Input file was not sepcified\n");
		return 1;
	}

	int fd = open(inputPath, O_RDONLY);
	if (fd == -1) {
		hdb::error("Cannot access input file (Or filepath was invalid)\n");
		return 1;
	}
	close(fd);

	import(inputPath, vertexRatio);

	char output[256];
	if (outputPath == nullptr) {
		unsigned int i;
		while (inputPath[i]) {
			if (inputPath[i] == '.') {
				break;
			} else {
				output[i] = inputPath[i];
			}
			++i;
		}

		outputPath = output;
		outputPath[i] = '.';
		outputPath[i+1] = 'o';
		outputPath[i+2] = 'b';
		outputPath[i+3] = 'j';
		outputPath[i+4] = 0;
	}

	++nodeCount;
	buildBSPNode(nodes);
	printBSPNode(nodes);
	// for (unsigned int i = 0; i < nodeCount; ++i) {
	// 	if (nodes[i].polygon) {
	// 		printf("%p\n", nodes[i].polygon);
	// 	}
	// }

	FILE* file = fopen(outputPath, "w");

	for (unsigned int f = 0; f < faceCount; ++f) {
		Face face = faces[f];

		for (unsigned int v = 0; v < face.count; ++v) {
			fprintf(file, "v %f %f %f\n", face.vertices[v].x, face.vertices[v].y, face.vertices[v].z);
		}

		fprintf(file, "f ");
		for (unsigned int v = 0; v < face.count; ++v) {
			fprintf(file, "%d ", face.vertices + v - vertices + 1);
		}
		fprintf(file, "\n");
	}
	
	fclose(file);
	
	hdb::free(vertices);
	hdb::free(faces);
	hdb::free(nodes);

	return 0;
}