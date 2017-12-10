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
#include "stdlib.h"
#include "log.h"
#include <pthread.h>
#include <string.h>

void speak_terminate(void *arg) {
    /* Log termination */
    msg_t tx;
    LOG_FMT(DEFS_ID_BBG, DEFS_TASK_SPEAK, LOG_LEVEL_WARN, tx, "Terminating speak task");
    msg_send(&tx);
}

void *speak_task(void *data) {
    pthread_cleanup_push(speak_terminate, "speak");

    mqd_t rxq = mq_open(msg_names[DEFS_TASK_SPEAK], O_RDONLY);
    msg_t rx;
    while(1) {
        mq_receive(rxq, (char *) &rx, MSG_SIZE+1, NULL);

        /* Speak API */
        switch(rx.cmd) {
            case SPEAK_INIT:
                speak_init(&rx);
                break;
            case SPEAK_SPEAK:
                speak_speak(&rx);
                break;
            case LOG_KILL:
                speak_kill(&rx);
                break;
            default:
                break;
        }
    }

    pthread_cleanup_pop(1);
}

uint8_t speak_init(msg_t *rx) {
    /* No initialization needed */

    msg_t tx;
    LOG_FMT(DEFS_ID_BBG, DEFS_TASK_SPEAK, LOG_LEVEL_INFO, tx, "Initialized speak task");
    msg_send(&tx);

    return SPEAK_SUCCESS;
}

uint8_t speak_speak(msg_t *rx) {

    char str[256] = {"flite -t '"};
    strcat(str, (const char *) rx->data);
    strcat(str, "'");

    system(str);

    return SPEAK_SUCCESS;
}

uint8_t speak_kill(msg_t *rx) {

    pthread_exit(0);

    return SPEAK_ERR_STUB;
}
