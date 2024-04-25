#pragma once
#include "BasicTypes.h"
#include <string>
#include <list>
#include <map>
#include <thread>

class BaseObject;
class Mesh;

class MeshData
{
public:
	std::string Name;

	unsigned int* faces;
	unsigned int* NormalIndices;
	size_t stride;

	/* X, Y, Z, X, Y, Z, ... */
	float* vertices;

	/* X, Y, Z, X, Y, Z, ... */
	float* normals;

	const unsigned int GetFaceCount() const { return FaceCount; }
	const unsigned int GetVertexCount() const { return VertexCount; }

	void OwnerDeleted() { ReferenceCount--; }

	const float* GetVertices() const {
		return vertices;
	}

	const unsigned int* GetFaces() const {
		return faces;
	}

	size_t GetVertexStrideBytes() const {
		return sizeof(float) * 3;
	}

	///
	/// Get the geometric normal and the shading normal at `face_idx' th face.
	///
	void GetNormal(float Ng[3], float Ns[3], const unsigned int face_idx, const float u, const float v) const {
		unsigned int f0, f1, f2;
		Vector v0, v1, v2;

		f0 = NormalIndices[3 * face_idx + 0];
		f1 = NormalIndices[3 * face_idx + 1];
		f2 = NormalIndices[3 * face_idx + 2];

		v0.X = normals[3 * f0 + 0];
		v0.Y = normals[3 * f0 + 1];
		v0.Z = normals[3 * f0 + 2];

		v1.X = normals[3 * f1 + 0];
		v1.Y = normals[3 * f1 + 1];
		v1.Z = normals[3 * f1 + 2];

		v2.X = normals[3 * f2 + 0];
		v2.Y = normals[3 * f2 + 1];
		v2.Z = normals[3 * f2 + 2];

		Vector nor = (v0 * u + v1 * v + v2 * (1 - v - u));

		Ng[0] = nor.X;
		Ng[1] = nor.Y;
		Ng[2] = nor.Z;

		Ns[0] = Ng[0];
		Ns[1] = Ng[1];
		Ns[2] = Ng[2];

	}

private:
	friend class Renderer;
	friend class GC;

	MeshData();
	~MeshData();

	unsigned int FaceCount;
	unsigned int VertexCount;
	unsigned int NormalCount;
	int ReferenceCount;
};

class GC
{
public:
	GC(std::string dir);
	~GC();
	static void AddObject(BaseObject* obj);
	static void RemoveObject(BaseObject* obj);
	static Mesh* LoadMesh(std::string Name);
	bool LoadObj(std::string Name, MeshData* Data);
	void Quit();
	static std::list<Data*> Pointers;
private:
	static std::map<std::string, MeshData*> LoadedMeshes;
	std::map<std::string, std::ifstream*> ModelStreams;
	std::string ActiveDir;
	std::thread Cleaner;
	bool bQuitting;
	void CleanRunner();
};