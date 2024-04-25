#include "Gameplay/PlayerController.h"
#include "UI/UIHolder.h"
#include "GameState.h"

GameState::GameState()
{
	Sun = Vector(1, 1, -1).Normalize();
}

void GameState::BeginPlay()
{
}
