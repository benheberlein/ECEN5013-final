/*******************************************************************************/
/**
 * @file msg.c
 * @brief Message queues for interprocess communication
 * 
 * The interprocess communication is handled by POSIX message queues. Each task
 * has its own queue that is implemented here.
 *
 * @author Ben Heberlein
 * @date Dec 9 2017
 * @version 2.0
 *
 */

#include "msg.h"
#include <stdint.h>
#include <stdio.h>
#include "defs.h"

/* Queue array */
//QueueHandle_t msg_queues[MSG_QUEUE_NUM];

uint8_t msg_route(msg_t *rx) {
    if (rx->devt != DEFS_ID_TIVA) {
        return MSG_ERR_ROUTE;
    }

    if (rx->to == DEFS_TASK_TIVA) {
        return MSG_ERR_ROUTE;
    }

    if (!xQueueSend(msg_queues[rx->to], rx, 500)) {
        return MSG_ERR_FAIL;
    }

    return MSG_SUCCESS;
}

uint8_t msg_send(msg_t *tx) {
    
    /* Always send to main thread, handles socket */
    if (!xQueueSend(msg_queues[DEFS_TASK_TIVA], tx, 500)) {
        return MSG_ERR_FAIL;   
    }

#ifdef TARGET_BBG
    mq_send(msg_queues[to], (char *) tx, MSG_SIZE, 0);
#endif

    return MSG_SUCCESS;
}

uint8_t msg_init(void) {

    for (int i = 0; i < MSG_QUEUE_NUM; i++) {
        msg_queues[i] = xQueueCreate(MSG_MAXMSGS, sizeof(msg_t));
    }

#ifdef TARGET_BBG
    /* Set attributes */
    for (int i = 0; i < MAIN_THREAD_TOTAL; i++) {
        msg_attrs[i].mq_flags = 0;
        msg_attrs[i].mq_maxmsg = MSG_MAXMSGS;
        msg_attrs[i].mq_msgsize = MSG_SIZE;
        msg_attrs[i].mq_curmsgs = 0;
    }
    msg_attrs[MAIN_THREAD_LOG].mq_msgsize = MSG_LOGSIZE;

    /* Open queues */
    for (int i = 0; i < MAIN_THREAD_TOTAL; i++) {
            msg_queues[i] = mq_open(msg_names[i], O_WRONLY | O_CREAT, MSG_QUEUE_PERM, &msg_attrs[i]);
            if (msg_queues[i] == -1) {
                perror("msg init");
                return MSG_ERR_INIT;
            }

    }

#endif

    return MSG_SUCCESS;
}

