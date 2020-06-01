#pragma once

#include "API/Ark/Ark.h"
#include "signalr_value.h"
#include <string>
#include <map>

namespace CrossServerChat
{
	void MessageFromLinkCallback(const std::map<std::string, signalr::value> msg);
}

bool ChatMessageCallback(AShooterPlayerController* _AShooterPlayerController, FString* Message, EChatSendMode::Type Mode, bool spam_check, bool command_executed);