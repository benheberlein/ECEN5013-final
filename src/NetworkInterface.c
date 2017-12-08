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

/* FreeRTOS+TCP includes */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DNS.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"

BaseType_t xNetworkInterfaceInitialise(void) {
    BaseType_t b;
    return b;
}

BaseType_t xNetworkInterfaceOutput(NetworkBufferDescriptor_t *const pxDescriptor, BaseType_t xReleaseAfterSend) {
    
    BaseType_t b;
    return b;

}
