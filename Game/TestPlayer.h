#pragma once
#include "Core.h"
#include "Settings.h"
#include "Gameplay/PlayerController.h"


class TestPlayer : public Player
{
public:
	TestPlayer();
	virtual ~TestPlayer() { INI->SetValue("Player", "Start", CameraPoint.String()); INI->SetValue("Player", "Direction", CameraDirection.String()); }
	virtual void Tick(float Delta) override;
	virtual void BeginPlay() override;
private:
	short forward;
	short right;
	short up;

	void RunInputW(bool KeyDown);
	void RunInputA(bool KeyDown);
	void RunInputD(bool KeyDown);
	void RunInputS(bool KeyDown);
	void RunInputSpace(bool KeyDown);
	void RunInputShift(bool KeyDown);;
	void BreakIt(bool KeyDown);
	void LeftMouseDown(bool KeyDown);
	void MouseMoved(float X, float Y);

	void InputExit() { Exit(); }

	Ref<VisibleObject> Roads;
	Ref<VisibleObject> Buildings;
	Ref<VisibleObject> Domain;
	std::vector<Ref<VisibleObject>> cubes;
};