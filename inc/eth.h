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
 * @brief Process recieve interrupt
 */
int32_t eth_process(void);

/**
 * @brief Ethernet interrupt
 * 
 * Needs to be connected to vector table
 */
void eth_int(void);

//int32_t eth_rx(void);

/**
 * @brief Ethernet send
 */
int32_t eth_tx(uint8_t *buf, int32_t len);

/**
 * @brief Initialize ethernet
 */ 
void eth_init();

#endif /*__ETH_H__*/
