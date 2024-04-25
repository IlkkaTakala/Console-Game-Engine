#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOCOMM
#include <Windows.h>
#include <string>
#include "InputControl.h"
#include "Renderer.h"

std::map<unsigned short, std::function<void(bool)>> Input::KeyCallers;
std::map<unsigned long, std::function<void(float, float)>> Input::MouseCallers;
std::map<unsigned short, std::function<void()>> Input::MainLoopCallers;

Input::Input()
{
	bQuitting = false;
	captureMouse = false;
	POINT p;
	GetCursorPos(&p);
	last_mouse_x = p.x;
	last_mouse_y = p.y;
	main_thread = std::thread(&Input::Run, this);
}

Input::~Input()
{
	KeyCallers.clear();
	MouseCallers.clear();
}

void Input::Quit()
{
	bQuitting = true;
	main_thread.join();
}

void Input::Run()
{
	bQuitting = false;
	DWORD cNumRead, i;
	INPUT_RECORD irInBuf[128];
	HANDLE hOut = GetStdHandle(STD_INPUT_HANDLE);
	std::map<DWORD, WORD> MouseKeys;
	MouseKeys[1] = 0x01;
	MouseKeys[2] = 0x02;
	while (!bQuitting) {
		ReadConsoleInput(hOut, irInBuf, 128, &cNumRead);
		for (i = 0; i < cNumRead; i++)
		{
			switch (irInBuf[i].EventType)
			{
			case KEY_EVENT:
				if (!KeyCallers.empty()) {
					auto t = KeyCallers.find(irInBuf->Event.KeyEvent.wVirtualKeyCode);
					if (t != KeyCallers.end()) {
						auto r = KeyStates.find(irInBuf->Event.KeyEvent.wVirtualKeyCode);
						if (r != KeyStates.end()) {
							if (r->second && !irInBuf->Event.KeyEvent.bKeyDown) {
								t->second(false);
								KeyStates[irInBuf->Event.KeyEvent.wVirtualKeyCode] = false;
							}
							else if (!r->second && irInBuf->Event.KeyEvent.bKeyDown) {
								t->second(true);
								KeyStates[irInBuf->Event.KeyEvent.wVirtualKeyCode] = true;
							}
						}
						else {
							t->second(true);
							KeyStates[irInBuf->Event.KeyEvent.wVirtualKeyCode] = true;
						}
					}
				}
				if (!MainLoopCallers.empty() && irInBuf->Event.KeyEvent.bKeyDown) {
					auto t = MainLoopCallers.find(irInBuf->Event.KeyEvent.wVirtualKeyCode);
					if (t != MainLoopCallers.end()) {
						InvokeList.push(t->first);
					}
				}
				
				break;
			case MOUSE_EVENT:
				
				if (!KeyCallers.empty()) {
					auto t = KeyCallers.find(MouseKeys[irInBuf->Event.MouseEvent.dwButtonState]);
					if (t != KeyCallers.end()) {
						t->second(true);
					}
				}
				if (irInBuf->Event.MouseEvent.dwButtonState == 0x001 && !captureMouse) {
					POINT p;
					GetCursorPos(&p);
					last_mouse_x = p.x;
					last_mouse_y = p.y;
					captureMouse = true;
				}
				else if (irInBuf->Event.MouseEvent.dwButtonState == 0x000) captureMouse = false;
				break;
			default:
				break;
			}
		}
	}
}

void Input::Tick(float)
{
	while (!InvokeList.empty()) {
		MainLoopCallers[InvokeList.front()]();
		InvokeList.pop();
	}
}

void Input::AnalyseMouse()
{
	if (!MouseCallers.empty() && captureMouse) {
		POINT p;
		GetCursorPos(&p);
		auto t = MouseCallers.find(0x0001);
		if (t != MouseCallers.end()) {
			t->second((float)last_mouse_x - (float)p.x, (float)last_mouse_y - (float)p.y);
			last_mouse_x = p.x;
			last_mouse_y = p.y;
		}
	}
}
