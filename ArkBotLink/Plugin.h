#pragma once
#include "json.hpp"

class Plugin
{
public:
	nlohmann::json json;

	std::string serverKey;
	std::string linkUri;
	int playerLocationsPushInterval;

	static Plugin& Get();

	Plugin(const Plugin&) = delete;
	Plugin(Plugin&&) = delete;
	Plugin& operator=(const Plugin&) = delete;
	Plugin& operator=(Plugin&&) = delete;

private:
	Plugin() = default;
	~Plugin() = default;
};