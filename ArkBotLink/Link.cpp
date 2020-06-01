#include "API/Ark/Ark.h"

#include <iostream>
#include <sstream>
#include "hub_connection_builder.h"
#include "json_helpers.h"
#include "log_writer.h"
#include <future>

#include "Plugin.h"
#include "Link.h"
#include "LinkLog.h"
#include "CrossServerChat.h"

//todo: change all logging to redirect to separate log. console log stuff should only be things of importance
namespace Link
{
    signalr::hub_connection* _connection = nullptr;

    class logger : public signalr::log_writer
    {
        // Inherited via log_writer
        virtual void __cdecl write(const std::string& entry) override
        {
            size_t ofs = entry.find_first_of('\x5B') + 1; // '['
            auto tmp = entry.substr(ofs, entry.length() - ofs);
            tmp = tmp.erase(tmp.find_last_not_of(" \t\n\r\f\v\x1E") + 1);

            LinkLog::GetLog()->info("[signalr, {}", tmp);
        }
    };

    signalr::value fromJsonString(std::string rawJson)
    {
        const auto rawJsonLength = static_cast<int>(rawJson.length());
        JSONCPP_STRING err;
        Json::Value root;

        Json::CharReaderBuilder builder;
        const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
        if (!reader->parse(rawJson.c_str(), rawJson.c_str() + rawJsonLength, &root, &err))
        {
            return nullptr;
        }

        return signalr::createValue(root);
    }

    void Send(const std::string methodName, signalr::value args)
    {
        if (_connection == nullptr || _connection->get_connection_state() != signalr::connection_state::connected)
        {
            LinkLog::GetLog()->info("[signalr, send] {} could not send due to connection state", methodName);
            return;
        }

        LinkLog::GetLog()->info("[signalr, send] {} sending data", methodName);

        (*_connection).invoke(methodName, args, [&methodName](const signalr::value& value, std::exception_ptr exception)
            {
                try
                {
                    if (exception) std::rethrow_exception(exception);
                }
                catch (const std::exception & e)
                {
                    LinkLog::GetLog()->warn("[signalr] Error while sending data: {}", e.what());
                }
            });
    }

    void Start()
    {
        //todo: make log level configurable
        signalr::hub_connection connection = signalr::hub_connection_builder::create(Plugin::Get().linkUri)
            .with_logging(std::make_shared <logger>(), signalr::trace_level::all)
            .build();

        _connection = &connection;

        connection.on("RequestServerInfo", [](const signalr::value& m)
            {
                LinkLog::GetLog()->info("[signalr, receive?] RequestServerInfo");
                _link_requested_serverInfo = true;
            });

        connection.on("ChatMessage", [](const signalr::value& m)
            {
                LinkLog::GetLog()->info("[signalr, receive?] ChatMessage");
                const std::map<std::string, signalr::value> msg = m.as_array()[0].as_map();
                CrossServerChat::MessageFromLinkCallback(msg);
            });

        while (shutdown_now == false)
        {
            std::promise<void> task;
            connection.start([&connection, &task](std::exception_ptr exception)
            {
                if (exception)
                {
                    try
                    {
                        std::rethrow_exception(exception);
                    }
                    catch (const std::exception & ex)
                    {
                        //03/09/20 13:22 [ArkBotLink][warning] [signalr] Exception when starting connection: WinHttpSendRequest: 12029: A connection with the server could not be established
                        LinkLog::GetLog()->warn("[signalr] Exception when starting connection: {}", ex.what());
                    }
                    task.set_value();
                    return;
                }

                while (connection.get_connection_state() == signalr::connection_state::connected)
                {
                    if (shutdown_now == true) break;

                    std::this_thread::sleep_for(std::chrono::milliseconds(250));
                }

                connection.stop([&task](std::exception_ptr exception)
                    {
                        try
                        {
                            if (exception) std::rethrow_exception(exception);
                            LinkLog::GetLog()->info("[signalr] Connection stopped successfully");
                        }
                        catch (const std::exception & e)
                        {
                            LinkLog::GetLog()->warn("[signalr] Exception when stopping connection: {}", e.what());
                        }

                        task.set_value();
                    });
            });

            task.get_future().get();
            
            if (shutdown_now == false) std::this_thread::sleep_for(std::chrono::milliseconds(2500));
        }

        _connection = nullptr;
    }
}