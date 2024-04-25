#pragma once
#include "VisibleObject.h"

class Actor : public VisibleObject, public Tickable
{
public:
	Actor() {}
	virtual ~Actor() {};
	//void SetModel(const char* NewModel) { Model = NewModel; }
protected:
	virtual void Tick(float Delta) override;
	virtual void BeginPlay() override {};
};