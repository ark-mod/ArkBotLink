#pragma once

#include "API/Ark/Ark.h"

namespace OnlinePlayers
{
	void LoginCallback(APlayerController* NewPlayer);
	void LogoutCallback(AController* Exiting);
}