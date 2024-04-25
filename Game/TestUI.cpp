#include "UI/UIElements.h"
#include "TestUI.h"


void TestUI::Construct()
{
	unsigned char borders[9] = {201, 205, 187, 186, 186, 200, 205, 188};
	root = new Panel(Vector2D(0, 75), Vector2D(100, 25), ' ');
	root->AddChild(
		new Image(Vector2D(94, 0), NumberOne, Vector2D(6, 7))
	).AddChild(
		new Text(Vector2D(3, 77), "This is an example game. You can move around by using the WASD, shift and space keys. You may move the camera using your mouse. "
									"The game saves your location automatically when you quit, but you can change it manually from the settings file", Vector2D(53, 10))
	).AddChild(
		new Border(Vector2D(0, 75), Vector2D(100, 25), borders)
	)
	;
}
