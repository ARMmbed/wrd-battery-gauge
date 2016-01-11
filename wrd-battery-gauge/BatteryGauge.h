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

#ifndef __WRD_BATTERY_GAUGE_H__
#define __WRD_BATTERY_GAUGE_H__

#include "mbed-drivers/mbed.h"

#include "core-util/SharedPointer.h"
#include "core-util/FunctionPointer.h"

using namespace mbed::util;

namespace BatteryGauge
{
    typedef enum {
        BatteryCapacity,
        BatteryVoltage
    } type_t;

    typedef struct {
        type_t type;
    } transaction_t;

    /**
     * @brief Get battery level in permille.
     *
     * @return Battery level in permille.
     */
    int16_t getPerMille(void);

    /**
     * @brief Get battery voltage in milli volt.
     *
     * @return Battery voltage in milli volt.
     */
    int16_t getMilliVolt(void);

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
    void setPerMilleChangeCallbackTask(FunctionPointer1<void, int16_t> callback);

    /**
     * @brief Set callback function for when the battery level changes.
     * @details The callback function is be called every time the battery
     *          level changes approximately +/- 1%. The exact value may depend
     *          upon the granularity supported by the hardware and what offers
     *          the best tradeof between power consumption and precision.
     *
     * @param object Object containing calback member function.
     * @param member Function to be called when the battery level has changed
     *               with the new level in permille as the parameter.
     */
    template <typename T>
    void setPerMilleChangeCallbackTask(T* object, void (T::*member)(int16_t))
    {
        FunctionPointer1<void, int16_t> callback(object, member);

        setPerMilleChangeCallbackTask(callback);
    }

    void cancelCallbackTask(FunctionPointer1<void, int16_t> callback);

    template <typename T>
    void cancelCallbackTask(T* object, void (T::*member)(int16_t))
    {
        FunctionPointer1<void, int16_t> callback(object, member);

        cancelCallbackTask(callback);
    }

    uint32_t getTotalCapacity(void);
    uint32_t getAverageCurrent(void);
}

#if 0
class BatteryGauge
{
public:
    /**
     * @brief Battery Battery gauge constructor.
     */
    BatteryGauge(void)
        :   gauge(SharedPointer<BatteryGaugeBase>(new BatteryGaugeImplementation()))
    {}


private:
    SharedPointer<BatteryGaugeBase> gauge;
};
#endif

#endif // __WRD_BATTERY_GAUGE_H__
