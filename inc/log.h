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

#include "msg.h"
#include "stdint.h"
#include "stdio.h"

/**
 * @brief Error codes
 */
#define LOG_SUCCESS     0
#define LOG_ERR_INIT    1
#define LOG_ERR_FILE    2
#define LOG_ERR_UNINIT  3
#define LOG_ERR_STUB    126
#define LOG_ERR_UNKNOWN 127

/**
 * @brief Log format macro
 *
 * Can format strings with sprintf. Can be used by requesting threads to format
 * data before requesting a log. Arguments are FROM, LOG_LEVEL, and the command
 * packet TX to put data into.
 */
#define LOG_FMT(df, fr, lvl, tx, ...) do { (tx).devf = (df); \
                                       (tx).from = (fr); \
                                       (tx).devt = 0; \
                                       (tx).to = 2; \
                                       (tx).cmd = LOG_LOG; \
                                       (tx).data[0] = (lvl); \
                                       sprintf((char *) &((tx).data[1]), __VA_ARGS__); \
                                      } while (0)

/**
 * @brief Log levels
 */
#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 3

/**
 * @brief Log string arrays
 */
static char *log_level_strings[] = {"DEBUG", "INFO", "WARN", "ERROR"};
static char *log_task_strings_bbg[] = {"BBG", "SPEAK", "LOG"};
static char *log_task_strings_tiva[] = {"TIVA", "TEMP", "GAS"};

/**
 * @brief Task API Definitions
 * ---------------------------
 */

#define LOG_INIT    0
#define LOG_LOG     1
#define LOG_SETPATH 2
#define LOG_ALIVE   3
#define LOG_KILL    4

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

/* Log terminate handler */
void log_terminate(void *arg);

/* Log task */
void *log_task(void *p);

#endif /*__LOG_H__*/
