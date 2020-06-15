#pragma once

#include "ApiExtensions.h"

std::string DestroyStructuresForTeamIdAtPositionInternal(FString* cmd);
void DestroyStructuresForTeamIdAtPositionRconCommand(RCONClientConnection* rconClientConnection, RCONPacket* rconPacket, UWorld* uWorld);
