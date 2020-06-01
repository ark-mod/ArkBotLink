#include "ServerInfo.h"
#include "API/Ark/Ark.h"
#include "ApiExtensions.h"
#include <atomic>
#include "signalr_value.h"
#include "Link.h"
#include "Plugin.h"

extern bool _wasInitialized;
std::atomic<bool> _link_requested_serverInfo { false };

void ServerInfo::TickCallback()
{
	if (!_wasInitialized || _link_requested_serverInfo == false) return;

	UWorld* world = ArkApi::GetApiUtils().GetWorld();
	AShooterGameMode* gameMode = ArkApi::GetApiUtils().GetShooterGameMode();

	FString map_name;
	FString serverName;
	gameMode->GetMapName(&map_name);
	gameMode->GetServerName(&serverName, false);

	auto majorVersion = ArkExtensions::Globals::GServerMajorVersion()();
	auto minorVersion = ArkExtensions::Globals::GServerMinorVersion()();

	FString address;
	world->GetAddressURL(&address);

	//int numPlayers = gameMode->NumPlayersField() + gameMode->NumTravellingPlayersField();
	int maxPlayers = 0; // gameMode->GameSessionField()->MaxPlayersField(); <-- this is an invalid memory location (probably session is just for non dedicated and client side use)

	// RealTimeSeconds: Returns time in seconds since world was brought up for play, does NOT stop when game pauses, NOT dilated/clamped
	// TimeSeconds: Returns time in seconds since world was brought up for play, adjusted by time dilation and IS stopped when game pauses
	// GameTimeSeconds: Get the current game time, in seconds. This stops when the game is paused and is affected by slomo.

	long long t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	double gameTime = world->TimeSecondsField();
	double realTime = world->RealTimeSecondsField();
	double loadedAtTime = world->LoadedAtTimeSecondsField();

	AShooterGameState* shooterGameState = static_cast<AShooterGameState*>(gameMode->GameStateField());
	int& dayNumber = shooterGameState->DayNumberField();

	TArray<UPrimalPlayerData*> playerDatas = gameMode->PlayerDatasField();

	std::map<std::string, signalr::value> o
	{
		{ "id", Plugin::Get().serverKey },
		{ "mapName", map_name.ToString() },
		{ "serverName", serverName.ToString() },
		{ "majorVersion", (double)majorVersion },
		{ "minorVersion", (double)minorVersion },
		//todo: this does not work currently (not sure how to get it - maybe easier to just use the config setting in ark bot instead)
		{ "address", address.ToString() },
		//todo: this does not work currently (maybe possible to get it from server method that handle loging (disallow to many players), or get it from the server options parsed at startup?
		{ "maxPlayers", (double)maxPlayers }, 
		//todo: all times should be real utc time, with "game time" only for cases where we want it
		{ "gameTime", (double)gameTime },
		{ "realTime", (double)realTime },
		{ "utcTime", (double)t },
		{ "loadedAtTime", (double)loadedAtTime },
		{ "dayNumber", (double)dayNumber },
		{ "dayTime", world->CurrentDayTimeField().ToString() }
	};

	std::vector<signalr::value> onlinePlayers;
	for (auto playerData : playerDatas)
	{
		FString& steamId = playerData->MyDataField()->SavedNetworkAddressField();
		long double loginTime = playerData->MyDataField()->LoginTimeField();

		std::map<std::string, signalr::value> p
		{
			{ "steamId", steamId.ToString() },
			{ "time", (double)loginTime },
		};
		onlinePlayers.push_back(signalr::value(p));
	}

	signalr::value args(std::vector<signalr::value> { Plugin::Get().serverKey, o, onlinePlayers });

	Link::Send("ServerInfo", args);
	_link_requested_serverInfo = false;
}


// CURRENT
//[
//	{
//		"address": "",
//		"currentTime": 30026.2265625,
//		"id": "server1",
//		"majorVersion": 306,
//		"mapName": "TheIsland",
//		"maxPlayers": -1474988840,
//		"minorVersion": 83,
//		"players": 1,
//		"serverName": "Tobias"
//	},
//	[
//		{
//			"steamId": "76561198005371608",
//			"time": 103784.85830730572
//		}
//	]
//]

// WEB APP API
//{
//	"Servers": [
//		{
//			"Name": "ARKNordic.se - PvE GE",
//			"Address": "pve.arkbot.dev:27015",
//			"Version": "306.86",
//			"MapName": "Genesis",
//			"OnlinePlayerCount": 1,
//			"OnlinePlayerMax": 35,
//			"InGameTime": "293",
//			"OnlinePlayers": [
//				{
//					"SteamName": "123",
//					"CharacterName": null,
//					"TribeName": null,
//					"DiscordName": null,
//					"TimeOnline": "13m",
//					"TimeOnlineSeconds": 818
//				}
//			]
//		}
//	]
//}

// FROM SOURCE API
//{
//    "info": {
//        "Address": "84.217.10.223:27005",
//        "Name": "ARKNordic.se - PvE GE - (v306.86)",
//        "Map": "Genesis",
//        "Players": 1,
//        "MaxPlayers": 35,
//    },
//    "rules": [
//        {
//            "Name": "DayTime_s",
//            "Value": "292"
//        }
//    ],
//    "playerinfos": [
//        {
//            "Name": "123",
//            "Score": 0,
//            "Time": "00:04:31.8890000"
//        }
//    ]
//}