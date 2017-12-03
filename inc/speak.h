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
 * @file speak.h
 * @brief Speaker task
 * 
 * This task controls playback out of the speakers connected to the BBG.
 *
 * @author Ben Heberlein
 * @date Dec 2 2017
 * @version 1.0
 *
 */

#ifndef __SPEAK_H__
#define __SPEAK_H__

/**
 * @brief Error codes
 */
#define SPEAK_SUCCESS       0
#define SPEAK_ERR_STUB      0
#define SPEAK_ERR_UNKNOWN   0

/**
 * @brief Task API Definitions
 * ---------------------------
 */

/**
 * @brief Initialize speaker
 *
 * DATA none
 */
uint8_t speak_init(msg_t *rx);

/**
 * @brief Play sound from associated list
 *
 * DATA 1 byte of message ID
 */
uint8_t speak_speak(msg_t *rx);

/**
 * @brief Kill task gracefully
 *
 * DATA none
 */
uint8_t speak_kill(msg_t *rx);

/**
 * @brief Private functions
 * ------------------------
 */

#endif /*__SPEAK_H__*/
