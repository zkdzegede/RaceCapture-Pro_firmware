/*
 * Race Capture Pro Firmware
 *
 * Copyright (C) 2015 Autosport Labs
 *
 * This file is part of the Race Capture Pro fimrware suite
 *
 * This is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should
 * have received a copy of the GNU General Public License along with
 * this code. If not, see <http://www.gnu.org/licenses/>.
 */

#include "FreeRTOS.h"
#include "imu.h"
#include "loggerConfig.h"
#include "mod_string.h"
#include "printk.h"
#include "slip_angle.h"
#include "slip_angle_task.h"
#include "task.h"

#include <stdbool.h>

#define SLIP_ANGLE_STACK_SIZE	64
#define SLIP_ANGLE_TASK_NAME	"Slip Angle Task"
#define SLIP_ANGLE_MS_DELAY	10
#define LOG_PFX	"[slip_angle] "

/* Ensure value is cleanly divisible.  Needed for sanity. */
#if SLIP_ANGLE_MS_DELAY % MS_PER_TICK != 0
 #error "SLIP_ANGLE_MS_DELAY must be divisible by MS_PER_TICK"
#endif

static ImuConfig *yaw_cfg;
static float yaw_delta;
static const ChannelConfig yaw_chan_cfg = DEFAULT_GYRO_YAW_CONFIG;
static const portTickType tick_increment = SLIP_ANGLE_MS_DELAY / MS_PER_TICK;

static ImuConfig* find_yaw_channel()
{
        ImuConfig *ic = getWorkingLoggerConfig()->ImuConfigs;

        for(int i = 0; i < CONFIG_IMU_CHANNELS; ++i) {
                if (0 == strcmp(ic[i].cfg.label, yaw_chan_cfg.label))
                        return ic + i;
        }

        return NULL;
}

void init_slip_angle_yaw_cfg(void)
{
        yaw_cfg = find_yaw_channel();
        if (NULL == yaw_cfg)
                pr_warning_str_msg("Unable to locate channel: ",
                                   yaw_chan_cfg.label);

        reset_yaw_delta();
}

void reset_yaw_delta(void)
{
        yaw_delta = 0;
}

float get_yaw_delta(void)
{
        return yaw_delta;
}

static void update_yaw_delta(void)
{
        if (NULL == yaw_cfg)
                return;

        const float value = imu_read_value(yaw_cfg->physicalChannel, yaw_cfg);
        yaw_delta += value * SLIP_ANGLE_MS_DELAY / 1000;
}

static void slip_angle_task(void *param)
{
        portTickType next_warn_tick = 0;
        init_slip_angle_yaw_cfg();

        /*
         * Calculate when we wake next at the start of the loop because
         * then it doesn't matter how long the update_yaw_delta task
         * takes so long as it completes before it is supposed to be
         * woken again.  In that case the task is too slow and will
         * produce inaccurate results due to the math being skewed.
         * To ensure that doesn't happen we add a check for that in the
         * code.
         */
        for(;;) {
                /* Can't use const qualifier here b/c of vTaskDelayUntil */
                portTickType wake_tick = xTaskGetTickCount();
                const portTickType next_wake_tick = wake_tick + tick_increment;

                update_yaw_delta();

                const portTickType tick = xTaskGetTickCount();
                if (tick > next_wake_tick && tick > next_warn_tick) {
                        pr_warning(LOG_PFX "update_yaw_delta took too long.  "
                                   "Expect errors on yaw_delta.\r\n");
                        /* So we don't flood the log file */
                        next_warn_tick = tick + (1000 / MS_PER_TICK);
                }

                vTaskDelayUntil(&wake_tick, tick_increment);
        }
}

bool start_slip_angle_task(const int priority)
{
        const signed portCHAR* task_name =
                (signed portCHAR*) SLIP_ANGLE_TASK_NAME;

        const signed portBASE_TYPE result =
                xTaskCreate(slip_angle_task, task_name,
                            SLIP_ANGLE_STACK_SIZE, NULL, priority, NULL);

        return pdPASS == result;
}
