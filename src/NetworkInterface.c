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
 * @file NetworkInterface.c
 * @brief Network interface port layer for FreeRTOS+TCP
 * 
 * This is the interface layer between FREERTOS+TCP and the ethernet driver.
 *
 * @author Ben Heberlein
 * @date Dec 7 2017
 * @version 1.0
 *
 */

/* Standard includes */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

/* FreeRTOS+TCP includes */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DNS.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"

/* Ethernet controller */
#include "eth.h"

TaskHandle_t xNetworkInterfaceProcessHandle = NULL;

/* Forward declaration */
void xNetworkInterfaceProcess(void *v);

/* Ethernet controller function definition */
void eth_process(void) {
    
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(xNetworkInterfaceProcessHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void vNetworkInterfaceProcess(void *v) {
    NetworkBufferDescriptor_t *pxBufferDescriptor;
    size_t xBytesRecieved;
    IPStackEvent_t xRxEvent;

    while(1) {

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        xBytesRecieved = eth_rx_size();

        if (xBytesRecieved > 0) {
    
            pxBufferDescriptor = pxGetNetworkBufferWithDescriptor( xBytesRecieved, 0 );
    
            if (pxBufferDescriptor != NULL) {
                pxBufferDescriptor->xDataLength = eth_rx(pxBufferDescriptor->pucEthernetBuffer, xBytesRecieved);

                xRxEvent.eEventType = eNetworkRxEvent;
                xRxEvent.pvData = (void *) pxBufferDescriptor;

                if (xSendEventStructToIPTask(&xRxEvent, 0) == pdFALSE) {
                    vReleaseNetworkBufferAndDescriptor( pxBufferDescriptor );
                    iptraceETHERNET_RX_EVENT_LOST();
                } else {
                    iptraceNETWORK_INTERFACE_RECEIVE();
                }
            } else {
                iptraceETHERNET_RX_EVENT_LOST();
            }
        }

    }

}

BaseType_t xNetworkInterfaceInitialise(void) {
    eth_init();
    xTaskCreate( vNetworkInterfaceProcess, "EMAC", configMINIMAL_STACK_SIZE*10, NULL, configMAX_PRIORITIES - 1, &xNetworkInterfaceProcessHandle );
    
    return pdPASS;
}

BaseType_t xNetworkInterfaceOutput(NetworkBufferDescriptor_t *const pxDescriptor, BaseType_t xReleaseAfterSend) {

    /* Transmit with ethernet driver */
    eth_tx(pxDescriptor->pucEthernetBuffer, pxDescriptor->xDataLength);

    /* Call trace function */
    iptraceNETWORK_INTERFACE_TRANSMIT();
    
    /* Release FreeRTOS ethernet buffer */
    if (xReleaseAfterSend != pdFALSE) {
        vReleaseNetworkBufferAndDescriptor( pxDescriptor );
    }

    return pdTRUE;
}
