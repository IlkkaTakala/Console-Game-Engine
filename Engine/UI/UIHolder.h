#pragma once
#include "UIElements.h"
#include "Objects/BaseObject.h"

class UIHolder : public Data
{
public:
	UIHolder() { root = nullptr; }
	virtual ~UIHolder() { delete root; };
	virtual void DestroyObject() override {}
	virtual void Construct() {};
	void DrawUI(const int SizeX, const int SizeY, Pixel* Buffer) { if (root != nullptr) root->Draw(SizeX, SizeY, Buffer); }
protected:
	ElementBase* root;
};