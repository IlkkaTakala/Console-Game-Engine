#include "GameLoop.h"
#include "Renderer.h"
#include "Objects/VisibleObject.h"
#include <fstream>
#include <sstream>
#include <iterator>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include "GarbageCollector.h"

std::list<Data*> GC::Pointers;
std::map<std::string, MeshData*> GC::LoadedMeshes;

GC::GC(std::string dir)
{
	namespace fs = std::experimental::filesystem;
	ActiveDir = dir;
	for (const auto& f : fs::recursive_directory_iterator(dir)) {
		if (f.path().extension() == ".obj") {
			std::ifstream* stre = new std::ifstream(f.path(), std::ios_base::in);
			ModelStreams[f.path().filename().replace_extension("").string()] = stre;
		}
	}
	bQuitting = false;
	Cleaner = std::thread(&GC::CleanRunner, this);
}

GC::~GC()
{
	while (Pointers.size() > 0) {
		delete Pointers.front();
		Pointers.pop_front();
	}
	for (auto i : ModelStreams) {
		i.second->close();
		delete i.second;
	}
	for (auto i : LoadedMeshes) {
		delete i.second;
	}
	LoadedMeshes.clear();
	Pointers.clear();
	ModelStreams.clear();
}

void GC::AddObject(BaseObject* obj)
{
	Pointers.push_back(obj);
	Tickable* t = dynamic_cast<Tickable*>(obj);
	if (t != nullptr) Loop->AddToTick(t);
	VisibleObject* v = dynamic_cast<VisibleObject*>(obj);
	if (v != nullptr) {
		std::unique_lock<std::mutex> lock(Loop->Render->list_mutex);
		Renderer::RenderObjects.push_back(v);
	}
}

void GC::RemoveObject(BaseObject* obj)
{
	Tickable* t = dynamic_cast<Tickable*>(obj);
	if (t != nullptr) Loop->RemoveFromTick(t);
	VisibleObject* v = dynamic_cast<VisibleObject*>(obj);
	if (v != nullptr) {
		std::unique_lock<std::mutex> lock(Loop->Render->list_mutex);
		Renderer::InvalidObjects.push_back(v);
	}
	obj->bMarked = true;
}

Mesh* GC::LoadMesh(std::string Name)
{
	Mesh* mesh = new Mesh();
	auto iter = LoadedMeshes.find(Name);
	if (iter != LoadedMeshes.end()) {
		mesh->LoadedData = iter->second;
		iter->second->ReferenceCount++;
		mesh->ModelLoaded = true;
		mesh->Vertices = new float[mesh->LoadedData->VertexCount * 3]();
		memcpy(mesh->Vertices, mesh->LoadedData->vertices, mesh->LoadedData->VertexCount * 3 * sizeof(float));
	}
	else {
		MeshData* newData = new MeshData();
		mesh->ModelLoaded = Loop->Collector->LoadObj(Name, newData);
		if (mesh->ModelLoaded) {
			newData->ReferenceCount = 1;
			mesh->LoadedData = newData;
			LoadedMeshes.insert(std::pair<std::string, MeshData*>(Name, newData));
			mesh->Vertices = new float[mesh->LoadedData->VertexCount * 3]();
			memcpy(mesh->Vertices, mesh->LoadedData->vertices, mesh->LoadedData->VertexCount * 3 * sizeof(float));
		}
		else {
			delete newData;
			mesh->LoadedData = nullptr;
		}
	}
	std::string na = std::to_string(rand());
	mesh->NodeName = na;
	return mesh;
}

template <typename Out>
void split(const std::string& s, char delim, Out result) {
	std::istringstream iss(s);
	std::string item;
	while (std::getline(iss, item, delim)) {
		*result++ = item;
	}
}

