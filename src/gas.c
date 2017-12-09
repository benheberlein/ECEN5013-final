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
 * @file gas.c
 * @brief The gas sensor task
 * 
 * This task is responsible for setting up and maintaining the gas sensor.
 *
 * @author Ben Heberlein
 * @date Dec 2 2017
 * @version 1.0
 *
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

#include "gas.h"
#include "stdint.h"
#include "msg.h"

void gas_task(void *p) {

    while(1) {
        vTaskDelay(1000);
    }

}

uint8_t gas_init(msg_t *rx) {

    return GAS_ERR_STUB;
}

uint8_t gas_shutdown(msg_t *rx) {

    return GAS_ERR_STUB;
}

uint8_t gas_wakeup(msg_t *rx) {

    return GAS_ERR_STUB;
}

uint8_t gas_period(msg_t *rx) {

    return GAS_ERR_STUB;
}

uint8_t gas_kill(msg_t *rx) {

    return GAS_ERR_STUB;
}
