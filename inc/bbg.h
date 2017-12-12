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
 * @file bbg.h
 * @brief Beagle Bone Green main file
 * 
 * This file contains the main task for the Beagle Bone Green.
 *
 * @author Ben Heberlein
 * @date Dec 2 2017
 * @version 1.0
 *
 */

#ifndef __BBG_H__
#define __BBG_H__

#include "stdint.h"
#include "msg.h"

/**
 * @brief Error codes 
 */
#define BBG_SUCCESS     0
#define BBG_ERR_INIT    1
#define BBG_ERR_STUB    126
#define BBG_ERR_UKNOWN  127

/**
 * @brief Heartbeat check in ns
 */
#define BBG_HEARTBEAT_PERIOD    300000000

/**
 * @brief Task API Definitions
 * ---------------------------
 */

#define BBG_HEARTBEAT 0
#define BBG_EXIT 1

/**
 * @brief BBG heartbeat handler
 *
 * DATA none
 */ 
uint8_t bbg_heartbeat(msg_t *rx);

/**
 * @brief Exit BBG program
 *
 * DATA none
 */
uint8_t bbg_exit(msg_t *rx);

/**
 * @brief Private functions
 * ------------------------
 */

/**
 * @brief Main task 
 */
int main(int argc, char **argv);

#endif /*__BBG_H__*/
