#pragma once
#include "Core.h"
#include <map>
#include <list>
#include <queue>
#include <thread>
#include <functional>

class Input : public Tickable
{
public:
	Input();
	~Input();
	void Quit();

	template <class UserClass>
	static void RegisterKeyInput(unsigned short Key, void (UserClass::* Callback)(bool), UserClass* Caller) 
	{
		using std::placeholders::_1;
		std::function<void(bool)> f = std::bind(Callback, Caller, _1);
		KeyCallers.insert(std::pair<unsigned short, std::function<void(bool)>>(Key, f));
	}

	template <class UserClass>
	static void RegisterMouseInput(unsigned long Key, void (UserClass::* Callback)(float, float), UserClass* Caller)
	{
		using std::placeholders::_1;
		using std::placeholders::_2;
		std::function<void(float, float)> f = std::bind(Callback, Caller, _1, _2);
		MouseCallers.insert(std::pair<unsigned long, std::function<void(float, float)>>(Key, f));
	}

	template <class UserClass>
	static void RegisterMainLoopInput(unsigned short Key, void (UserClass::* Callback)(), UserClass* Caller)
	{
		std::function<void()> f = std::bind(Callback, Caller);
		MainLoopCallers.insert(std::pair<unsigned short, std::function<void()>>(Key, f));
	}
	void AnalyseMouse();

private:
	static std::map<unsigned short, std::function<void(bool)>> KeyCallers;
	static std::map<unsigned long, std::function<void(float, float)>> MouseCallers;
	static std::map<unsigned short, std::function<void()>> MainLoopCallers;
	std::map<unsigned short, bool> KeyStates;
	std::thread main_thread;
	std::queue<unsigned short> InvokeList;
	int last_mouse_x;
	int last_mouse_y;
	bool bQuitting;
	bool captureMouse;
	void Run();
	virtual void Tick(float Delta) override;
};