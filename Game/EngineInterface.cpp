#include "Gameplay/GameState.h"
#include "TestPlayer.h"
#include "TestUI.h"
#include "EngineInterface.h"

Ref<GameState> EngineInterface::CreateDefaults()
{
    Ref<GameState> State = SpawnObject<GameState>();
    TestPlayer* pla = SpawnObject<TestPlayer>();
    State->SetPlayer<TestPlayer>(pla);
    pla->SetUI<TestUI>();
    return State;
}
