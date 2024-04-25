#pragma once
#include "Core.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOCOMM
#include <Windows.h>
#include <thread>
#include <condition_variable>
#include <queue>
#define NANORT_USE_CPP11_FEATURE
#include "nanosg.h"

class VisibleObject;

struct Pixel
{
	char color[5];
	char shade;
};

constexpr const char* RED = "\x1B[31m";
constexpr const char* GRN = "\x1B[32m";
constexpr const char* YEL = "\x1B[33m";
constexpr const char* BLU = "\x1B[34m";
constexpr const char* MAG = "\x1B[35m";
constexpr const char* CYN = "\x1B[36m";
constexpr const char* WHT = "\x1B[37m";

const char* getColor(short idx);

void SetColor(Pixel& p, const char* color);

class Renderer
{
public:
	Renderer();
	~Renderer();
	int Run();
	int Quit();
	void SetWinSize(COORD Size);
	static void MarkRenderDirty(short Flag = REN_REQUIRESBUILD);
	static void AddRenderQueue() { RenderQueue.push(1); WaitRender.notify_all(); }
	static std::list<VisibleObject*> RenderObjects;
	static std::list<VisibleObject*> InvalidObjects;
	static COORD WinSize;

	static void ShowCrazy(bool show);

private:
	friend class GC;
	friend class GameLoop;
	void DrawScreen(bool commit);
	void DrawUI();
	unsigned char DoBounce(nanort::Ray<float>& ray, short& bounce, char& shade, short& color);
	static HANDLE hOut;
	std::thread RenderThread;
	static std::condition_variable WaitRender;
	std::mutex RenderMutex;
	std::mutex list_mutex;
	std::mutex buffer_mutex;
	bool bQuitting;
	static std::queue<int> RenderQueue;
	Pixel* RenderBuffer;
	Pixel* UIBuffer;
	Pixel* FinalBuffer;
	int BufferSize;
	bool ShowFPS;
	nanosg::Scene<float, Mesh> scene;
	float RenderTime;
	short max_bounces;
};
