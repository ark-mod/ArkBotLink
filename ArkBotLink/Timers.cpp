#include "Timers.h"
#include "ApiExtensions.h"
#include "ServerInfo.h"
#include "PlayerLocations.h"

extern bool _wasInitialized;

void ArkBotLinkTickCallback(float delta);

void Timers::Load()
{
	PlayerLocations::Load();

	ArkApi::GetCommands().AddOnTickCallback("ArkBotLinkTickCallback", &ArkBotLinkTickCallback);
}

void Timers::Unload()
{
	ArkApi::GetCommands().RemoveOnTickCallback("ArkBotLinkTickCallback");
}

void ArkBotLinkTickCallback(float delta)
{
	ServerInfo::TickCallback();
	PlayerLocations::TickCallback();
}