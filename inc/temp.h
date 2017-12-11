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
 * @brief I2C and sensor macros
 */
#define TEMP_I2C_BUS  0
#define TEMP_I2C_ADDR 0x48
#define TEMP_REG_TEMP 0x00
#define TEMP_REG_CTRL 0x01
#define TEMP_REG_HIGH 0x02
#define TEMP_REG_LOW  0x03

#define TEMP_REG_CTRL_OS    (1<<15)
#define TEMP_REG_CTRL_R1    (1<<14)
#define TEMP_REG_CTRL_R0    (1<<13)
#define TEMP_REG_CTRL_F1    (1<<12)
#define TEMP_REG_CTRL_F0    (1<<11)
#define TEMP_REG_CTRL_POL   (1<<10)
#define TEMP_REG_CTRL_TM    (1<<9)
#define TEMP_REG_CTRL_SD    (1<<8)
#define TEMP_REG_CTRL_CR1   (1<<7)
#define TEMP_REG_CTRL_CR2   (1<<6)
#define TEMP_REG_CTRL_AL    (1<<5)
#define TEMP_REG_CTRL_EM    (1<<4)

/**
 * @brief Heartbeat time
 */
#define TEMP_HEARTBEAT_MS   100

/**
 * @brief Conversion rates
 */
#define TEMP_CONV_0_25HZ 0
#define TEMP_CONV_1HZ    1
#define TEMP_CONV_4HZ    2
#define TEMP_CONV_8Hz    3

/**
 * @brief Resolution per DN (C)
 */
#define TEMP_RES 0.0625

/**
 * @brief Initial period
 */
#define TEMP_PER 5000

/**
 * @brief Format strings
 */
static const char *temp_fmt_strings[] = {"Celcius", "Farenheit", "Kelvin"};

/**
 * @brief Task API Definitions
 * ---------------------------
 */

#define TEMP_INIT           0
#define TEMP_PERIOD         1
#define TEMP_SHUTDOWN       2
#define TEMP_WAKEUP         3
#define TEMP_KILL           4

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
uint8_t temp_kill(msg_t *rx);

/**
 * @brief Private functions
 * ------------------------
 */

/**
 * @brief Temp task
 */
void temp_task(void *p);

#endif /*__TEMP_H__*/
