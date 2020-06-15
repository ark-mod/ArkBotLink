#pragma once

#include <API/ARK/Ark.h>

std::string DestroyAllStructuresForTeamIdInternal(FString* cmd);
void DestroyAllStructuresForTeamIdRconCommand(RCONClientConnection* rconClientConnection, RCONPacket* rconPacket, UWorld* uWorld);
