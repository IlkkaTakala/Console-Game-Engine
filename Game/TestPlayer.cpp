#include "GamePlay/InputControl.h"
#include "TestPlayer.h"
#include "Renderer.h"

TestPlayer::TestPlayer()
{
	forward = 0;
	right = 0;
	up = 0;
	CameraPoint = INI->GetValue("Player", "Start");
	CameraDirection = INI->GetValue("Player", "Direction");

	Input::RegisterKeyInput(0x57, &TestPlayer::RunInputW, this);
	Input::RegisterKeyInput(0x41, &TestPlayer::RunInputA, this);
	Input::RegisterKeyInput(0x53, &TestPlayer::RunInputS, this);
	Input::RegisterKeyInput(0x44, &TestPlayer::RunInputD, this);
	Input::RegisterKeyInput(0x20, &TestPlayer::RunInputSpace, this);
	Input::RegisterKeyInput(0x10, &TestPlayer::RunInputShift, this);
	Input::RegisterKeyInput(0x01, &TestPlayer::LeftMouseDown, this);
	Input::RegisterKeyInput(0x54, &TestPlayer::BreakIt, this);
	Input::RegisterMainLoopInput(0x1B, &TestPlayer::InputExit, this);
	Input::RegisterMouseInput(0x0001, &TestPlayer::MouseMoved, this);
}

void TestPlayer::RunInputW(bool KeyDown)
{
	if (KeyDown)
		forward = 1;
	else if (forward == 1) forward = 0;
}

void TestPlayer::RunInputA(bool KeyDown)
{
	if (KeyDown)
		right = -1;
	else if (right == -1) right = 0;
}

void TestPlayer::RunInputD(bool KeyDown)
{
	if (KeyDown)
		right = 1;
	else if (right == 1) right = 0;
}

void TestPlayer::RunInputS(bool KeyDown)
{
	if (KeyDown)
		forward = -1;
	else if (forward == -1) forward = 0;
}

void TestPlayer::RunInputSpace(bool KeyDown)
{
	if (KeyDown)
		up = 1;
	else if (up == 1) up = 0;
}

void TestPlayer::RunInputShift(bool KeyDown)
{
	if (KeyDown)
		up = -1;
	else if (up == -1) up = 0;
}

void TestPlayer::BreakIt(bool KeyDown)
{
	Renderer::ShowCrazy(KeyDown);
}

#include "Objects/VisibleObject.h"

void TestPlayer::LeftMouseDown(bool)
{
	/*Vector Start(CameraPoint);
	Vector End(CameraPoint + CameraDirection * 100);
	Vector Out;
	VisibleObject* Hit = nullptr;
	Trace(Start, End, Out, &Hit);
	if (Hit != nullptr) Hit->DestroyObject();*/
}

void TestPlayer::MouseMoved(float X, float Y)
{
	RotateCamera(Vector(0, Y / 3.f, -X / 3.f) + CameraRotation);
}

void TestPlayer::Tick(float)
{
	float Sens = 15.f;
	Vector out;
	switch (forward)
	{
	case 1:
		//if (!Trace(CameraPoint, CameraPoint + CameraDirection, out))
		CameraPoint = CameraPoint + CameraDirection * Sens;
		break;
	case -1:
		CameraPoint = CameraPoint + CameraDirection * -Sens;
		break;
	}
	switch (right)
	{
	case 1:
		CameraPoint = CameraPoint + RightVector() * Sens;
		break;
	case -1:
		CameraPoint = CameraPoint + RightVector() * -Sens;
		break;
	}
	switch (up)
	{
	case 1:
		CameraPoint = CameraPoint + UpVector() * Sens;
		break;
	case -1:
		CameraPoint = CameraPoint + UpVector() * -Sens;
		break;
	}
}

void TestPlayer::BeginPlay()
{
	/*Roads = SpawnObject<VisibleObject>();
	Roads->SetModel("Roads");
	Roads->SetScale(Vector(1, 1, 1));*/
	Buildings = SpawnObject<VisibleObject>();
	Buildings->SetModel("Buildings");
	Buildings->SetMaterial(0);
	Buildings->SetScale(Vector(1, 1, 1));
	Domain = SpawnObject<VisibleObject>();
	Domain->SetModel("Domain");
	Domain->SetMaterial(2);
	Domain->SetScale(Vector(100, 100, 100));

	for (int i = 0; i < 10; i++) {
		auto cube = SpawnObject<VisibleObject>();
		cube->SetModel("Cube");
		cube->SetMaterial(4);
		cube->SetScale(Vector(100, 100, 100));
		cube->SetLocation(Vector(i * 700.f, 0, 1610.f));
		cubes.push_back(cube);
	}

	auto cube = SpawnObject<VisibleObject>();
	cube->SetModel("Cube");
	cube->SetMaterial(3);
	cube->SetScale(Vector(400, 400, 400));
	cube->SetLocation(Vector(7000.f, 0, 6100.f));
	cubes.push_back(cube);
}
