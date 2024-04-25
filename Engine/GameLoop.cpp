#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOCOMM
#include <Windows.h>
#include "Objects/Actor.h"
#include "Renderer.h"
#include "Gameplay/GameState.h"
#include "Gameplay/InputControl.h"
#include "../Game/EngineInterface.h"
#include "Settings.h"
#include "GameLoop.h"

using namespace std;

std::list<Tickable*> GameLoop::TickList;
std::list<Tickable*> GameLoop::TickListRemoval;
Ref<GameState> GameLoop::State = nullptr;

GameLoop::GameLoop()
{
	bQuit = false;
	bQuitStarted = false;
	Render = nullptr;
	InputHandler = nullptr;
	Collector = nullptr;
	fps = 0.f;
}

GameLoop::~GameLoop()
{
	if (State != nullptr) State == nullptr;
	if (InputHandler != nullptr) delete InputHandler;
	if (Render != nullptr) delete Render;
	if (Collector != nullptr) delete Collector;
	delete INI;
}

int GameLoop::Start()
{
	Loop = this;
	INI = new INISettings("Settings.ini");
	if (!INI->IsValid()) return 10;
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_FONT_INFOEX fontInfo;
	fontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(hOut, false, &fontInfo);
	COORD font = { std::stoi(INI->GetValue("Engine", "FontX")), std::stoi(INI->GetValue("Engine", "FontY")) };
	fontInfo.dwFontSize = font;
	fontInfo.FontFamily = FF_ROMAN;
	fontInfo.FontWeight = FW_BOLD;
	wcscpy_s(fontInfo.FaceName, L"Terminal");//SimSun-ExtB
	SetCurrentConsoleFontEx(hOut, false, &fontInfo);

	CONSOLE_SCREEN_BUFFER_INFOEX SBInfo;

	SBInfo.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	GetConsoleScreenBufferInfoEx(hOut, &SBInfo);
	Vector2D res = Vector2D(std::stoi(INI->GetValue("Engine", "ResolutionX")), std::stoi(INI->GetValue("Engine", "ResolutionY")));
	SBInfo.dwSize.X = res.X;
	SBInfo.dwSize.Y = res.Y;
	SBInfo.srWindow.Bottom = res.Y;
	SBInfo.srWindow.Right = res.X - 1;
	SBInfo.srWindow.Top = 0;
	SBInfo.srWindow.Left = 0;

	SetConsoleScreenBufferInfoEx(hOut, &SBInfo);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(hOut, &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(hOut, &cursorInfo);

	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD prev_mode;
	GetConsoleMode(hInput, &prev_mode);
	SetConsoleMode(hInput, ENABLE_EXTENDED_FLAGS |
		(prev_mode & ~(ENABLE_QUICK_EDIT_MODE | ENABLE_VIRTUAL_TERMINAL_INPUT | ENABLE_PROCESSED_INPUT)) | ENABLE_MOUSE_INPUT);
	GetConsoleMode(hOut, &prev_mode);
	SetConsoleMode(hOut, prev_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

	StartRenderer();
	GetConsoleScreenBufferInfoEx(hOut, &SBInfo);
	Sleep(100);
	Render->SetWinSize(SBInfo.dwSize);
	InputHandler = new Input();
	TickList.push_back(InputHandler);
	Collector = new GC(INI->GetValue("Engine", "DataFolder"));
	State = EngineInterface::CreateDefaults();

	return MainLoop();
}

void GameLoop::Quit()
{
	bQuitStarted = true;
	TickListRemoval.push_back(InputHandler);
	Collector->Quit();
	InputHandler->Quit();
	Render->Quit();
	bQuit = true;
}

void GameLoop::StartRenderer()
{
	Render = new Renderer();
	Render->max_bounces = std::stoi(INI->GetValue("Engine", "Bounces"));
	return;
}

int GameLoop::MainLoop()
{
	std::chrono::duration<float> duration = std::chrono::milliseconds(0);
	auto begin = std::chrono::steady_clock::now();
	auto time = std::chrono::milliseconds(10);
	while (!bQuit) {
		auto start = std::chrono::steady_clock::now();

		std::this_thread::sleep_until(start + time);

		InputHandler->AnalyseMouse();

		for (Tickable* t : TickList) {
			bool found = false;
			for (Tickable* old : TickListRemoval) {
				if (old == t) {
					found = true; break;
				}
			}
			if (found) continue;
			t->Tick(duration.count());
		}
		std::unique_lock<std::mutex> lock(TickListMutex);
		for (Tickable* t : TickListRemoval) {
			TickList.remove(t);
		}
		lock.unlock();
		duration = std::chrono::steady_clock::now() - start;
		fps = 1.f / duration.count();
	}
	
	return 0;
}

bool GameLoop::DoTrace(const Vector Start, const Vector End, Vector& Hit, VisibleObject** OutTarget)
{
	Vector dir = (End - Start).Normalize();
	Vector normal = End - Start;
	Vector t_v = normal / dir;
	nanort::Ray<float> ray;
	ray.min_t = 1.f;
	ray.max_t = (t_v.X + t_v.Y + t_v.Z) / 3;
	ray.org[0] = Start.X;
	ray.org[1] = Start.Y;
	ray.org[2] = Start.Z;
	ray.dir[0] = dir.X;
	ray.dir[1] = dir.Y;
	ray.dir[2] = dir.Z;
	nanosg::Intersection<float> isect;
	std::unique_lock<std::mutex> lock(Render->list_mutex);
	if (Render->scene.Traverse<nanosg::Intersection<float>, nanort::TriangleIntersector<float, nanosg::Intersection<float>>>(ray, &isect, true)) {
		Hit.X = isect.P[0];
		Hit.Y = isect.P[1];
		Hit.Z = isect.P[2];
		if (OutTarget != nullptr) {
			std::string name = (*Render->scene.GetNodes())[isect.node_id].GetName();
			for (VisibleObject* obj : Render->RenderObjects) {
				if (obj->GetName() == name) { *OutTarget = obj; break; }
			}
		}
		return true;
	}
	return false;
}


