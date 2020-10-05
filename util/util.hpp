#pragma once

#include <map>
#include <vector>
#include <string>

#include "conf.hpp"

/**
 * Determine sku type.
 *
 * @return Sku type of the system.
 */
int getSkuNum();

/**
 * Get sensor temp from dbus.
 *
 * @param[in] sensorDbusPath - sensor dbus path.
 * @return Sensor temp.
 */
int getSensorDbusTemp(std::string sensorDbusPath);

/**
 * Get spec temp.
 *
 * @param[in] config - sensor config.
 * @return Spec temp of the sensor.
 */
int getSpecTemp(struct conf::sensorConfig config);

/**
 * Calculate margin offset value.
 *
 * @param[in] setPoint - zone setpoint.
 * @param[in] scalar - sensor scalar.
 * @param[in] maxTemp - sensor max temp.
 * @param[in] targetTemp - sensor target temp.
 * @param[in] targetOffset - sensor target temp offset.
 * @return Margin offset.
 */
int calOffsetValue(int setPoint, double scalar, int maxTemp, int targetTemp, int targetOffset = 0);

/**
 * Get dbus service name.
 *
 * @param[in] dbusPath - sensor dbus path.
 * @return Dbus service name.
 */
std::string getService(const std::string dbusPath);

/**
 * Update margin temp of zone zoneNum.
 *
 * @param[in] zoneNum - zone number.
 * @param[in] marginTemp - margin temp.
 */
void updateDbusMarginTemp(int zoneNum, int64_t marginTemp, std::string targetpath);

/**
 * Calculate and update margin temp every second.
 *
 * @param[in] skuConfig - sku config.
 * @param[in] sensorConfig - sensor config.
 */
void updateMarginTempLoop(
    conf::skuConfig skuConfig,
    std::map<std::string, struct conf::sensorConfig> sensorConfig);
