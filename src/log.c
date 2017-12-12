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
 * @file log.c
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

#include "msg.h"
#include "stdint.h"
#include "log.h"
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

/* Private data */
static FILE *log_file;

/* Private functions */
void log_terminate(void *arg) {
    if (log_file != NULL) {
        /* Get time */
        time_t t;
        struct tm *ti;
        time(&t);
        ti = localtime(&t);

        /* Log */
        char *p = asctime(ti);
        p[strlen(p) - 1] = 0;
        fprintf(log_file, "%s\t", p);
        fprintf(log_file, "%s\t", log_task_strings_bbg[DEFS_TASK_LOG]);
        fprintf(log_file, "%s\t", log_level_strings[LOG_LEVEL_WARN]);
        fprintf(log_file, "'%s'\n", "Closing log thread gracefully");
        fflush(log_file);

        fclose(log_file);
        log_file = NULL;
    }
}

void *log_task(void *data) {
    pthread_cleanup_push(log_terminate, "log");

    mqd_t rxq = mq_open(msg_names[DEFS_TASK_LOG], O_RDONLY);
    msg_t rx;
    while(1) {
        mq_receive(rxq, (char *) &rx, MSG_SIZE+1, NULL);

        /* Log API */
        switch(rx.cmd) {
            case LOG_INIT:
                log_init(&rx);
                break;
            case LOG_LOG:
                log_log(&rx);
                break;
            case LOG_SETPATH:
                log_setpath(&rx);
                break;
            case LOG_KILL:
                log_kill(&rx);
                break;
            default:
                break;
        }
    }

    pthread_cleanup_pop(1);
}

uint8_t log_init(msg_t *rx) {
    if (log_file != NULL) {
        fclose(log_file);
        log_file = NULL;
    }

    if (rx->data[0] == 1) {
        log_file = fopen((char *)(rx->data+1), "w+");
    } else {
        log_file = fopen((char *)(rx->data+1), "a+");
    }
    if (log_file == NULL) {
        return LOG_ERR_FILE;
    }

    msg_t tx;
    LOG_FMT(DEFS_ID_BBG, DEFS_TASK_LOG, LOG_LEVEL_INFO, tx, "Initialized logger");
    msg_send(&tx);

    return LOG_SUCCESS;
}

uint8_t log_log(msg_t *rx) {

    if (log_file == NULL) {
        return LOG_ERR_UNINIT;
    }

    /* Get time */
    time_t t;
    struct tm *ti;
    time(&t); 
    ti = localtime(&t);

    /* Log */
    char *p = asctime(ti);
    p[strlen(p) - 1] = 0;
    fprintf(log_file, "%s\t", p);
    if (rx->devf == DEFS_ID_BBG) {
        fprintf(log_file, "%s\t", log_task_strings_bbg[rx->from]);
    } else if (rx->devf == DEFS_ID_TIVA) {
        fprintf(log_file, "%s\t", log_task_strings_tiva[rx->from]);
    }
    fprintf(log_file, "%s\t", log_level_strings[rx->data[0]]);
    fprintf(log_file, "'%s'\n", &rx->data[1]);
    fflush(log_file); 

    /* Also print */
    printf("%s\t", p);
    if (rx->devf == DEFS_ID_BBG) {
        printf("%s\t", log_task_strings_bbg[rx->from]);
    } else if (rx->devf == DEFS_ID_TIVA) {
        printf("%s\t", log_task_strings_tiva[rx->from]);
    }
    printf("%s\t", log_level_strings[rx->data[0]]);
    printf("'%s'\n", &rx->data[1]);

    return LOG_SUCCESS;

}

uint8_t log_setpath(msg_t *rx) {
    if (log_file != NULL) {
        fclose(log_file);
    }
    log_file = fopen((char *)rx->data, "w+");

    return LOG_SUCCESS;
}

uint8_t log_kill(msg_t *rx) {
    
    pthread_exit(0);

    return LOG_ERR_STUB;
}
