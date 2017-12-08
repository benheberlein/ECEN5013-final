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
 * @file eth.h
 * @brief Ethernet controller
 * 
 * This is an ethernet driver for the TM4C129XL. It is designed to be bare
 * bones, supporting just basic IO.
 *
 * @author Ben Heberlein
 * @date Dec 7 2017
 * @version 1.0
 *
 */

#ifndef __ETH_H__
#define __ETH_H__

/** 
 * @brief Function to call from rx interrupt to process
 *
 * Should be implemented elsewhere depending on OS and
 * schedule call to eth_rx
 */
void eth_process(void);

/**
 * @brief Process recieve interrupt length
 */
int32_t eth_rx_size(void);

/**
 * @brief Process recieve interrupt
 */
int32_t eth_rx(uint8_t *buf, int32_t len);

/**
 * @brief Ethernet interrupt
 * 
 * Needs to be connected to vector table
 */
void eth_int(void);

/**
 * @brief Ethernet send
 */
int32_t eth_tx(uint8_t *buf, int32_t len);

/**
 * @brief Initialize ethernet
 */ 
void eth_init();

#endif /*__ETH_H__*/
