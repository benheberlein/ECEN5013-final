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
#include "msg.h"
#include "log.h"
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

/**
 * Private variables
 */
static const char *MAIN_USAGE = "One optional argument for log file name.\n";
static char *log_name;
static pthread_t main_tasks[DEFS_NUM_TASKS_BBG];

uint8_t bbg_heartbeat(msg_t *rx) {

    return BBG_ERR_STUB;	
}

uint8_t bbg_exit(msg_t *rx) {

    return BBG_ERR_STUB;
}

int main(int argc, char **argv) {

    /* Check if there are the right arguments */
    if (argc > 3) {
        printf("%s", MAIN_USAGE);
    }

    /* Initialize message system */
    msg_init();
        
    /* Open all threads */
//    if (pthread_create(&main_tasks[DEFS_TASK_SPEAK], NULL, speak_task, NULL)) {
//       return BBG_ERR_INIT;
//    }
    if (pthread_create(&main_tasks[DEFS_TASK_LOG], NULL, log_task, NULL)) {
       return BBG_ERR_INIT;
    }

    /* Start logger */
    if (argc == 2) {
        log_name = argv[1];
    } else {
        log_name = "project1.log";
    }

    msg_t tx;
    tx.devf = DEFS_ID_BBG;
    tx.from = DEFS_TASK_BBG;
    tx.devt = DEFS_ID_BBG;
    tx.to   = DEFS_TASK_LOG;
    tx.cmd = LOG_INIT;
    tx.data[0] = 1;
    strcpy((char *) (tx.data+1), log_name);
    msg_send(&tx);

    /* Initialize networking */

    /* Initialize heartbeat timers */

    /* Open queue and start main loop */
    mqd_t rxq = mq_open(msg_names[DEFS_TASK_BBG], O_RDONLY);
    msg_t rx;
    while(1) {
        mq_receive(rxq, (char *) &rx, MSG_SIZE+10, NULL);
        
        /* Send message over network */
        if (rx.devt != DEFS_ID_BBG) {
            /* TODO send over network */

        /* Route to another BBG task */
        } else if (rx.to != DEFS_TASK_BBG) {
            msg_route(&rx);    
        /* Handle message */
        } else {
            /* BBG API */            
            switch(rx.cmd) {
                case BBG_HEARTBEAT:
                    bbg_heartbeat(&rx);
                    break;
                case BBG_EXIT:
                    bbg_exit(&rx);
                    break;
                default:
                    break;
            }
        }
    }

    pthread_join(main_tasks[DEFS_TASK_SPEAK], NULL);
    pthread_join(main_tasks[DEFS_TASK_LOG], NULL);

    return BBG_SUCCESS;
}
