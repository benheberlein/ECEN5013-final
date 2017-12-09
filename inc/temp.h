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
 * @file temp.h
 * @brief Temperature sensor task
 * 
 * This task is responsible for managin interactions with the TMP102 sensor.
 *
 * @author Ben Heberlein
 * @date Dec 2 2017
 * @version 1.0
 *
 */

#ifndef __TEMP_H__
#define __TEMP_H__

#include "msg.h"

/**
 * @brief Error codes 
 */
#define TEMP_SUCCESS      0
#define TEMP_ERR_STUB     126
#define TEMP_ERR_UNKNOWN  127

/**
 * @brief Task API Definitions
 * ---------------------------
 */

/**
 * @brief Temp init task
 *
 * DATA none
 */
uint8_t temp_init(msg_t *rx);

/**
 * @brief Shutdown temp sensor
 *
 * DATA none
 */
uint8_t temp_shutdown(msg_t *rx);

/**
 * @brief Wake up temp sensor
 *
 * DATA none
 */
uint8_t temp_wakeup(msg_t *rx);


/**
 * @brief Set the period for temperature check
 *
 * DATA 4 bytes of period in milliseconds
 */
uint8_t temp_period(msg_t *rx);

/**
 * @brief Kill temp task gracefully
 *
 * DATA none
 */
uint8_t temp_kil(msg_t *rx);

/**
 * @brief Private functions
 * ------------------------
 */

/**
 * @brief Temp task
 */
void temp_task(void *p);

#endif /*__TEMP_H__*/
