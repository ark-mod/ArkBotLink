#pragma once

#include "API/Ark/Ark.h"

namespace Hooks
{
	void Load();
	void Unload();
}

DECLARE_HOOK(UWorld_LoadFromFile, __int64, UWorld*, const FString*);
DECLARE_HOOK(AGameMode_PostLogin, void, AGameMode*, APlayerController*);
DECLARE_HOOK(AGameMode_Logout, void, AGameMode*, AController*);
DECLARE_HOOK(AShooterGameMode_BeginPlay, void, AShooterGameMode*);