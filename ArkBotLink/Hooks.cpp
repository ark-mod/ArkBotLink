#include "Hooks.h"
#include "API/Ark/Ark.h"
#include "OnlinePlayers.h"

bool hooked;

extern void Init();

void Hooks::Load()
{
    if (ArkApi::GetApiUtils().GetStatus() == ArkApi::ServerStatus::Ready) Init();
    else
    {
        hooked = true;
        ArkApi::GetHooks().SetHook("AShooterGameMode.BeginPlay", &Hook_AShooterGameMode_BeginPlay, &AShooterGameMode_BeginPlay_original);
    }

    ArkApi::GetHooks().SetHook("AGameMode.PostLogin", &Hook_AGameMode_PostLogin, &AGameMode_PostLogin_original);
    ArkApi::GetHooks().SetHook("AGameMode.Logout", &Hook_AGameMode_Logout, &AGameMode_Logout_original);
}

void Hooks::Unload()
{
    if (hooked) ArkApi::GetHooks().DisableHook("AShooterGameMode.BeginPlay", &Hook_AShooterGameMode_BeginPlay);

    ArkApi::GetHooks().DisableHook("AGameMode.PostLogin", &Hook_AGameMode_PostLogin);
    ArkApi::GetHooks().DisableHook("AGameMode.Logout", &Hook_AGameMode_Logout);
}

void Hook_AShooterGameMode_BeginPlay(AShooterGameMode* _AShooterGameMode)
{
	AShooterGameMode_BeginPlay_original(_AShooterGameMode);

	Init();
}

void __fastcall Hook_AGameMode_PostLogin(AGameMode* _this, APlayerController* NewPlayer)
{
    AGameMode_PostLogin_original(_this, NewPlayer);

    OnlinePlayers::LoginCallback(NewPlayer);
}

void __fastcall Hook_AGameMode_Logout(AGameMode* _this, AController* Exiting)
{
    AGameMode_Logout_original(_this, Exiting);

    OnlinePlayers::LogoutCallback(Exiting);
}