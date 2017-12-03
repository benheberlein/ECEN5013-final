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
 * @file log.h
 * @brief Log task
 * 
 * This task will log messages to a file. The messages can be of several levels
 * (LOG_LEVEL_ERROR, LOG_LEVEL_WARNING, LOG_LEVEL_INFO, LOG_LEVEL_DEBUG).
 *
 * @author Ben Heberlein
 * @date Dec 2 2017
 * @version 1.0
 *
 */

#ifndef __LOG_H__
#define __LOG_H__

/**
 * @brief Error codes
 */
#define LOG_SUCCESS     0
#define LOG_ERR_STUB    126
#define LOG_ERR_UNKNOWN 127

/**
 * @brief Task API Definitions
 * ---------------------------
 */

/**
 * @brief Initialize log
 *
 * DATA none
 */
uint8_t log_init(msg_t *rx);

/**
 * @brief Log to file
 *
 * DATA none
 */
uint8_t log_log(msg_t *rx);

/**
 * @brief Set new filepath
 *
 * DATA none
 */
uint8_t log_setpath(msg_t *rx);

/**
 * @brief Kill log task gracefully
 *
 * DATA none
 */
uint8_t log_kill(msg_t *rx);

/**
 * @brief Private functions
 * ------------------------
 */

#endif /*__LOG_H__*/
