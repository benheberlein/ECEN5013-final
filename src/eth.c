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
 * @file eth.c
 * @brief Ethernet controller
 * 
 * This is an ethernet driver for the TM4C129XL. It is designed to be bare
 * bones, supporting just basic IO. This file is mostly adapted from the 
 * TivaWare Peripheral Driver Library User's Guide.
 *
 * @author Ben Heberlein
 * @date Dec 7 2017
 * @version 1.0
 *
 */

/* C standard */
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

/* Tivaware includes */
#include "inc/hw_emac.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/can.h"
#include "driverlib/emac.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"

#include "eth.h"

/* MAC address on board */
const static uint8_t eth_mac_addr[6] = {0x00, 0x1a, 0xb6, 0x03, 0x3a, 0x04};

/* Need at least 3 descriptors for each */
#define ETH_NUM_TX_DESCRIPTORS 1
#define ETH_NUM_RX_DESCRIPTORS 3
tEMACDMADescriptor eth_rx_descriptor[ETH_NUM_RX_DESCRIPTORS];
tEMACDMADescriptor eth_tx_descriptor[ETH_NUM_TX_DESCRIPTORS];

uint32_t eth_rx_desc_index = 0;
uint32_t eth_tx_desc_index = ETH_NUM_TX_DESCRIPTORS - 1;

#define ETH_RX_BUF_SIZE 1536
#define ETH_TX_BUF_SIZE 1536
uint8_t eth_rx_buf[ETH_NUM_RX_DESCRIPTORS][ETH_RX_BUF_SIZE];
uint8_t eth_tx_buf[ETH_NUM_TX_DESCRIPTORS][ETH_TX_BUF_SIZE];

void eth_isr(void) {
    /* Read and clear */
    uint32_t temp;
    temp = EMACIntStatus(EMAC0_BASE, true);
    EMACIntClear(EMAC0_BASE, temp);

    /* Process interrupt or defer processing. Should be implemented somewhere 
       else to connect with OS. */
    eth_process();    
}

int32_t eth_rx_size() {
    int_fast32_t len = 0;

    /* Check if we have descriptor */
    if (!(eth_rx_descriptor[eth_rx_desc_index].ui32CtrlStatus & DES0_RX_CTRL_OWN)) {

        /* Check if frame is valid */
        if (!(eth_rx_descriptor[eth_rx_desc_index].ui32CtrlStatus & DES0_RX_STAT_ERR)) {

            /* Check if the last descriptor flag */
            if (eth_rx_descriptor[eth_rx_desc_index].ui32CtrlStatus & DES0_RX_STAT_LAST_DESC) {

                /* Get size */
                len = ((eth_rx_descriptor[eth_rx_desc_index].ui32CtrlStatus &
                                DES0_RX_STAT_FRAME_LENGTH_M) >>
                                DES0_RX_STAT_FRAME_LENGTH_S);

                return len;
            }
        }
    }
    
    return 0;
}

int32_t eth_rx(uint8_t *buf, int32_t len) { 

    /* Check if we have descriptor */
    if (!(eth_rx_descriptor[eth_rx_desc_index].ui32CtrlStatus & DES0_RX_CTRL_OWN)) {
        
        /* Check if frame is valid */
        if (!(eth_rx_descriptor[eth_rx_desc_index].ui32CtrlStatus & DES0_RX_STAT_ERR)) {

            /* Check if the last descriptor flag */
            if (eth_rx_descriptor[eth_rx_desc_index].ui32CtrlStatus & DES0_RX_STAT_LAST_DESC) {

                /* Get size */
/*                len = ((eth_rx_descriptor[eth_rx_desc_index].ui32CtrlStatus & 
                                DES0_RX_STAT_FRAME_LENGTH_M) >> 
                                DES0_RX_STAT_FRAME_LENGTH_S);
*/
                /* Process the frame */
                memcpy(buf, eth_rx_descriptor[eth_rx_desc_index].pvBuffer1, len);
            }
        }

        /* Reuse descriptor */
        eth_rx_descriptor[eth_rx_desc_index].ui32CtrlStatus = DES0_RX_CTRL_OWN;

        /* Use next descriptor */
        eth_rx_desc_index++;
        if (eth_rx_desc_index >= ETH_NUM_RX_DESCRIPTORS) {
            eth_rx_desc_index = 0;
        }
    }

    return len;
}

int32_t eth_tx(uint8_t *buf, int32_t len) {
    /* Next descriptor */
    eth_tx_desc_index++;
    if (eth_tx_desc_index >= ETH_NUM_TX_DESCRIPTORS) {
        eth_tx_desc_index = 0;
    }

    /* Wait for transmit descriptor */
    while (eth_tx_descriptor[eth_tx_desc_index].ui32CtrlStatus & DES0_TX_CTRL_OWN) {}

    /* Copy data into TX buffer */
    for (int i = 0; i < len; i++) {
        eth_tx_buf[eth_tx_desc_index][i] = buf[i];
    }

    /* Fill out packet info */
    eth_tx_descriptor[eth_tx_desc_index].ui32Count = (DES1_TX_CTRL_BUFF1_SIZE_M &
                                                     (len << DES1_TX_CTRL_BUFF1_SIZE_S));
    eth_rx_descriptor[eth_tx_desc_index].pvBuffer1 = eth_tx_buf[eth_tx_desc_index];
    eth_tx_descriptor[eth_tx_desc_index].ui32CtrlStatus = 
            (DES0_TX_CTRL_LAST_SEG | DES0_TX_CTRL_FIRST_SEG |
             DES0_TX_CTRL_INTERRUPT | DES0_TX_CTRL_IP_ALL_CKHSUMS |
             DES0_TX_CTRL_CHAINED | DES0_TX_CTRL_OWN);

    /* Start DMA */
    EMACTxDMAPollDemand(EMAC0_BASE);

    return len;
}