std::vector<std::string> split(const std::string& s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

bool GC::LoadObj(std::string Name, MeshData* Data)
{
	std::ifstream* s = ModelStreams[Name];
	if (!s) return false;
	s->seekg(0);
	std::string in;
	size_t v_index = 0;
	size_t n_index = 0;
	std::vector<float> verts;
	std::vector<float> normals;
	std::vector<unsigned int> normalIndices;
	std::vector<unsigned int> faces;
	while (std::getline(*s, in)) {
		switch (in.front())
		{
		case 'v':
		{
			std::vector<std::string> vec = split(in, ' ');
			if (vec[0] == "v") {
				verts.push_back(std::stof(vec[1]));
				verts.push_back(std::stof(vec[2]));
				verts.push_back(std::stof(vec[3]));
				v_index++;
			}
			else if (vec[0] == "vn") {
				normals.push_back(std::stof(vec[1]));
				normals.push_back(std::stof(vec[2]));
				normals.push_back(std::stof(vec[3]));
				n_index++;
			}
		}
		break;

		case 'f':
		{
			std::vector<std::string> face = split(in, ' ');
			face.erase(face.begin(), face.begin() + 1);
			for (std::string str : face) {
				std::vector<std::string> data = split(str, '/');
				faces.push_back(std::stoi(data[0]) - 1);
				normalIndices.push_back(std::stoi(data[2]) - 1);
			}
		}
		break;

		default:
			break;
		}
	}
	Data->Name = Name;
	Data->FaceCount = (unsigned int)faces.size() / 3;
	Data->VertexCount = (unsigned int)v_index;
	Data->NormalCount = (unsigned int)n_index;
	Data->vertices = new float[v_index * 3]();
	Data->normals = new float[n_index * 3]();
	Data->faces = new unsigned int[faces.size()]();
	Data->NormalIndices = new unsigned int[faces.size()]();
	memcpy(Data->vertices, verts.data(), v_index * 3 * sizeof(float));
	memcpy(Data->normals, normals.data(), n_index * 3 * sizeof(float));
	memcpy(Data->faces, faces.data(), faces.size() * sizeof(unsigned int));
	memcpy(Data->NormalIndices, normalIndices.data(), faces.size() * sizeof(unsigned int));

	return true;
}

void GC::Quit()
{
	bQuitting = true;
	Cleaner.join();
}

void GC::CleanRunner()
{
	std::chrono::duration<float> duration = std::chrono::milliseconds(0);
	while (!bQuitting) {
		auto start = std::chrono::steady_clock::now();
		auto time = std::chrono::milliseconds(5000) - duration;
		if (time.count() > 0) std::this_thread::sleep_for(time);

		std::vector<Data*> removal;
		for (Data* d : Pointers) {
			if (d->ReferenceCount < 1) {
				if (d->bMarked) {
					VisibleObject* v = dynamic_cast<VisibleObject*>(d);
					if (v != nullptr) {
						if (v->Model == nullptr || v->Model->AddedToScene == false) {
							delete d;
							removal.push_back(d);
						}
					}
					else {
						delete d;
						removal.push_back(d);
					}
					
				}
				else {
					d->DestroyObject();
				}
			}
			else {
				d->bMarked = false;
			}
		}
		for (Data* d : removal) {
			Pointers.remove(d);
		}

		std::vector<std::string> removed;
		for (auto d : LoadedMeshes) {
			if (d.second->ReferenceCount < 1) {
				delete d.second;
				removed.push_back(d.first);
			}
		}
		for (std::string s : removed) {
			LoadedMeshes.erase(s);
		}

		duration = std::chrono::steady_clock::now() - start;
	}
}

MeshData::MeshData()
{
	Name = "";
	FaceCount = 0;
	NormalCount = 0;
	VertexCount = 0;
	ReferenceCount = 0;
	normals = nullptr;
	vertices = nullptr;
	faces = nullptr;
	NormalIndices = nullptr;
}

MeshData::~MeshData()
{
	delete[] vertices;
	delete[] normals;
	delete[] faces;
	delete[] NormalIndices;
}
