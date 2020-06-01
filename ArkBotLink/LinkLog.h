#pragma once

#include "Logger/spdlog/spdlog.h"
#include <Tools.h>
#include <iostream>
#include <IApiUtils.h>
#include <filesystem>

class LinkLog
{
public:
	LinkLog(const LinkLog&) = delete;
	LinkLog(LinkLog&&) = delete;
	LinkLog& operator=(const LinkLog&) = delete;
	LinkLog& operator=(LinkLog&&) = delete;

	static LinkLog& Get()
	{
		static LinkLog instance;
		return instance;
	}

	static std::shared_ptr<spdlog::logger>& GetLog()
	{
		return Get().logger_;
	}

private:
	LinkLog()
	{
		try
		{
			std::filesystem::create_directory(API::Tools::GetCurrentDir() + "/ArkApi/Plugins/ArkBotLink/logs");

			auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
				spdlog::sinks::default_daily_file_name_calculator::
				calc_filename(API::Tools::GetCurrentDir() + "/ArkApi/Plugins/ArkBotLink/logs/LinkLog_" + std::to_string(GetCurrentProcessId()) + ".log"), 
				1024 * 1024, 5
				);

			logger_ = std::make_shared<spdlog::logger>("ArkBotLink", sink);

			logger_->set_pattern("%D %R [%l] %v");
			logger_->flush_on(spdlog::level::info);
		}
		catch (const std::exception&)
		{
			std::cout << "Failed to create log file\n";
		}
	}

	~LinkLog() = default;

	std::shared_ptr<spdlog::logger> logger_;
};