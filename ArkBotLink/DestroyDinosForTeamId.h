#pragma once

#include <API/ARK/Ark.h>

std::string DestroyDinosForTeamIdInternal(FString* cmd);
void DestroyDinosForTeamIdRconCommand(RCONClientConnection* rconClientConnection, RCONPacket* rconPacket, UWorld* uWorld);