#pragma once
#include "BasicTypes.h"
#include <vector>
#include <string>

struct Pixel;

class ElementBase
{
public:
	virtual void Draw(const int SizeX, const int SizeY, Pixel* Buffer) = 0;
	ElementBase(Vector2D loc, Vector2D size) : Location(loc), Size(size), Z_Order(0) {};
	virtual ~ElementBase() { for (ElementBase* c : Children) delete c; }
	ElementBase& AddChild(ElementBase* child) { Children.push_back(child); return *this; }
protected:
	Vector2D Location;
	Vector2D Size;
	int Z_Order;
	std::vector<ElementBase*> Children;

	void BeginDraw(const int SizeX, const int SizeY, Pixel* Buffer);
};

class Panel : public ElementBase
{
public:
	Panel(Vector2D loc, Vector2D size, unsigned char t) : ElementBase(loc, size), type(t) {};

	virtual void Draw(const int SizeX, const int SizeY, Pixel* Buffer);
protected:
	unsigned char type;
};

class Border : public ElementBase
{
public:
	Border(Vector2D loc, Vector2D size, const unsigned char t[9]) : ElementBase(loc, size) {
		Data0 = t[0];
		Data1 = t[1];
		Data2 = t[2];
		Data3 = t[3];
		Data4 = t[4];
		Data5 = t[5];
		Data6 = t[6];
		Data7 = t[7];
	};

	virtual void Draw(const int SizeX, const int SizeY, Pixel* Buffer);
protected:
	unsigned char Data0;
	unsigned char Data1;
	unsigned char Data2;
	unsigned char Data3;
	unsigned char Data4;
	unsigned char Data5;
	unsigned char Data6;
	unsigned char Data7;
};

class Image : public ElementBase
{
public:
	Image(Vector2D loc, const char* data, Vector2D size) : Data(data), ElementBase(loc, size){};
	virtual void Draw(const int SizeX, const int SizeY, Pixel* Buffer) override;
protected:
	const char* Data;
};

class Text : public ElementBase
{
public:
	Text(Vector2D loc, std::string data, Vector2D size) : Data(data), ElementBase(loc, size) {};
	virtual void Draw(const int SizeX, const int SizeY, Pixel* Buffer) override;
protected:
	std::string Data;
};

#define NumberOne	"         M    MM     M     M     M        "