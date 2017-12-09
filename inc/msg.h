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
 * @file msg.h
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

#ifndef __MSG_H__
#define __MSG_H__

#include <stdint.h>

/**
 * @brief Message attributes
 */
#define MSG_SIZE        128
#define MSG_DATASIZE    125
#define MSG_MAXMSGS     8

/**
 * @brief Error codes
 */
#define MSG_SUCCESS     0
#define MSG_ERR_INIT    1
#define MSG_ERR_STUB    126
#define MSG_ERR_UNKNOWN 127

/**
 * @brief Message structure for tasks
 */
typedef struct __attribute((packed)) msg_s {
    uint8_t dev;        /* Device ID */
    uint8_t from;       /* First bit is RSP field */
    uint8_t cmd;        /* Command */ 
    uint8_t data[MSG_DATASIZE];   /* NULL terminated data */
} msg_t;

/**
 * @brief Queue descriptions
 */
#ifdef TARGET_BBG
#define MSG_QUEUE_NUM 4
#define MSG_QUEUE_PERM  0666
mqd_t msg_queues[MSG_QUEUE_NUM];
struct mq_attr msg_attrs[MSG_QUEUE_NUM];
static const char *msg_names[] = {"/mainqueue",
      		                        "/lightqueue",
            	    	            "/tempqueue",
                          		    "/logqueue"};
#endif

/**
 * @brief Send a message
 */ 
uint8_t msg_send(msg_t *tx);

/**
 * @brief Initialize queues
 */
uint8_t msg_init(void);

#endif /* __MSG_H__ */
