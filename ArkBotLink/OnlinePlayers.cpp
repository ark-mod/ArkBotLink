#include "OnlinePlayers.h"
#include "Link.h"
#include "Plugin.h"

void OnlinePlayers::LoginCallback(APlayerController* NewPlayer)
{
    if (NewPlayer == nullptr) return;

    AShooterPlayerController* controller = static_cast<AShooterPlayerController*>(NewPlayer);

    FString netId;
    controller->GetUniqueNetIdAsString(&netId);

    bool createdNewPlayerData;
    FPrimalPlayerCharacterConfigStruct playerCharacterConfigStruct;

    double loginTime = ArkApi::GetApiUtils().GetWorld()->TimeSecondsField();

    Link::Send("PlayerLogin", signalr::value(std::vector<signalr::value> { Plugin::Get().serverKey, netId.ToString(), (double)loginTime }));
}

void OnlinePlayers::LogoutCallback(AController* Exiting)
{
    if (Exiting == nullptr) return;

    AShooterPlayerController* controller = static_cast<AShooterPlayerController*>(Exiting);

    FString netId;
    controller->GetUniqueNetIdAsString(&netId);

    Link::Send("PlayerLogout", signalr::value(std::vector<signalr::value> { Plugin::Get().serverKey, netId.ToString() }));
}