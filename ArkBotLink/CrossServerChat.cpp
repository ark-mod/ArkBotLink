#include "CrossServerChat.h"
#include "Link.h"
#include "Plugin.h"

void CrossServerChat::MessageFromLinkCallback(const std::map<std::string, signalr::value> msg)
{
	auto _at = msg.find("at");
	auto _serverKey = msg.find("serverKey");
	auto _steamId = msg.find("steamId");
	auto _playerName = msg.find("playerName");
	auto _characterName = msg.find("characterName");
	auto _tribeName = msg.find("tribeName");
	auto _message = msg.find("message");
	auto _mode = msg.find("mode");
	auto _icon = msg.find("icon");

	if (_at == msg.end() || _serverKey == msg.end() || _steamId == msg.end() || _playerName == msg.end() || _characterName == msg.end() || _tribeName == msg.end() || _message == msg.end() || _mode == msg.end() || _icon == msg.end()) return;

	long at = _at->second.as_double();
	std::string serverKey = _serverKey->second.as_string();
	std::string steamId = _steamId->second.as_string();
	std::string playerName = _playerName->second.as_string();
	std::string characterName = _characterName->second.as_string();
	std::string tribeName = _tribeName->second.as_string();
	std::string message = _message->second.as_string();
	int mode = _mode->second.as_double();
	int icon = _icon->second.as_double();

	//todo: send message to connected clients
}

std::string GetPlayerName(AShooterPlayerController* playerController)
{
	auto playerState = playerController->PlayerStateField();
	if (playerState == nullptr) return nullptr;

	return playerState->PlayerNameField().ToString();
}

std::string GetPlayerCharacterName(AShooterPlayerController* playerController)
{
	auto characterName = ArkApi::GetApiUtils().GetCharacterName(playerController);

	return characterName.ToString();
}

std::string GetTribeName(AShooterPlayerController* playerController)
{
	auto playerState = reinterpret_cast<AShooterPlayerState*>(playerController->PlayerStateField());
	if (playerState == nullptr) return nullptr;

	auto tribeData = playerState->MyTribeDataField();
	return tribeData->TribeNameField().ToString();
}

enum class ChatIcon { None, Admin };

bool ChatMessageCallback(AShooterPlayerController* _AShooterPlayerController, FString* Message, EChatSendMode::Type Mode, bool spam_check, bool command_executed)
{
	//todo: this method relays messages that get stopped by the internal spam check in ark
	if (spam_check || command_executed) return false;

	// process the message and relay
	long double lastChatMessageTime = 0;
	std::string msg;
	if (_AShooterPlayerController && Message && Mode == EChatSendMode::Type::GlobalChat)
	{
		lastChatMessageTime = _AShooterPlayerController->LastChatMessageTimeField();
		msg = Message->ToString();

		auto steamId = ArkApi::GetApiUtils().GetSteamIdFromController(_AShooterPlayerController);
		auto playerName = GetPlayerName(_AShooterPlayerController);
		auto characterName = GetPlayerCharacterName(_AShooterPlayerController);
		auto tribeName = GetTribeName(_AShooterPlayerController);

		auto icon = _AShooterPlayerController->bIsAdmin()()
			&& !_AShooterPlayerController->bSuppressAdminIconField()
			? ChatIcon::Admin
			: ChatIcon::None;

		//todo: maybe support other badges in the future (but when are they used?)
		//auto badgeGroups = uPrimalGameData->BadgeGroupsNameTagField()();
		//if (badgeGroups.Num() > icon) icon = reinterpret_cast<DWORD64>(badgeGroups[icon]);

		std::map<std::string, signalr::value> chatmsg
		{
			// c# strings serialzied have this format: 2020-03-11T02:00:45.2456747Z
			{ "at", std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) },
			{ "serverKey", Plugin::Get().serverKey },
			{ "steamId", std::to_string(steamId) },
			{ "playerName", playerName },
			{ "characterName", ArkApi::Tools::Utf8Encode(ArkApi::Tools::ConvertToWideStr(characterName)) },
			{ "tribeName", tribeName },
			{ "message", ArkApi::Tools::Utf8Encode(ArkApi::Tools::ConvertToWideStr(msg)) },
			{ "mode", (double)Mode },
			{ "icon", (double)icon }
		};

		Link::Send("ChatMessage", signalr::value(std::vector<signalr::value> { signalr::value(chatmsg) }));
	}

	return false;
}