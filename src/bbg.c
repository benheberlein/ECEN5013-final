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
static char socket_buf[MSG_SIZE];
static pthread_t socket_task;

void *bbg_socket_helper(void *p) {
    /* Get socket and initialize */
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&server_address, '0', sizeof(server_address));
    memset(socket_buf, '0', sizeof(socket_buf));

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
        read(socket_conn, socket_buf, MSG_SIZE);
        printf((char*)socket_buf);
    }
} 

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
