#pragma once
#include "BasicTypes.h"
#include "Objects/BaseObject.h"
#include "GarbageCollector.h"
#include "UI/UIHolder.h"

#define REN_UI 0x20
#define REN_REQUIRESBUILD 0x21

class VisibleObject;
class GameLoop;
class INISettings;
extern GameLoop* Loop;
extern INISettings* INI;

void Print(const char* text);

void Exit();

template <class T = BaseObject>
Ref<T> SpawnObject()
{
	T* next = new T();
	BaseObject* base = dynamic_cast<BaseObject*>(next);
	if (base == nullptr) {
		delete next;
		return nullptr;
	}
	GC::AddObject(base);
	base->BeginPlay();
	return Ref<T>(next);
}

void MarkRenderUpdate();

bool Trace(const Vector Start, const Vector End, Vector& Hit, VisibleObject** OutTarget = nullptr);