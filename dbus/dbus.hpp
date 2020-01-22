#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/bus/match.hpp>
#include <sdbusplus/message.hpp>
#include <xyz/openbmc_project/Common/error.hpp>

constexpr auto marginTempPath = "/xyz/openbmc_project/extsensors/margin";
constexpr auto dbusPropertyIface = "org.freedesktop.DBus.Properties";

namespace dbus
{
class SDBusPlus
{
public:
    template <typename T>
    static auto
        setProperty(sdbusplus::bus::bus& bus, const std::string& busName,
            const std::string& objPath, const std::string& interface,
            const std::string& property, const T& value)
    {
        sdbusplus::message::variant<T> data = value;

        try
        {
            auto methodCall = bus.new_method_call(
                busName.c_str(), objPath.c_str(), dbusPropertyIface, "Set");

            methodCall.append(interface.c_str());
            methodCall.append(property);
            methodCall.append(data);

            auto reply = bus.call(methodCall);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Set properties fail. ERROR = " << e.what()
                << std::endl;
            std::cerr << "Object path = " << objPath << std::endl;
            return;
        }
    }

    template <typename Property>
    static auto
        getProperty(sdbusplus::bus::bus& bus, const std::string& busName,
                    const std::string& objPath, const std::string& interface,
                    const std::string& property)
    {
        auto methodCall = bus.new_method_call(busName.c_str(), objPath.c_str(),
            dbusPropertyIface, "Get");

        methodCall.append(interface.c_str());
        methodCall.append(property);

        sdbusplus::message::variant<Property> value;

        try
        {
            auto reply = bus.call(methodCall);
            reply.read(value);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Get properties fail.. ERROR = " << e.what()
                      << std::endl;
            std::cerr << "Object path = " << objPath << std::endl;
            return -1;
        }

        return sdbusplus::message::variant_ns::get<Property>(value);
    }
};
}
