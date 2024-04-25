#include <iostream>
#include <chrono>
#include "GameLoop.h"
#include "Objects/VisibleObject.h"
#include "Gameplay/GameState.h"
#include "Gameplay/PlayerController.h"

#define NANORT_USE_CPP11_FEATURE
#include "nanosg.h"
#include "Settings.h"
#include "Renderer.h"

HANDLE Renderer::hOut = GetStdHandle(STD_OUTPUT_HANDLE);
COORD Renderer::WinSize = COORD();
std::condition_variable Renderer::WaitRender;
std::list<VisibleObject*> Renderer::RenderObjects;
std::list<VisibleObject*> Renderer::InvalidObjects;
std::queue<int> Renderer::RenderQueue;

// char SHADES[10] = " .-~=+<#%@"; // alternate character only palette
unsigned char SHADES[10] = { 255, 255, 176, 176, 177, 177, 178, 178, 219, 219 }; // 

Renderer::Renderer()
{
	bQuitting = false;
	BufferSize = 0;
	RenderBuffer = nullptr;
	UIBuffer = nullptr;
	FinalBuffer = nullptr;
	RenderTime = 0.f;
	RenderThread = std::thread(&Renderer::Run, this);
	ShowFPS = INI->GetValue("Engine", "ShowFps") == "true";
	max_bounces = 1;
}

Renderer::~Renderer()
{
	delete[] RenderBuffer;
	delete[] UIBuffer;
	delete[] FinalBuffer;
	while (!RenderQueue.empty()) RenderQueue.pop();
	RenderObjects.clear();
}

int Renderer::Run()
{
	BufferSize = WinSize.Y * WinSize.X;
	RenderBuffer = new Pixel[BufferSize]();
	UIBuffer = new Pixel[BufferSize]();
	FinalBuffer = new Pixel[BufferSize]();
	std::chrono::duration<float> duration = std::chrono::milliseconds(0);
	auto begin = std::chrono::steady_clock::now();
	auto time = std::chrono::milliseconds(10);
	while (!bQuitting) {
		auto start = std::chrono::steady_clock::now();
		if (BufferSize == 0) continue;
		bool RequiresCommit = false;
		std::unique_lock<std::mutex> lock(list_mutex);
		std::vector<nanosg::Node<float, Mesh>>* nodes = scene.GetNodes();
		for (VisibleObject* obj : InvalidObjects) {
			RenderObjects.remove(obj);
			for (int i = 0; i < nodes->size(); i++) {
				if ((*nodes)[i].GetName() == obj->Model->NodeName) { nodes->erase(nodes->begin() + i); obj->Model->AddedToScene = false; RequiresCommit = true; break; }
			}
		}
		InvalidObjects.clear();
		lock.unlock(); 
		{
			std::unique_lock<std::mutex> buf_lock(buffer_mutex);
			DrawScreen(RequiresCommit);
			DrawUI();
			
			for (int y = 0; y < WinSize.Y; y++) {
				for (int x = 0; x < WinSize.X; x++) {
					FinalBuffer[y * WinSize.X + x] = UIBuffer[y * WinSize.X + x].shade == 0 ? RenderBuffer[y * WinSize.X + x] : UIBuffer[y * WinSize.X + x];
				}
			}
		}
		
		{
			std::unique_lock<std::mutex> buf_lock(buffer_mutex);
			//std::cout.write((char*)FinalBuffer, BufferSize * sizeof(Pixel));
			((char*)FinalBuffer)[BufferSize * sizeof(Pixel)] = 0;
			std::cout << (char*)FinalBuffer;
		}
		duration = std::chrono::steady_clock::now() - start;
		RenderTime = 1.f / duration.count();
	}
	return 0;
}

int Renderer::Quit()
{
	bQuitting = true;
	Renderer::MarkRenderDirty();
	RenderThread.join();
	return 0;
}

void Renderer::SetWinSize(COORD Size)
{
	WinSize = Size;
	std::unique_lock<std::mutex> buf_lock(buffer_mutex);
	delete[] RenderBuffer;
	delete[] UIBuffer;
	delete[] FinalBuffer;
	BufferSize = WinSize.Y * WinSize.X;
	RenderBuffer = new Pixel[BufferSize]();
	UIBuffer = new Pixel[BufferSize]();
	FinalBuffer = new Pixel[BufferSize]();
	Renderer::MarkRenderDirty();
}

void Renderer::MarkRenderDirty(short Flag)
{
	RenderQueue.push(Flag);
	WaitRender.notify_all();
}

