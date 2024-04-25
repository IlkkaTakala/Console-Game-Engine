#include "VisibleObject.h"

Mesh::Mesh()
{
	Vertices = nullptr;
	ModelLoaded = false; 
	LoadedData = nullptr;
	RequiresBuild = true;
	AddedToScene = false;
	mat = 0;
	transform[0][0] = 1;
	transform[1][1] = 1;
	transform[2][2] = 1;
	transform[3][3] = 1;
	NodeName = "";
	roughness = 100;
}

Mesh::~Mesh()
{
	delete[] Vertices;
	//delete Node;
	LoadedData->OwnerDeleted();
}

void Mesh::Transform(Vector translate, Vector rotation, Vector scale)
{
	if (LoadedData == nullptr) return;

	std::fill(&transform[0][0], &transform[0][0] + sizeof(transform) / sizeof(transform[0][0]), 0.f);

	transform[0][0] = 1;
	transform[1][1] = 1;
	transform[2][2] = 1;
	transform[3][3] = 1;

	float angleX = rotation.X * PI / 180.f;
	float angleY = rotation.Y * PI / 180.f;
	float angleZ = rotation.Z * PI / 180.f;

	float s_x = sin(angleX);
	float c_x = cos(angleX);

	float s_y = sin(angleY);
	float c_y = cos(angleY);

	float s_z = sin(angleZ);
	float c_z = cos(angleZ);

	transform[0][0] = c_y * c_z;
	transform[0][1] = (c_y * s_z * s_x - s_y * c_x);
	transform[0][2] = (c_y * s_z * c_x + s_y * s_x);

	transform[1][0] = s_y * c_z;
	transform[1][1] = (s_y * s_z * s_x + c_y * c_x);
	transform[1][2] = (s_y * s_z * c_x - c_y * s_x);

	transform[2][0] = -s_z;
	transform[2][1] = c_z * s_x;
	transform[2][2] = c_z * c_x;

	transform[0][0] *= scale.X;
	transform[1][0] *= scale.X;
	transform[2][0] *= scale.X;
	transform[0][1] *= scale.Y;
	transform[1][1] *= scale.Y;
	transform[2][1] *= scale.Y;
	transform[0][2] *= scale.Z;
	transform[1][2] *= scale.Z;
	transform[2][2] *= scale.Z;

	transform[3][0] = translate.X ;
	transform[3][1] = translate.Y;
	transform[3][2] = translate.Z;

	

	RequiresBuild = true;

	//for (size_t i = 0; i < LoadedData->GetVertexCount(); i++) {
	//	Vertices[i * 3 + 0] = LoadedData->Vertices[i * 3 + 0] * scale.X;
	//	Vertices[i * 3 + 1] = LoadedData->Vertices[i * 3 + 1] * scale.Y;
	//	Vertices[i * 3 + 2] = LoadedData->Vertices[i * 3 + 2] * scale.Z;
	//}

	//float* copy = new float[LoadedData->GetVertexCount() * 3]();

	//float angleX = rotation.X * PI / 180.f;
	//float angleY = -rotation.Y * PI / 180.f;
	//float angleZ = rotation.Z * PI / 180.f;

	///* Axis 1, 0, 0 */
	//float s = sin(angleX);
	//float c = cos(angleX);
	////memcpy(Vertices, LoadedData->Vertices, LoadedData->GetFaceCount() * 3 * 3 * sizeof(float));
	//memcpy(copy, Vertices, LoadedData->GetVertexCount() * 3 * sizeof(float));

	//for (size_t i = 0; i < LoadedData->GetVertexCount(); i++) {
	//	Vertices[i * 3 + 1] = copy[i * 3 + 1] * c - copy[i * 3 + 2] * s;
	//	Vertices[i * 3 + 2] = copy[i * 3 + 1] * s + copy[i * 3 + 2] * c;
	//}
	///* Axis 0, 1, 0 */
	//s = sin(angleY);
	//c = cos(angleY);
	//memcpy(copy, Vertices, LoadedData->GetVertexCount() * 3 * sizeof(float));

	//for (size_t i = 0; i < LoadedData->GetVertexCount(); i++) {
	//	Vertices[i * 3 + 0] = copy[i * 3 + 0] * c + copy[i * 3 + 2] * s;
	//	Vertices[i * 3 + 2] = copy[i * 3 + 0] * -s + copy[i * 3 + 2] * c;
	//}

	///* Axis 0, 0, 1 */
	//s = sin(angleZ);
	//c = cos(angleZ);
	//memcpy(copy, Vertices, LoadedData->GetVertexCount() * 3 * sizeof(float));

	//for (size_t i = 0; i < LoadedData->GetVertexCount(); i++) {
	//	Vertices[i * 3 + 0] = copy[i * 3 + 0] * c + copy[i * 3 + 1] * -s;
	//	Vertices[i * 3 + 1] = copy[i * 3 + 1] * c + copy[i * 3 + 0] * s;
	//}

	//for (size_t i = 0; i < LoadedData->GetVertexCount(); i++) {
	//	Vertices[i * 3 + 0] = Vertices[i * 3 + 0] + translate.X;
	//	Vertices[i * 3 + 1] = Vertices[i * 3 + 1] + translate.Y;
	//	Vertices[i * 3 + 2] = Vertices[i * 3 + 2] + translate.Z;
	//}
	//delete[] copy;
}

VisibleObject::VisibleObject() : BaseObject()
{
	Location = Vector(0, 0, 0);
	Rotation = Vector(0, 0, 0);
	Scale = Vector(1, 1, 1);
	Model = nullptr;
}

void VisibleObject::SetLocation(Vector NewLocation)
{
	Location = NewLocation;
	if (Model != nullptr) Model->Transform(Location, Rotation, Scale);
}

void VisibleObject::SetRotation(Vector NewRotation)
{
	Rotation = NewRotation;
	if (Model != nullptr) Model->Transform(Location, Rotation, Scale);
}

void VisibleObject::SetScale(Vector NewScale)
{
	Scale = NewScale;
	if (Model != nullptr) Model->Transform(Location, Rotation, Scale);
}

void VisibleObject::SetModel(std::string Name)
{
	if (Model != nullptr) {
		delete Model;
	}
	Model = GC::LoadMesh(Name);
	if (Model != nullptr) Model->Transform(Location, Rotation, Scale);
}
