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
#include <string.h>
#include <math.h>

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
static TickType_t temp_per = pdMS_TO_TICKS(TEMP_PER);

/* Reverse from K&R C Programming Language */
static void reverse(char s[]) {
    int i, j;
    char c;
 
    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* Itoa from K&R C Programming Language */
static void itoa(int n, char s[]) {
    int i, sign;
 
    if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

float temp_conv(uint16_t data) {
    /* Get temperature */
    data = data >> 4;
    float c = 0;

    /* Convert to deg C */
    if (data & 0x0800) {
        data = ((~data) & 0x0fff) + 1;
        c = data * -TEMP_RES;
    } else {
        c = data * TEMP_RES;
    }
    return c;

}

uint16_t temp_i2c_read(uint8_t address) {

    uint8_t data_low = 0;
    uint8_t data_high = 0;

    I2CMasterSlaveAddrSet(I2C0_BASE, 0x48, false);
    I2CMasterDataPut(I2C0_BASE, address);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    while(I2CMasterBusy(I2C0_BASE)) {}
    I2CMasterSlaveAddrSet(I2C0_BASE, 0x48, true);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
    while(I2CMasterBusy(I2C0_BASE)) {}
    data_high = I2CMasterDataGet(I2C0_BASE);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    while(I2CMasterBusy(I2C0_BASE)) {}
    data_low = I2CMasterDataGet(I2C0_BASE);

    return (data_high << 8) | data_low;
}


void temp_i2c_write(uint16_t data, uint8_t address) {

    I2CMasterSlaveAddrSet(I2C0_BASE, 0x48, false);
    I2CMasterDataPut(I2C0_BASE, address);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    while(I2CMasterBusy(I2C0_BASE)) {}
    I2CMasterDataPut(I2C0_BASE, data >> 8);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
    while(I2CMasterBusy(I2C0_BASE)) {}
    I2CMasterDataPut(I2C0_BASE, data & 0xff);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    while(I2CMasterBusy(I2C0_BASE)) {}

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

    /* Wait for I2C 0 */
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0)) {}

    /* Init master */
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

    return TEMP_SUCCESS;
}

void temp_read(void *p) {

    float temperature = temp_conv(temp_i2c_read(TEMP_REG_TEMP));

    /* Send temperature data to LOG whenever we wake up */
    msg_t tx;
    /* Use itoa because sprintf won't build on TIVA */
    char buf[10];
    itoa(round(temperature), buf);
    char str[50] = "Temperature is ";
    strcat(str, buf);
    strcat(str, " degrees C");
    tx.devf = DEFS_ID_TIVA;
    tx.from = DEFS_TASK_TEMP;
    tx.devt = DEFS_ID_BBG;
    tx.to   = DEFS_TASK_LOG;
    tx.cmd  = LOG_LOG;
    tx.data[0] = LOG_LEVEL_INFO;
    memcpy((tx.data+1), str, 50);
    msg_send(&tx);

    /* Send alarm data if we are out of range */
    if (temperature < 26) {
        tx.to = DEFS_TASK_SPEAK;
        memcpy(tx.data, str, 50);
        msg_send(&tx);
    } else {
        tx.to = DEFS_TASK_SPEAK;
        char str2[50] = "Something is on fire, please evacuate!";
        memcpy(tx.data, str2, 50);
        msg_send(&tx);
    }

}

void temp_task(void *p) {
    /* Initialize self */
    temp_init(NULL);        

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
        if (!xQueueReceive(msg_queues[DEFS_TASK_TEMP], &rx, TEMP_HEARTBEAT_MS)) {
            
        /* Handle message */
        } else {
            /* Temp API */
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

    temp_i2c_init();

    return TEMP_SUCCESS;

}

uint8_t temp_shutdown(msg_t *rx) {
    uint16_t data = temp_i2c_read(TEMP_REG_CTRL);
    data |= TEMP_REG_CTRL_SD;

    temp_i2c_write(TEMP_REG_CTRL, data);

    return TEMP_SUCCESS;
}

uint8_t temp_wakeup(msg_t *rx) {
    uint16_t data = temp_i2c_read(TEMP_REG_CTRL);
    data &= ~TEMP_REG_CTRL_SD;

    temp_i2c_write(TEMP_REG_CTRL, data);

    return TEMP_SUCCESS;
}

uint8_t temp_period(msg_t *rx) {
    temp_per = *((uint32_t *) rx->data);

    return TEMP_SUCCESS;
}

uint8_t temp_kill(msg_t *rx) {
    vTaskDelete(NULL);

    return TEMP_SUCCESS;
}
