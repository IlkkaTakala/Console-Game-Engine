#pragma once
#include "Core.h"
#include <string>

class MeshData;

class Mesh
{
public:
	Mesh();

	const unsigned int GetFaceCount() const { return LoadedData->GetFaceCount(); }
	const unsigned int GetVertexCount() const { return LoadedData->GetVertexCount(); }

	const float* GetVertices() const {
		return LoadedData->vertices;
	}

	const unsigned int* GetFaces() const {
		return LoadedData->faces;
	}

	size_t GetVertexStrideBytes() const {
		return sizeof(float) * 3;
	}

	///
	/// Get the geometric normal and the shading normal at `face_idx' th face.
	///
	void GetNormal(float Ng[3], float Ns[3], const unsigned int face_idx, const float u, const float v) const {
		LoadedData->GetNormal(Ng, Ns, face_idx, u, v);
	}
private:
	friend class Renderer;
	friend class GC;
	friend class VisibleObject;
	~Mesh();
	Mesh(const Mesh&);
	Mesh& operator=(const Mesh&);
	bool ModelLoaded;
	bool RequiresBuild;
	bool AddedToScene;
	float transform[4][4];
	std::string NodeName;

	/* X, Y, Z, X, Y, Z, ... */
	float* Vertices;

	unsigned char mat;
	unsigned short roughness;
	MeshData* LoadedData;

	void Transform(Vector translate, Vector rotation, Vector scale);
};

class VisibleObject : public BaseObject
{
public:
	VisibleObject();
	virtual ~VisibleObject() { delete Model; }
	void SetLocation(Vector NewLocation);
	void SetRotation(Vector NewRotation);
	void SetScale(Vector NewScale);
	void AddLocation(Vector NewLocation) { SetLocation(NewLocation + Location); };
	void AddRotation(Vector NewRotation) { SetRotation(NewRotation + Rotation); };
	void SetModel(std::string Name);
	void SetMaterial(const unsigned char Material) { Model->mat = Material; }
	const std::string GetName() const { return Model->NodeName; }

	virtual void BeginPlay() override {};
protected:
	friend class Renderer;
	friend class GC;
	

	Mesh* Model;

	Vector Location;
	Vector Rotation;
	Vector Scale;
};