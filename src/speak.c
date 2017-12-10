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
 * @file speak.c
 * @brief Speaker task
 * 
 * This task controls playback out of the speakers connected to the BBG.
 *
 * @author Ben Heberlein
 * @date Dec 2 2017
 * @version 1.0
 *
 */

#include "msg.h"
#include "speak.h"
#include "bbg.h"

uint8_t speak_init(msg_t *rx) {
    
    return SPEAK_ERR_STUB;
}

uint8_t speak_speak(msg_t *rx) {

    return SPEAK_ERR_STUB;
}

uint8_t speak_kill(msg_t *rx) {

    return SPEAK_ERR_STUB;
}
