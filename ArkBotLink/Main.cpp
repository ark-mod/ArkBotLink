#include "Plugin.h"
#include "Link.h"
#include "Hooks.h"
#include "Timers.h"
#include "CrossServerChat.h"
#include <fstream>

#pragma comment(lib, "ArkApi.lib")

bool _wasInitialized = false;

void LoadConfig()
{
	auto& plugin = Plugin::Get();
	std::ifstream file(ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/ArkBotLink/config.json");
	if (!file.is_open())
	{
		Log::GetLog()->error("Could not open file config.json");
		throw;
	}

	file >> plugin.json;
	file.close();

	plugin.serverKey = plugin.json.value("serverKey", "");
	if (plugin.serverKey.empty())
	{
		Log::GetLog()->error("serverKey must be set in config.json");
		throw;
	}

	plugin.linkUri = plugin.json.value("linkUri", "");
	if (plugin.linkUri.empty())
	{
		Log::GetLog()->error("linkUri must be set in config.json");
		throw;
	}

	plugin.playerLocationsPushInterval = plugin.json.value("playerLocationsPushInterval", 10000);
	if (plugin.playerLocationsPushInterval < 500)
	{
		plugin.playerLocationsPushInterval = 500;

		Log::GetLog()->error("playerLocationsPushInterval below minimum value (500)");
		throw;
	}
	
}

void ReloadConfig(APlayerController* playerController, FString* cmd, bool shouldLog)
{
	auto& plugin = Plugin::Get();

	LoadConfig();
	//todo: maybe config should use the old "dynamic" config subsystem
	//ArkLibrary::ReloadInternalSettingsForCommandsAndFeatures(plugin.json);

	auto aShooterPlayerController = static_cast<AShooterPlayerController*>(playerController);
	ArkApi::GetApiUtils().SendChatMessage(aShooterPlayerController, L"[system]", L"ARK Bot Link config reloaded!");
}

void Init()
{
	_wasInitialized = true;
}

void Load()
{
	Log::Get().Init("ArkBotLink");

#ifdef DEVELOPER_BUILD
	Load_Developer();
#endif

	auto& plugin = Plugin::Get();
	auto& commands = ArkApi::GetCommands();

	LoadConfig();
	//ArkLibrary::LoadCommandsAndFeatures(plugin.json);

    std::thread thread = std::thread([]()
    {
        Link::Start();
    });

    thread.detach();

	Hooks::Load();
	Timers::Load();

	//try
	//{
	//	
	//}
	//catch (const std::exception& error)
	//{
	//	Log::GetLog()->error(error.what());
	//	throw;
	//}

	commands.AddOnChatMessageCallback("ChatMessageCallback", &ChatMessageCallback);
}

void Unload()
{
	// signal the link subsystem to shutdown
	Link::shutdown_now = true;

#ifdef DEVELOPER_BUILD
	Unload_Developer();
#endif

	Hooks::Unload();
	Timers::Unload();

    ArkApi::GetCommands().RemoveOnChatMessageCallback("ChatMessageCallback");
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Load();
		break;
	case DLL_PROCESS_DETACH:
		Unload();
		break;
	}
	return TRUE;
}