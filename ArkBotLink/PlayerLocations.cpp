#include "PlayerLocations.h"
#include "API/Ark/Ark.h"
#include "ApiExtensions.h"
#include "Link.h"
#include "Plugin.h"
#include <chrono>

std::chrono::time_point<std::chrono::steady_clock> _last_report;

void PlayerLocations::Load()
{
	_last_report = std::chrono::high_resolution_clock::now();
}

void PlayerLocations::TickCallback()
{
	auto t = std::chrono::high_resolution_clock::now();

	if (std::chrono::duration_cast<std::chrono::milliseconds>(t - _last_report).count() >= Plugin::Get().playerLocationsPushInterval)
	{
		_last_report = t;

		float longitudeOrigin = -400000.0, latitudeOrigin = -400000.0, longitudeScale = 800.0, latitudeScale = 800.0;
		float multiplierX, multiplierY, shiftX, shiftY;

		UWorld* world = ArkApi::GetApiUtils().GetWorld();
		AWorldSettings* worldSettings = world->GetWorldSettings(false, true);
		if (worldSettings != nullptr)
		{
			auto primalWorldSettingsEx = static_cast<ArkExtensions::APrimalWorldSettings*>(worldSettings);

			if (primalWorldSettingsEx->bOverrideLongitudeAndLatitudeField())
			{
				longitudeOrigin = primalWorldSettingsEx->LongitudeOriginField();
				latitudeOrigin = primalWorldSettingsEx->LatitudeOriginField();
				longitudeScale = primalWorldSettingsEx->LongitudeScaleField();
				latitudeScale = primalWorldSettingsEx->LatitudeScaleField();
			}
		}

		multiplierX = longitudeScale * 10.0;
		shiftX = -longitudeOrigin / multiplierX;
		multiplierY = latitudeScale * 10.0;
		shiftY = -latitudeOrigin / multiplierY;

		// float latitude = Y / multiplierY + shiftY;
		// float longitude = X / multiplierX + shiftX;
		// float X = ((longitude * longitudeScale) * 10.0) + longitudeOrigin;
		// float Y = ((latitude * latitudeScale) * 10.0) + latitudePrigin;

		std::vector<signalr::value> playerLocations;

		const auto& player_controllers = world->PlayerControllerListField();
		for (TWeakObjectPtr<APlayerController> player_controller : player_controllers)
		{
			const uint64 steamId = ArkApi::GetApiUtils().GetSteamIdFromController(player_controller.Get());
			const FVector pos = player_controller->DefaultActorLocationField();

			float latitude = pos.Y / multiplierY + shiftY;
			float longitude = pos.X / multiplierX + shiftX;

			std::map<std::string, signalr::value> ploc
			{
				{ "steamId", std::to_string(steamId) }, 
				{ "x", (double)pos.X }, 
				{ "y", (double)pos.Y }, 
				{ "z", (double)pos.Z }, 
				{ "latitude", (double)latitude }, 
				{ "longitude", (double)longitude }
			};

			playerLocations.push_back(signalr::value(ploc));
		}

		Link::Send("PlayerLocations", signalr::value(std::vector<signalr::value> { Plugin::Get().serverKey, playerLocations }));
	}
}