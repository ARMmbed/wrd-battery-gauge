/*
 * Copyright (c) 2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __WRD_BATTERY_GAUGE_BASE_H__
#define __WRD_BATTERY_GAUGE_BASE_H__

#include "mbed-drivers/mbed.h"
#include "core-util/FunctionPointer.h"

using namespace mbed::util;

class BatteryGaugeBase
{
protected:
    BatteryGaugeBase(void) { }

public:
    virtual ~BatteryGaugeBase(void) { }

    /**
     * @brief Get battery level in permille.
     *
     * @param callback Function to be called with the battery level as parameter.
     */
    virtual void getPerMille(FunctionPointer1<void, uint16_t> callback) = 0;

    /**
     * @brief Get battery voltage in milli volt.
     *
     * @param callback Function to be called with the battery voltage as parameter.
     */
    virtual void getMilliVolt(FunctionPointer1<void, uint16_t> callback) = 0;

    /**
     * @brief Set callback function for when the battery level changes.
     * @details The callback function is be called every time the battery
     *          level changes approximately +/- 1%. The exact value may depend
     *          upon the granularity supported by the hardware and what offers
     *          the best tradeof between power consumption and precision.
     *
     * @param callback Function to be called when the battery level has changed
     *                 with the new level in permille as the parameter.
     */
    virtual void setPerMilleChangeCallback(FunctionPointer1<void, uint16_t> callback) = 0;

    /**
     * @brief Cancel callback function set up by setPerMilleChangeCallback.
     *
     * @param callback Function to be removed from callback list.
     */
    virtual void cancelCallback(FunctionPointer1<void, uint16_t> callback) = 0;
};

#endif // __WRD_BATTERY_GAUGE_BASE_H__
