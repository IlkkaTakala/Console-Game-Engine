#pragma once
#include <math.h>
#include <string>
#define PI 3.14159265359f

struct Vector
{
	float X, Y, Z;

	Vector() { X = 0, Y = 0, Z = 0; }
	Vector(float X, float Y, float Z) : X(X), Y(Y), Z(Z) {};
	Vector(std::string in) { 
		size_t off = 0;
		size_t o_off = 0;
		X = std::stof(in.substr(o_off, off = in.find(',', off)));
		o_off = off;
		Y = std::stof(in.substr(o_off + 1, off = in.find(',', off + 1))); 
		o_off = off;
		Z = std::stof(in.substr(o_off + 1, off = in.find(',', off + 1)));
	}

	float Length() { return (float)sqrt(X * X + Y * Y + Z * Z); }
	/*Vector Normalize() { float mag = Length(); return Vector(X / mag, Y / mag, Z / mag); }*/

	//float Length() { return; }

	Vector Normalize() { return *this * Q_rsqrt(X * X + Y * Y + Z * Z); }

	Vector Rotate(const Vector& In) 
	{
		Vector out;
		out = *this;
		float angleX = In.X * PI / 180.f;
		float angleY = -In.Y * PI / 180.f;
		float angleZ = In.Z * PI / 180.f;

		float s = (float)sin(angleX);
		float c = (float)cos(angleX);
		Vector copy = out;

		out.Y = copy.Y * c - copy.Z * s;
		out.Z = copy.Y * s + copy.Z * c;

		s = (float)sin(angleY);
		c = (float)cos(angleY);
		copy = out;

		out.X = copy.X * c + copy.Z * s;
		out.Z = copy.X * -s + copy.Z * c;

		s = (float)sin(angleZ);
		c = (float)cos(angleZ);
		copy = out;

		out.X = copy.X * c + copy.Y * -s;
		out.Y = copy.Y * c + copy.X * s;
		return out;
	}

	static float Dot(const Vector& in1, const Vector& in2) { return in1.X* in2.X + in1.Y * in2.Y + in1.Z * in2.Z; }
	std::string String() { return std::string(std::to_string(X) + ',' + std::to_string(Y) + ',' + std::to_string(Z)); }

	friend Vector operator+(const Vector& obj, const Vector& obj2) { return Vector(obj2.X + obj.X, obj2.Y + obj.Y, obj2.Z + obj.Z); }

	friend Vector operator-(const Vector& obj, const Vector& obj2) { return Vector(obj2.X - obj.X, obj2.Y - obj.Y, obj2.Z - obj.Z); }

	Vector& operator*=(const Vector& obj) { X *= obj.X; Y *= obj.Y; Z *= obj.Z; return *this; }
	friend Vector operator*(Vector obj, const Vector& obj2) { obj *= obj2; return obj; }

	Vector& operator*=(const int& obj) { X *= obj; Y *= obj; Z *= obj; return *this; }
	friend Vector operator*(Vector obj, const int& obj2) { obj *= obj2; return obj; }
	friend Vector operator*(const int& obj, Vector obj2) { obj2 *= obj; return obj2; }

	Vector& operator*=(const float& obj) { X *= obj; Y *= obj; Z *= obj; return *this; }
	friend Vector operator*(Vector obj, const float& obj2) { obj *= obj2; return obj; }
	friend Vector operator*(const float& obj, Vector obj2) { obj2 *= obj; return obj2; }

	friend Vector operator/(const Vector& obj, const Vector& obj2) { return Vector(obj2.X / obj.X, obj2.Y / obj.Y, obj2.Z / obj.Z); }

	friend bool operator==(const Vector& obj, const Vector& obj2) { return obj2.X == obj.X && obj2.Y == obj.Y && obj2.Z == obj.Z; }

	float Q_rsqrt(float number)
	{
		long i;
		float x2, y;
		const float threehalfs = 1.5F;

		x2 = number * 0.5F;
		y = number;
		i = *(long*)&y;                       // evil floating point bit level hacking
		i = 0x5f3759df - (i >> 1);               // what the fuck? 
		y = *(float*)&i;
		y = y * (threehalfs - (x2 * y * y));   // 1st iteration
	//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

		return y;
	}

};

struct Vector2D
{
	long int X, Y;

	Vector2D() { X = 0, Y = 0; }
	Vector2D(long X, long Y) : X(X), Y(Y) {};

	Vector2D operator+(const Vector2D& obj) { return Vector2D(X + obj.X, Y + obj.Y); }
	Vector2D operator-(const Vector2D& obj) { return Vector2D(X - obj.X, Y - obj.Y); }
	Vector2D operator*(const Vector2D& obj) { return Vector2D(X * obj.X, Y * obj.Y); }
	Vector2D operator/(const Vector2D& obj) { return Vector2D(X / obj.X, Y / obj.Y); }

	friend bool operator==(const Vector2D& obj, const Vector2D& obj2) { return obj2.X == obj.X && obj2.Y == obj.Y; }
};