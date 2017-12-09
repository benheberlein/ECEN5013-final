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

#ifndef __TIVA_H__
#define __TIVA_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

#include "msg.h"

/**
 * @brief Error codes
 */
#define TIVA_SUCCESS        0
#define TIVA_ERR_STUB       126
#define TIVA_ERR_UNKNOWN    127

/**
 * @brief MAC and IP info
 */ 
#define TIVA_MAC0 0x00
#define TIVA_MAC1 0x1a
#define TIVA_MAC2 0xb6
#define TIVA_MAC3 0x03
#define TIVA_MAC4 0x3a
#define TIVA_MAC5 0x04
#define TIVA_IP0 192
#define TIVA_IP1 168
#define TIVA_IP2 0
#define TIVA_IP3 35
#define TIVA_NETMASK0 255
#define TIVA_NETMASK1 255
#define TIVA_NETMASK2 255
#define TIVA_NETMASK3 0
#define TIVA_GATEWAY0 192
#define TIVA_GATEWAY1 168
#define TIVA_GATEWAY2 0
#define TIVA_GATEWAY3 1
#define TIVA_DNS0 192
#define TIVA_DNS1 168
#define TIVA_DNS2 0
#define TIVA_DNS3 1
#define TIVA_SERVER0 192
#define TIVA_SERVER1 168
#define TIVA_SERVER2 0
#define TIVA_SERVER3 13

/**
 * @brief Task API Definitions
 * ---------------------------
 */

/**
 * @brief TIVA heartbeat handler
 *
 * DATA none
 */
uint8_t tiva_heartbeat(msg_t *rx);

/**
 * @brief Exit TIVA program
 *
 * Data none
 */
uint8_t tiva_exit(msg_t *rx);

/**
 * @brief Private functions
 * ------------------------
 */

/**
 * @brief Main task
 */
void main_task(void *p);

/**
 * @brief Ping Hook
 */
void vApplicationPingReplyHook(ePingReplyStatus_t status, uint16_t id);

/**
 * @brief Network changed hook
 */
void vApplicationIPNetworkEventHook(eIPCallbackEvent_t event);

/**
 * @brief Open socket connection
 */
void tiva_open_socket(void);

/**
 * @brief Main
 */
int main(void);

#endif /*__TIVA_H__*/
