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

#include "wrd-battery-gauge/BatteryGauge.h"

#include "wrd-battery-gauge/BatteryGaugeBase.h"
#include "core-util/CriticalSectionLock.h"

#include <queue>
#include <list>

#if YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_BATTERY_PRESENT
#include "wrd-battery-gauge/BatteryGaugeImplementation.h"
#else
#include "wrd-battery-gauge/BatteryGaugeNotPresent.h"
#endif

#if 0
#include "swo/swo.h"
#define printf(...) { swoprintf(__VA_ARGS__); }
#else
#define printf(...)
#endif

namespace BatteryGauge
{
// private
#if YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_BATTERY_PRESENT
    static BatteryGaugeImplementation realGauge;
#else
    static BatteryGaugeNotPresent realGauge;
#endif
    static BatteryGaugeBase& gauge = realGauge;

    static bool doInit = true;
    static int16_t voltage = -1;
    static int16_t capacity = -1;
    static std::queue<BatteryGauge::transaction_t> sendQueue;
    static std::list<FunctionPointer1<void, int16_t> > updateList;

    static void processQueueTask(void);
    static void sendDoneTask(uint16_t);

// public
    void init(void)
    {
        printf("battery: init\r\n");

        doInit = false;

        // get voltage
        transaction_t action1;
        action1.type = BatteryVoltage;

        // get capacity
        transaction_t action2;
        action2.type = BatteryCapacity;

        // protect queue access
        {
            CriticalSectionLock lock;

            sendQueue.push(action1);
            sendQueue.push(action2);
        }

        minar::Scheduler::postCallback(processQueueTask);
    }

    void setPerMilleChangeCallbackTask(FunctionPointer1<void, int16_t> callback)
    {
        /* Make sure module is initialized. */
        if (doInit)
        {
            init();
        }

        /* Check if function pointer is already in list. */
        bool notFound = true;

        std::list<FunctionPointer1<void, int16_t> >::iterator iter = updateList.begin();

        for ( ; iter != updateList.end(); ++iter)
        {
            if (*iter == callback)
            {
                notFound = false;
                break;
            }
        }

        /* only insert function pointer in list if not there already. */
        if (notFound)
        {
            printf("battery: insert\r\n");

            updateList.push_back(callback);
        }
    }

    void cancelCallbackTask(FunctionPointer1<void, int16_t> callback)
    {
        /* Make sure module is initialized. */
        if (doInit)
        {
            init();
        }

        /* Remove function pointer from list. */
        std::list<FunctionPointer1<void, int16_t> >::iterator iter = updateList.begin();

        for ( ; iter != updateList.end(); ++iter)
        {
            if (*iter == callback)
            {
                printf("battery: remove\r\n");

                updateList.erase(iter);
                break;
            }
        }
    }

    int16_t getPerMille(void)
    {
        /* Make sure module is initialized. */
        if (doInit)
        {
            init();
        }

        return capacity;
    }

    int16_t getMilliVolt(void)
    {
        /* Make sure module is initialized. */
        if (doInit)
        {
            init();
        }

        return voltage;
    }

    uint32_t getTotalCapacity(void)
    {
        /* Make sure module is initialized. */
        if (doInit)
        {
            init();
        }

#if YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_BATTERY_PRESENT
        return YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_BATTERY_CAPACITY;
#else
        return -1;
#endif
    }

    uint32_t getAverageCurrent(void)
    {
        /* Make sure module is initialized. */
        if (doInit)
        {
            init();
        }

#if YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_BATTERY_PRESENT
        return YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_BATTERY_ESTIMATED_CURRENT;
#else
        return -1;
#endif
    }

// private
    /*
        Process one transcation at a time from the queue.
    */
    static void processQueueTask()
    {
        // only process if queue is not empty
        if (sendQueue.size() > 0)
        {
            BatteryGauge::transaction_t action = sendQueue.front();

            if (action.type == BatteryGauge::BatteryCapacity)
            {
                gauge.getPerMille(sendDoneTask);
            }
            else if (action.type == BatteryGauge::BatteryVoltage)
            {
                gauge.getMilliVolt(sendDoneTask);
            }
        }
    }

    /*
        Cleanup after block is send. Remove transaction from queue.
    */
    static void sendDoneTask(uint16_t value)
    {
        BatteryGauge::transaction_t action = sendQueue.front();
        sendQueue.pop();

        if (action.type == BatteryGauge::BatteryCapacity)
        {
            /* Notify subscribers if capacity has changed. */
            if (capacity != value)
            {
                std::list<FunctionPointer1<void, int16_t> >::iterator iter = updateList.begin();

                for ( ; iter != updateList.end(); ++iter)
                {
                    printf("battery: callback\r\n");

                    /* Call callback immediately and not through the scheduler
                       in order to avoid the race condition where a callback
                       is cancelled after it has been queued up.
                    */
                    iter->call(value);
                }

                // update capacity
                capacity = value;
            }
        }
        else if (action.type == BatteryGauge::BatteryVoltage)
        {
            voltage = value;
        }

        // schedule next transcation in queue to be processed
        minar::Scheduler::postCallback(processQueueTask);
    }
}
