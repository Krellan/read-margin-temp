#include <iostream>
#include <string>
#include <map>
#include <filesystem>

#include <nlohmann/json.hpp>

#include "conf.hpp"
#include "json/buildjson.hpp"
#include "sensor/sensor.hpp"
#include "util/util.hpp"
#include "dbus/dbus.hpp"

constexpr auto marginConfigPath =
    "/usr/share/read-margin-temp/config-margin.json";

constexpr auto debugEnablePath = "/etc/thermal.d/margindebug";
extern bool debugEnabled;

std::map<std::string, struct conf::sensorConfig> sensorConfig = {};
conf::skuConfig skusConfig;

void run(const std::string& configPath)
{
    try
    {
        auto jsonData = parseValidateJson(configPath);
        sensorConfig = getSensorInfo(jsonData);
        skusConfig = getSkuInfo(jsonData);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed during building json file: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    /** determine sku **/
    int skuNum = getSkuNum();

    /** get sku info **/
    auto skuConfig = skusConfig[skuNum];

    /** start updating margin temp loop **/
    updateMarginTempLoop(skuConfig, sensorConfig);
}

int main(int argc, char **argv)
{
    std::string configPath = marginConfigPath;

    if (argc > 1)
    {
        configPath = argv[1];
    }

    if (std::filesystem::exists(debugEnablePath))
    {
        debugEnabled = true;
        std::cerr << "Debug logging enabled\n";
    }

    run(configPath);

    return 0;
}
