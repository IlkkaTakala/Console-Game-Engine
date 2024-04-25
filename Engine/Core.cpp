#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOCOMM
#include <Windows.h>
#include <iostream>
#include "Renderer.h"
#include "GameLoop.h"
#include "Core.h"

GameLoop* Loop = nullptr;
INISettings* INI = nullptr;

void Print(const char* text)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOut, COORD());
	std::cout << text;
}

void Exit()
{
	if (!Loop->bQuitStarted) Loop->Quit();
}

void MarkRenderUpdate()
{
	Renderer::MarkRenderDirty();
}

bool Trace(const Vector Start, const Vector End, Vector& Hit, VisibleObject** OutTarget)
{
	return Loop->DoTrace(Start, End, Hit, OutTarget);
}

