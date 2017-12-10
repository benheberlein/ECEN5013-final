/******************************************************************************
* Copyright (C) 2017 by Ben Heberlein
*
* Redistribution, modification or use of this software in source or binary
* forms is permitted as long as the files maintain this copyright. This file
* was created for the University of Colorado Boulder course Advanced Practical
* Embedded Software Development. Ben Heberlein and the University of Colorado 
* are not liable for any misuse of this material.
*
*******************************************************************************/
/**
 * @file temp.c
 * @brief Temperature sensor task
 * 
 * This task is responsible for managin interactions with the TMP102 sensor.
 *
 * @author Ben Heberlein
 * @date Dec 2 2017
 * @version 1.0
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/* Tivaware includes */
#include "inc/hw_emac.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_i2c.h"
#include "inc/hw_sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/can.h"
#include "driverlib/emac.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/i2c.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "list.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

#include "defs.h"
#include "msg.h"
#include "temp.h"
#include "tiva.h"
#include "log.h"

static TimerHandle_t temp_timer;
static TickType_t temp_per = pdMS_TO_TICKS(1000);

uint16_t temp_i2c_read(uint8_t address) {

    return 0;
}


void temp_i2c_write(uint16_t data, uint8_t address) {

    return;
}

uint8_t temp_i2c_init(void) {

    /* Enable I2C and reset*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

    /* Wait for periph */
    while ((!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0))) {}

    /* Enable GPIO */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    /* GPIO config */
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    /* Init master */
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

    return TEMP_SUCCESS;
}

void temp_read(void *p) {

    /* TODO Send temperature data to LOG whenever we wake up */
    msg_t tx;
    LOG_FMT(DEFS_ID_TIVA, DEFS_TASK_TEMP, LOG_LEVEL_INFO, tx, "Checked temperature");
    msg_send(&tx);

    /* TODO Send alarm data if we are out of range */

}

void temp_task(void *p) {

    /* Set up read timer */
    temp_timer =  xTimerCreate ("TEMP TIMER",
                                temp_per,
                                pdTRUE,
                                (void *) 0,
                                temp_read);
    xTimerStart(temp_timer, portMAX_DELAY);

    /* Heartbeat packet */
    msg_t tx;
    tx.devf = DEFS_ID_TIVA;
    tx.from = DEFS_TASK_TEMP;
    tx.devt = DEFS_ID_TIVA;
    tx.to   = DEFS_TASK_TIVA;
    tx.cmd  = TIVA_HEARTBEAT;
    tx.data[0] = 0;

    msg_t rx;
    while(1) {
        /* Wait for message */
        if (!xQueueReceive(msg_queues[DEFS_TASK_TIVA], &rx, TEMP_HEARTBEAT_MS)) {
            
        /* Handle message */
        } else {
            /* TODO Temp API */
            switch(rx.cmd) {
                case TEMP_INIT:
                    temp_init(&rx);
                    break;
                case TEMP_SHUTDOWN:
                    temp_shutdown(&rx);
                    break;
                case TEMP_WAKEUP:
                    temp_wakeup(&rx);
                    break;
                case TEMP_PERIOD:
                    temp_period(&rx);
                case TEMP_KILL:
                    temp_kill(&rx);
                    break;
                default:
                    break;

            }
        }

        /* Send heartbeat every time we wake up (max TEMP_HEARTBEAT_MS ms) */
        msg_send(&tx);
    }
}

uint8_t temp_init(msg_t *rx) {

    return TEMP_ERR_STUB;

}

uint8_t temp_shutdown(msg_t *rx) {

    return TEMP_ERR_STUB;
}

uint8_t temp_wakeup(msg_t *rx) {

    return TEMP_ERR_STUB;
}

uint8_t temp_period(msg_t *rx) {

    return TEMP_ERR_STUB;
}

uint8_t temp_kill(msg_t *rx) {

    return TEMP_ERR_STUB;
}
