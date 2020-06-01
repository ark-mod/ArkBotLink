#pragma once

#include <string>
#include "signalr_value.h"
#include <atomic>

#ifndef LINK_H
#define LINK_H

extern std::atomic<bool> _link_requested_serverInfo;

namespace Link
{
	static std::atomic<bool> shutdown_now = false;

	void Send(const std::string methodName, signalr::value args);
	void Start();

	signalr::value fromJsonString(std::string rawJson);
}
#endif