void eth_init() {

    /* Ethernet LEDs */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinConfigure(GPIO_PF0_EN0LED0);
    GPIOPinConfigure(GPIO_PF4_EN0LED1);
    GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_4);

    /* Turn on ethernet MAC and PHY */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EMAC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EPHY0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_EMAC0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_EPHY0);

    /* Wait for MAC */
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_EMAC0)) {}

    /* Set internal PHY */
    EMACPHYConfigSet(EMAC0_BASE, 
                    (EMAC_PHY_TYPE_INTERNAL | 
                     EMAC_PHY_INT_MDIX_EN | 
                     EMAC_PHY_AN_100B_T_FULL_DUPLEX));

    /* Initialize the MAC */
    EMACReset(EMAC0_BASE);
    EMACInit(EMAC0_BASE,
             120000000,
             EMAC_BCONFIG_MIXED_BURST | EMAC_BCONFIG_PRIORITY_FIXED, 
             4, 4, 0);

    /* Set MAC configuration */
    EMACConfigSet(EMAC0_BASE, 
                 (EMAC_CONFIG_FULL_DUPLEX | EMAC_CONFIG_CHECKSUM_OFFLOAD |
                  EMAC_CONFIG_7BYTE_PREAMBLE | EMAC_CONFIG_IF_GAP_96BITS |
                  EMAC_CONFIG_USE_MACADDR0 | EMAC_CONFIG_SA_FROM_DESCRIPTOR |
                  EMAC_CONFIG_BO_LIMIT_1024),
                 (EMAC_MODE_RX_STORE_FORWARD | EMAC_MODE_TX_STORE_FORWARD |
                  EMAC_MODE_TX_THRESHOLD_64_BYTES | EMAC_MODE_RX_THRESHOLD_64_BYTES), 0);

    /* Initialize the DMA TX descriptors */
    for (int i = 0; i < ETH_NUM_TX_DESCRIPTORS; i++) {
        eth_tx_descriptor[i].ui32Count = (DES1_TX_CTRL_SADDR_INSERT);
                                         //(ETH_TX_BUF_SIZE << DES1_TX_CTRL_BUFF1_SIZE_S));
        eth_tx_descriptor[i].pvBuffer1 = eth_tx_buf[i];
        eth_tx_descriptor[i].DES3.pLink = (i == (ETH_NUM_TX_DESCRIPTORS - 1)) ?
                                           eth_tx_descriptor : &eth_tx_descriptor[i+1];
        eth_tx_descriptor[i].ui32CtrlStatus = (DES0_TX_CTRL_LAST_SEG | DES0_TX_CTRL_FIRST_SEG |
                                               DES0_TX_CTRL_INTERRUPT | DES0_TX_CTRL_CHAINED |
                                               DES0_TX_CTRL_IP_ALL_CKHSUMS);
    }

    /* Initialize the DMA RX descriptors */
    for (int i = 0; i < ETH_NUM_RX_DESCRIPTORS; i++) {
        eth_tx_descriptor[i].ui32CtrlStatus = 0;
        eth_rx_descriptor[i].ui32Count = (DES1_RX_CTRL_CHAINED | 
                                         (ETH_RX_BUF_SIZE << DES1_RX_CTRL_BUFF1_SIZE_S));
        eth_rx_descriptor[i].pvBuffer1 = eth_rx_buf[i];
        eth_rx_descriptor[i].DES3.pLink = (i == (ETH_NUM_RX_DESCRIPTORS - 1)) ?
                                           eth_rx_descriptor : &eth_rx_descriptor[i+1];
    }

    /* Connect descriptors */
    EMACRxDMADescriptorListSet(EMAC0_BASE, eth_rx_descriptor);
    EMACTxDMADescriptorListSet(EMAC0_BASE, eth_tx_descriptor);

    /* Clear indexes */
    eth_rx_desc_index = 0;
    eth_tx_desc_index = 0;    

    /* Set MAC address */
    EMACAddrSet(EMAC0_BASE, 0, eth_mac_addr);    

    /* Wait for PHY */
    while((EMACPHYRead(EMAC0_BASE, 0, EPHY_BMSR) & EPHY_BMSR_LINKSTAT) == 0) {}

    /* Set MAC filtering */
    EMACFrameFilterSet(EMAC0_BASE, //EMAC_FRMFILTER_RiX_ALL | EMAC_FRMFILTER_PASS_ALL_CTRL);    
                      (EMAC_FRMFILTER_SADDR |
                       EMAC_FRMFILTER_PASS_MULTICAST |
                       EMAC_FRMFILTER_PASS_NO_CTRL));

    /* Clear Interrupts */
    EMACIntClear(EMAC0_BASE, EMACIntStatus(EMAC0_BASE, false));

    /* Activate DMA descriptors */
    for (int i = 0; i < ETH_NUM_RX_DESCRIPTORS; i++) {
        eth_rx_descriptor[i].ui32CtrlStatus |= DES0_RX_CTRL_OWN;
    }

    /* Enable ethernet */
    EMACTxEnable(EMAC0_BASE);
    EMACRxEnable(EMAC0_BASE);

    /* Set priority (important for FreeRTOS) */
    IntPrioritySet(INT_EMAC0, (7<<5));

    /* Enable ethernet interrupt */
    IntEnable(INT_EMAC0);
    EMACIntEnable(EMAC0_BASE, EMAC_INT_RECEIVE);
}
