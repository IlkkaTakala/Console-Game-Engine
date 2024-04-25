#include "PlayerController.h"

Player::Player()
{
	CameraPoint = Vector(-300, 0, 0);
	CameraDirection = Vector(1, 0, 0);
	CameraRotation = Vector(0, 0, 0);
}

void Player::Tick(float)
{
}

void Player::RotateCamera(Vector Rotation)
{
	CameraRotation = Rotation;
	CameraDirection = Vector(1, 0, 0);
	Vector copy = CameraDirection;

	float angleX = Rotation.X * PI / 180.f;
	float angleY = -Rotation.Y * PI / 180.f;
	float angleZ = Rotation.Z * PI / 180.f;

	float s = (float)sin(angleX);
	float c = (float)cos(angleX);
	copy = CameraDirection;

	CameraDirection.Y = copy.Y * c - copy.Z * s;
	CameraDirection.Z = copy.Y * s + copy.Z * c;

	s = (float)sin(angleY);
	c = (float)cos(angleY);
	copy = CameraDirection;

	CameraDirection.X = copy.X * c + copy.Z * s;
	CameraDirection.Z = copy.X * -s + copy.Z * c;

	s = (float)sin(angleZ);
	c = (float)cos(angleZ);
	copy = CameraDirection;

	CameraDirection.X = copy.X * c + copy.Y * -s;
	CameraDirection.Y = copy.Y * c + copy.X * s;
}

Vector Player::RightVector()
{
	return Vector(0, 1, 0).Rotate(CameraRotation);
}

Vector Player::UpVector()
{
	return Vector(0, 0, 1).Rotate(CameraRotation);
}
