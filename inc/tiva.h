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

/**
 * @brief Error codes
 */
#define TIVA_SUCCESS        0
#define TIVA_ERR_STUB       126
#define TIVA_ERR_UNKNOWN    127

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

int maint(void);

#endif /*__TIVA_H__*/
