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
 * @file bbg.c
 * @brief Beagle Bone Green main file
 * 
 * This file contains the main task for the Beagle Bone Green.
 *
 * @author Ben Heberlein
 * @date Dec 2 2017
 * @version 1.0
 *
 */

#include "bbg.h"

uint8_t bbg_heartbeat(msg_t *rx) {

    return BBG_ERR_STUB;	
}

uint8_t bbg_exit(msg_t *rx) {

    return BBG_ERR_STUB;
}

int main(void) {

    return 0;
}
