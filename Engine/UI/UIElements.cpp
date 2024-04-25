#include <list>
#include "UIElements.h"
#include "Renderer.h"

void Panel::Draw(const int SizeX, const int SizeY, Pixel* Buffer)
{
	if (Location.X > SizeX || Location.Y > SizeY) return;
	int useX = Size.X + Location.X > SizeX ? SizeX : Size.X + Location.X;
	int useY = Size.Y + Location.Y > SizeY ? SizeY : Size.Y + Location.Y;
	for (int y = Location.Y; y < useY; y++) {
		for (int x = Location.X; x < useX; x++) {
			Buffer[y * SizeX + x].shade = type;
			SetColor(Buffer[y * SizeX + x], WHT);
		}
	}
	BeginDraw(SizeX, SizeY, Buffer);
}

void Border::Draw(const int SizeX, const int SizeY, Pixel* Buffer)
{
	if (Location.X > SizeX || Location.Y > SizeY) return;
	int useX = Size.X + Location.X > SizeX ? SizeX : Size.X + Location.X;
	int useY = Size.Y + Location.Y > SizeY ? SizeY : Size.Y + Location.Y;
	for (int y = Location.Y; y < useY; y++) {
		for (int x = Location.X; x < useX; x++) {
			if (y == Location.Y && x == Location.X)
				Buffer[y * SizeX + x].shade = Data0;
			else if (y == Location.Y && x == useX - 1)
				Buffer[y * SizeX + x].shade = Data2;
			else if (y == useY - 1 && x == Location.X)
				Buffer[y * SizeX + x].shade = Data5;
			else if (y == useY - 1 && x == useX - 1)
				Buffer[y * SizeX + x].shade = Data7;

			else if (y == Location.Y)
				Buffer[y * SizeX + x].shade = Data1;
			else if (y == useY - 1)
				Buffer[y * SizeX + x].shade = Data6;

			else if (x == Location.X)
				Buffer[y * SizeX + x].shade = Data3;
			else if (x == useX - 1)
				Buffer[y * SizeX + x].shade = Data4;

			SetColor(Buffer[y * SizeX + x], WHT);
		}
	}
	BeginDraw(SizeX, SizeY, Buffer);
}

void ElementBase::BeginDraw(const int SizeX, const int SizeY, Pixel* Buffer)
{
	if (Children.size() > 0) {
		std::list<int> Cleared;
		for (int z = 0; ; z++) {
			if (Cleared.size() >= Children.size()) break;
			for (int i = 0; i < Children.size(); i++) {
				if (Children[i]->Z_Order == z) {
					Children[i]->Draw(SizeX, SizeY, Buffer);
					Cleared.push_back(i);
				}
			}
		}
	}
}

void Image::Draw(const int SizeX, const int SizeY, Pixel* Buffer)
{
	if (Location.X > SizeX || Location.Y > SizeY) return;
	int useX = Size.X + Location.X > SizeX ? SizeX : Size.X + Location.X;
	int useY = Size.Y + Location.Y > SizeY ? SizeY : Size.Y + Location.Y;
	int index = 0;
	for (int y = Location.Y; y < useY; y++) {
		for (int x = Location.X; x < useX; x++) {
			Buffer[y * SizeX + x].shade = Data[index];
			SetColor(Buffer[y * SizeX + x], WHT);
			index++;
		}
	}
	BeginDraw(SizeX, SizeY, Buffer);
}

void Text::Draw(const int SizeX, const int SizeY, Pixel* Buffer)
{
	if (Location.X > SizeX || Location.Y > SizeY) return;
	int useX = Size.X + Location.X > SizeX ? SizeX : Size.X + Location.X;
	int useY = Size.Y + Location.Y > SizeY ? SizeY : Size.Y + Location.Y;
	int idx = 0;
	for (int y = Location.Y; y < useY; y++) {
		for (int x = Location.X; x < useX; x++) {
			if (idx > Data.size() - 1) break;
			Buffer[y * SizeX + x].shade = Data[idx++];
			SetColor(Buffer[y * SizeX + x], WHT);
		}
	}
	BeginDraw(SizeX, SizeY, Buffer);
}
