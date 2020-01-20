#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <map>
#include <vector>

#include <sdbusplus/server.hpp>
#include <sdbusplus/bus.hpp>
#include <sdeventplus/event.hpp>
#include <xyz/openbmc_project/Sensor/Value/server.hpp>

#include "conf.hpp"
#include "util/util.hpp"
#include "sensor/sensor.hpp"
#include "dbus/dbus.hpp"



int getSkuNum()
{
    int skuNum = 1;

    /* TBD */

    return skuNum;
}

void updateDbusMarginTemp(int64_t marginTemp)
{
    sdbusplus::bus::bus bus = sdbusplus::bus::new_default();
    std::string INVENTORY_BUSNAME = "xyz.openbmc_project.Hwmon.external";
    std::string ITEM_IFACE = "xyz.openbmc_project.Sensor.Value";
    std::string path = "/xyz/openbmc_project/extsensors/margin/fleeting0";

    dbus::SDBusPlus::setProperty(
        bus, INVENTORY_BUSNAME, path, ITEM_IFACE, "Value", marginTemp);
}

void updateMarginTempLoop(
    std::map<int, std::vector<std::string>> skuConfig,
    std::map<std::string, struct conf::sensorConfig> sensorConfig)
{
    std::fstream sensorTempFile;
    std::string tmp;
    int numOfZones = skuConfig.size();
    int sensorRealTemp;
    int sensorSpecTemp;
    int sensorMarginTemp;
    int64_t marginTemp;
    std::map<std::string, struct conf::sensorConfig> sensorList[numOfZones];

    for (int i = 0; i < numOfZones; i++)
    {
        for (auto t = skuConfig[i].begin(); t != skuConfig[i].end(); t++)
        {
            sensorList[i][*t] = sensorConfig[*t];
        }
    }

    while (true)
    {
        for (int i = 0; i < numOfZones; i++)
        {
            marginTemp = -1;
            for (auto t = sensorList[i].begin(); t != sensorList[i].end(); t++)
            {
                sensorRealTemp = 0;
                sensorSpecTemp = sensorList[i][t->first].spec;
                if (sensorList[i][t->first].pathType.compare("sys") == 0)
                {
                    sensorTempFile.open(getSensorPath(t->second), std::ios::in);
                    if (sensorTempFile)
                    {
                        sensorTempFile >> sensorRealTemp;
                    }
                    sensorTempFile.close();
                }
                else if (sensorList[i][t->first].pathType.compare("dbus") == 0)
                {
                    /* code */
                }
                sensorMarginTemp = (sensorSpecTemp - sensorRealTemp);

                if (marginTemp == -1 || sensorMarginTemp < marginTemp)
                {
                    marginTemp = sensorMarginTemp;
                }
            }

            updateDbusMarginTemp(marginTemp);            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}
