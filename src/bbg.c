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
#include "speak.h"
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Private variables
 */
static const char *MAIN_USAGE = "One optional argument for log file name.\n";
static char *log_name;
static pthread_t main_tasks[DEFS_NUM_TASKS_BBG];
static struct sockaddr_in server_address;
static int socket_conn = 0;
static msg_t socket_msg;
static pthread_t socket_task;
static timer_t bbg_log_timer;
static timer_t bbg_speak_timer;

void *bbg_socket_helper(void *p) {
    /* Get socket and initialize */
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&server_address, '0', sizeof(server_address));
    memset(&socket_msg, '0', sizeof(socket_msg));

    /* Set up connection */
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(4000);

    /* Listen for connection */
    bind(fd, (struct sockaddr *)&server_address, sizeof(server_address));
    listen(fd, 10);

    /* Get connection */
    socket_conn = accept(fd, (struct sockaddr *) NULL, NULL);

    while(1) {
        read(socket_conn, &socket_msg, MSG_SIZE);
        
        /* Send to message queue */
        msg_send(&socket_msg);
    }
} 

void bbg_init_timer(timer_t tmr, void (*fun)(union sigval)) {
    struct itimerspec ts;
    struct sigevent se;

    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = &tmr;
    se.sigev_notify_function = fun;
    se.sigev_notify_attributes = NULL;

    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = BBG_HEARTBEAT_PERIOD;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    timer_create(CLOCK_REALTIME, &se, &tmr);
    timer_settime(tmr, 0, &ts, 0);
}

void bbg_log_heartbeat_timer(union sigval arg) {
    /* Restart thread */
}

void bbg_speak_heartbeat_timer(union sigval arg) {
    /* Restart thread */
}

uint8_t bbg_heartbeat(msg_t *rx) {
    if (rx->from == DEFS_TASK_LOG) {
        bbg_init_timer(bbg_log_timer, bbg_log_heartbeat_timer);
    } else if (rx->from == DEFS_TASK_SPEAK) {
        bbg_init_timer(bbg_speak_timer, bbg_speak_heartbeat_timer);
    }

    return BBG_SUCCESS;	
}

uint8_t bbg_exit(msg_t *rx) {
    /* Exit program */

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
    if (pthread_create(&main_tasks[DEFS_TASK_SPEAK], NULL, speak_task, NULL)) {
       return BBG_ERR_INIT;
    }
    if (pthread_create(&main_tasks[DEFS_TASK_LOG], NULL, log_task, NULL)) {
       return BBG_ERR_INIT;
    }
    if (pthread_create(&socket_task, NULL, bbg_socket_helper, NULL)) {
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
    tx.cmd  = LOG_INIT;
    tx.data[0] = 1;
    strcpy((char *) (tx.data+1), log_name);
    msg_send(&tx);

    /* Initialize speak task */
    tx.devt = DEFS_ID_BBG;
    tx.to   = DEFS_TASK_SPEAK;
    tx.cmd  = SPEAK_INIT;
    tx.data[0] = 0;
    msg_send(&tx);

    /* Send speak message */
    tx.cmd = SPEAK_SPEAK;
    char hello[32] = "Hello world";
    strcpy((char *) (tx.data), hello);
    msg_send(&tx); 

    /* Initialize heartbeat timers */
    bbg_init_timer(bbg_log_timer, bbg_log_heartbeat_timer);
    bbg_init_timer(bbg_speak_timer, bbg_speak_heartbeat_timer);

    /* Open queue and start main loop */
    mqd_t rxq = mq_open(msg_names[DEFS_TASK_BBG], O_RDONLY);
    msg_t rx;
    while(1) {
        mq_receive(rxq, (char *) &rx, MSG_SIZE+10, NULL);
        
        /* Send message over network */
        if (rx.devt != DEFS_ID_BBG) {
            /* Send over network */
            write(socket_conn, &rx, MSG_SIZE);
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
