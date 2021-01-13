#include <iostream>
#include <filesystem>
#include <string>
#include <map>

#include <nlohmann/json.hpp>

#include "conf.hpp"
#include "json/buildjson.hpp"
#include "sensor/sensor.hpp"
#include "util/util.hpp"
#include "dbus/dbus.hpp"

constexpr auto MARGINCONFIGPATH =
    "/usr/share/read-margin-temp/config-margin.json";

constexpr auto debugEnablePath = "/etc/thermal.d/margindebug";
extern bool debugEnabled;

constexpr auto spofDisablePath = "/etc/thermal.d/spofdisable";
extern bool spofEnabled;

std::map<std::string, struct conf::SensorConfig> sensorConfig = {};
std::map<int, conf::SkuConfig> skusConfig;

void printHelp()
{
    std::cout << "Option : " << std::endl;
    std::cout << "    --file, -f [path]: the direct path of config file"
              << std::endl;
    std::cout << "        default : " << MARGINCONFIGPATH << std::endl;
    std::cout << "    --loose, -l : ignore a sensor lost"
              << std::endl;
    std::cout << "        default : consider sensor lost" << std::endl;
    std::cout << "" << std::endl;
}

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
    std::string configPath = MARGINCONFIGPATH;

    bool inValiedoption = false;

    debugEnabled = false;
    if (std::filesystem::exists(debugEnablePath))
    {
        debugEnabled = true;
        std::cerr << "Debug logging enabled\n";
    }

    // TODO(): This changes default behavior,
    // from fail-if-all to fail-if-one sensor broken,
    // which may be surprising to other users,
    // so consider changing this before releasing.
    spofEnabled = true;
    if (std::filesystem::exists(spofDisablePath))
    {
        spofEnabled = false;
        std::cerr << "Single point of failure disabled\n";
    }

    for (int i = 1; i < argc; i++)
    {
        if (std::strcmp(argv[i], "--file") == 0 || std::strcmp(argv[i], "-f") == 0)
        {
            configPath = argv[i + 1];
            i++;
        }
        else if (std::strcmp(argv[i], "--loose") == 0 || std::strcmp(argv[i], "-l") == 0)
        {
            spofEnabled = false;
        }
        else
        {
            inValiedoption = true;
            printHelp();
        }
    }

    if (!inValiedoption)
    {
        run(configPath);
    }

    return 0;
}
