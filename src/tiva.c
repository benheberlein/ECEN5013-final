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
 * @file tiva.c
 * @brief Main task for the TIVA board
 * 
 * This is the main task for the TIVA board. It handles the creation of other
 * FreeRTOS tasks and the socket communication to the BBG.
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

#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/can.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

#include "tiva.h"
#include "temp.h"
#include "gas.h"
#include "msg.h"

/* MAC address */
static const uint8_t tiva_mac[6] = {TIVA_MAC0, 
                                    TIVA_MAC1,
                                    TIVA_MAC2,
                                    TIVA_MAC3,
                                    TIVA_MAC4,
                                    TIVA_MAC5};

/* IP Address */
static const uint8_t tiva_ip[4] = {TIVA_IP0, TIVA_IP1, TIVA_IP2, TIVA_IP3};
static const uint8_t tiva_netmask[4] = {TIVA_NETMASK0, TIVA_NETMASK1, TIVA_NETMASK2, TIVA_NETMASK3};
static const uint8_t tiva_gateway[4] = {TIVA_GATEWAY0, TIVA_GATEWAY1, TIVA_GATEWAY2, TIVA_GATEWAY3};
static const uint8_t tiva_dns[4] = {TIVA_DNS0, TIVA_DNS1, TIVA_DNS2, TIVA_DNS3};

/* Initialization flag (small state machine on network init) */
static uint8_t tiva_init_flag = 0;

/* Client socket */
Socket_t tiva_socket;

/* Ping hook for FreeRTOS+TCP */
void vApplicationPingReplyHook(ePingReplyStatus_t status, uint16_t id) {
    return;
}

/* Network changed hook for FreeRTOS+TCP */
void vApplicationIPNetworkEventHook(eIPCallbackEvent_t event) { 
    if (event == eNetworkUp) {

        /* Turn on LED */
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);

        if (tiva_init_flag == 0) {
            tiva_init_flag = 1;
        }

    } else {
        /* Turn off LED */
 //       tiva_init_flag = 0;
    }
}

void tiva_open_socket(void) {
    static const TickType_t timeOut = pdMS_TO_TICKS(5000);
    BaseType_t bytes_sent = 0;
    tiva_socket = FreeRTOS_socket(FREERTOS_AF_INET,
                                    FREERTOS_SOCK_STREAM,
                                    FREERTOS_IPPROTO_TCP);
    configASSERT(tiva_socket != FREERTOS_INVALID_SOCKET);

    FreeRTOS_bind(tiva_socket, NULL, sizeof(struct freertos_sockaddr));

    FreeRTOS_setsockopt(tiva_socket,
                        0,
                        FREERTOS_SO_RCVTIMEO,
                        &timeOut,
                        sizeof(timeOut));

    FreeRTOS_setsockopt(tiva_socket,
                        0,
                        FREERTOS_SO_SNDTIMEO,
                        &timeOut,
                        sizeof(timeOut));
    
    struct freertos_sockaddr server;
    server.sin_addr = FreeRTOS_inet_addr_quick(192, 168, 0, 13);
    server.sin_port = FreeRTOS_htons(4000);

    int ret = -1;
    while(ret) {
        ret = FreeRTOS_connect(tiva_socket, &server, sizeof(struct freertos_sockaddr));
    }

    if (ret == 0) {
        char str[32] = "Hello world!\n";
        bytes_sent = FreeRTOS_send(tiva_socket, str, 32, 0);
    }

    return;

}

void main_task(void *p) {

    /* Initialize GPIO LED */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {}
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x0);

    /* Initialize TCP stack */
    FreeRTOS_IPInit(tiva_ip, tiva_netmask, tiva_gateway, tiva_dns, tiva_mac);

    while(1) {
        if (tiva_init_flag == 1) {
            tiva_init_flag = 2;
            tiva_open_socket();
        }
    }
}

int main() {

    uint32_t clock_rate;
    clock_rate = SysCtlClockFreqSet(
        (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
        SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
        SYSTEM_CLOCK);

    /* Create tasks */
//    xTaskCreate(temp_task, (const portCHAR*)"TEMP", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
//    xTaskCreate(gas_task, (const portCHAR*)"GAS", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(main_task, (const portCHAR*)"MAIN", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    /* Start Scheduler */
    vTaskStartScheduler();

    while(1) {}

    return 0;

}

uint8_t tiva_heartbeat(msg_t *rx) {

    return TIVA_ERR_STUB;
}

uint8_t tiva_exit(msg_t *rx) {

    return TIVA_ERR_STUB;

}
