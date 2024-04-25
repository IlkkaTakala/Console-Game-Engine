#pragma once
#include "Core.h"

class UIHolder;

class Player : public BaseObject, public Tickable
{
public:
	Player();
	virtual void Tick(float Delta) override;
	virtual void BeginPlay() override {};

	template <class T>
	void SetUI() {
		auto n = new T();
		auto y = dynamic_cast<UIHolder*>(n);
		if (y != nullptr) {
			UI = y;
			GC::Pointers.push_back(y);
			y->Construct();
		}
	}


protected:
	friend class Renderer;
	Vector CameraPoint;
	Vector CameraDirection;
	Vector CameraRotation;


	void RotateCamera(Vector Rotation);
	Vector RightVector();
	Vector UpVector();

	Ref<UIHolder> UI;
};

