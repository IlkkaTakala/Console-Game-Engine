#include <iostream>
#include "GameLoop.h"
//#include "../data.h"
int main()
{
	//setup();
	//setlocale(LC_ALL, "");
	GameLoop* Game = new GameLoop();
	int retur = Game->Start();
	delete Game;
	switch (retur)
	{
	case 10:
		printf("Failed to load INI file\n");
	default:
		break;
	}
	system("pause");
	return retur;
}