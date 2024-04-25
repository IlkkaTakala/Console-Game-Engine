#pragma once
#include "Core.h"
#include <mutex>
#include <vector>
#include <list>

class Renderer;
class GameState;
class GC;
class Input;

class GameLoop
{
	friend class Renderer;
	friend class GC;
public:
	GameLoop();
	~GameLoop();
	int Start();
	void Quit();
	void AddToTick(Tickable* t) { std::unique_lock<std::mutex> lock(TickListMutex); TickList.push_back(t); }
	void RemoveFromTick(Tickable* t) { std::unique_lock<std::mutex> lock(TickListMutex); TickListRemoval.push_back(t); }
	bool bQuitStarted;
	bool DoTrace(const Vector Start, const Vector End, Vector& Hit, VisibleObject** OutTarget);

private:
	void StartRenderer();
	int MainLoop();

	static std::list<Tickable*> TickList;
	static std::list<Tickable*> TickListRemoval;

	Renderer* Render;
	Input* InputHandler;
	GC* Collector;
	std::mutex TickListMutex;
	static Ref<GameState> State;
	bool bQuit;
	float fps;
};