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
 * @file gas.h
 * @brief The gas sensor task
 * 
 * This task is responsible for setting up and maintaining the gas sensor.
 *
 * @author Ben Heberlein
 * @date Dec 2 2017
 * @version 1.0
 *
 */

#ifndef __GAS_H__
#define __GAS_H__

/**
 * @brief Error codes 
 */
#define GAS_SUCCESS     0
#define GAS_ERR_STUB    126
#define GAS_ERR_UKNOWN  127

/**
 * @brief Task API Definitions
 * ---------------------------
 */

/**
 * @brief Gas init task
 *
 * DATA none
 */
uint8_t gas_init(msg_t *rx);

/**
 * @brief Shutdown gas sensor
 *
 * DATA	none
 */
uint8_t gas_shutdown(msg_t *rx);

/**
 * @brief Wake up gas sensor
 *
 * DATA	none
 */
uint8_t gas_wakeup(msg_t *rx);

/**
 * @brief Set the period for air quality check
 *
 * DATA	4 bytes of period in milliseconds
 */
uint8_t gas_period(msg_t *rx);

/**
 * @brief Kill gas task gracefully
 *
 * DATA none
 */
uint8_t gas_kill(msg_t *rx);

/**
 * @brief Private functions
 * ------------------------
 */

#endif /*__GAS_H__*/