void Renderer::ShowCrazy(bool show)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD prev_mode;
	GetConsoleMode(hOut, &prev_mode);
	if (show)
		SetConsoleMode(hOut, prev_mode & ~(ENABLE_VIRTUAL_TERMINAL_PROCESSING));
	else 
		SetConsoleMode(hOut, prev_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

void Renderer::DrawScreen(bool commit)
{
	GameState* game = GameLoop::State;
	SetConsoleCursorPosition(hOut, COORD());
	if (game == nullptr || game->CurrentPlayer == nullptr) return;
	const unsigned char wall = 219;
	const unsigned char wall2 = 178;
	const unsigned char wall3 = 177;
	const unsigned char wall4 = 176;
	const unsigned char shade = 111;
	const unsigned char empty = 255;

	if (WinSize.X * WinSize.Y != BufferSize) return;

	memset(RenderBuffer, empty, BufferSize * sizeof(Pixel));
	Vector up = game->CurrentPlayer->UpVector();
	Vector right = game->CurrentPlayer->RightVector();

	bool Commit = commit;

	for (VisibleObject* obj : RenderObjects) {
		bool found = false;
		for (VisibleObject* old : InvalidObjects) {
			if (old == obj) {
				found = true; break;
			}
		}
		if (found) continue;
		Mesh* mesh = obj->Model;
		if (mesh == nullptr || !mesh->ModelLoaded || obj->bMarked) continue;
		if (!mesh->AddedToScene) {
			nanosg::Node<float, Mesh> node(mesh);
			node.SetLocalXform(mesh->transform);
			node.SetName(mesh->NodeName);
			scene.AddNode(node);
			Commit = true;
			mesh->AddedToScene = true;
		}
		if (mesh->RequiresBuild) {
			nanosg::Node<float, Mesh>* node;
			scene.FindNode(mesh->NodeName, &node);
			node->SetLocalXform(mesh->transform);
			Commit = true;
			mesh->RequiresBuild = false;
		}
	}
	if (scene.GetNodes()->size() < 1) return;
	if (Commit) scene.Commit();

	nanort::Ray<float> s_ray;
	s_ray.min_t = 10.f;
	s_ray.max_t = 10000.f;
	nanort::Ray<float> ray;
	ray.min_t = 1.f;
	ray.max_t = 100000000.f;
	Vector point = game->CurrentPlayer->CameraPoint;
	ray.org[0] = game->CurrentPlayer->CameraPoint.X;
	ray.org[1] = game->CurrentPlayer->CameraPoint.Y;
	ray.org[2] = game->CurrentPlayer->CameraPoint.Z;
	Vector sun = game->Sun * -1;

	#pragma omp parallel for
	for (int y = 0; y < WinSize.Y; y++) {
		for (int x = 0; x < WinSize.X; x++) {
			Vector upDir = up * ((float)(WinSize.Y - y) / WinSize.Y - 0.5f);
			Vector rightDir = right * ((float)x / WinSize.X - 0.5f);
			Vector dir = (game->CurrentPlayer->CameraDirection + upDir + rightDir).Normalize();
			
			ray.dir[0] = dir.X;
			ray.dir[1] = dir.Y;
			ray.dir[2] = dir.Z;

			char shade = 0;
			short color;
			short bounces = 1;
			DoBounce(ray, bounces, shade, color);
			RenderBuffer[y * WinSize.X + x].shade = SHADES[shade];
			memcpy(RenderBuffer[y * WinSize.X + x].color, getColor(color), 5);
		}
	}
	
}

void write_text(Pixel* start, const char* text)
{
	for (int i = 0; i < strlen(text); i++) {
		start[i].shade = text[i];
		SetColor(start[i], WHT);
	}
}

void Renderer::DrawUI()
{
	memset(UIBuffer, 0, BufferSize * sizeof(Pixel));
	if (GameLoop::State == nullptr || GameLoop::State->CurrentPlayer == nullptr || GameLoop::State->CurrentPlayer->UI == nullptr) return;
	GameLoop::State->CurrentPlayer->UI->DrawUI(WinSize.X, WinSize.Y, UIBuffer);

	if (ShowFPS) {
		char buffer[13];
		char timebuffer[11];
		snprintf(timebuffer, sizeof timebuffer, "fps: %f", RenderTime);
		write_text(UIBuffer, timebuffer);
	}
}

unsigned char Renderer::DoBounce(nanort::Ray<float>& ray, short& bounce, char& shade, short& color)
{
	nanosg::Intersection<float> isect;

	color = 6;
	shade = 9;

	if (scene.Traverse<nanosg::Intersection<float>, nanort::TriangleIntersector<float, nanosg::Intersection<float>>>(ray, &isect, true)) {
		auto mesh = (*scene.GetNodes())[isect.node_id].GetMesh();
		color = mesh->mat;
		shade = 9;

		GameState* game = GameLoop::State;

		Vector nor = Vector(isect.Ns[0], isect.Ns[1], isect.Ns[2]);
		float dot = Vector::Dot(nor, game->Sun);
		Vector sun = game->Sun * -1;

		if (dot < 0.f) {
			nanort::Ray<float> s_ray;
			s_ray.min_t = 10.f;
			s_ray.max_t = 10000.f;

			s_ray.min_t = 10.f;
			s_ray.org[0] = isect.t * ray.dir[0] + ray.org[0];
			s_ray.org[1] = isect.t * ray.dir[1] + ray.org[1];
			s_ray.org[2] = isect.t * ray.dir[2] + ray.org[2];
			s_ray.dir[0] = sun.X;
			s_ray.dir[1] = sun.Y;
			s_ray.dir[2] = sun.Z;
			if (scene.Traverse<nanosg::Intersection<float>, nanort::TriangleIntersector<float, nanosg::Intersection<float>>>(s_ray, &isect, true)) {
				shade = 2;
			}
			else {
				shade = 9;
			}
		}
		else
			shade = 2;


		if (bounce <= max_bounces)
		{
			//Vector dir(direction - 2 * (Vector(isect.Ns[0], isect.Ns[1], isect.Ns[2])));
			//auto mult = 2 * (direction.X * isect.Ns[0] + direction.Y * isect.Ns[1] + direction.Z * isect.Ns[2]);
			//Vector dir(direction.X - mult * isect.Ns[0], direction.Y - mult * isect.Ns[1], direction.Z - mult * isect.Ns[2]);
			//auto nehit = DoBounce(Vector(isect.P[0], isect.P[1], isect.P[2]), dir, ++bounce, shade, color);
			//shade -= 9 - hit;
		}
		return shade;
	}
	
	else {
		return shade;
	}
}

const char* getColor(short idx) 
{
	switch (idx)
	{
	case 0: return WHT;
	case 1: return RED;
	case 2: return GRN;
	case 3: return YEL;
	case 4: return BLU;
	case 5: return MAG;
	case 6: return CYN;
	default:
		return WHT;
	}
}

void SetColor(Pixel& p, const char* color)
{
	memcpy(p.color, color, 5);
}
