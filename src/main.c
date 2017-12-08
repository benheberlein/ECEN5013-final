#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/can.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

#include "eth.h"

/* MAC address */
const static uint8_t ucMACAddr[6] = {0x00, 0x1a, 0xb6, 0x03, 0x3a, 0x04};

/* IP Address if DHCP fails */
static const uint8_t ucIPAddress[ 4 ] = { 192, 168, 0, 35 };
static const uint8_t ucNetMask[ 4 ] = { 255, 255, 255, 255 };
static const uint8_t ucGatewayAddress[ 4 ] = { 192, 168, 0, 1 };

static const uint8_t ucDNSServerAddress[ 4 ] = { 192, 168, 0, 1};

void demoTask(void *pvParamters) {

    uint8_t init = 0;

    //
    // Loop forever.
    //
    while(1)
    {
        //
        // Turn on the LED.
        //
//        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);

        //
        // Delay for a bit.
        //
        vTaskDelay(100);

        //
        // Turn off the LED.
        //
//        if (FreeRTOS_IsNetworkUp()) {
//            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x0);
//        }
        //
        // Delay for a bit.
        //
        vTaskDelay(100);

        if (init == 0) {
            FreeRTOS_IPInit(ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddr);
            init = 1;
        }
    }
}

void vApplicationPingReplyHook( ePingReplyStatus_t eStatus, uint16_t usIdentifier ) {
    switch( eStatus ) {
        case eSuccess    :
            /* A valid ping reply has been received.  Post the sequence number
            on the queue that is read by the vSendPing() function below.  Do
            not wait more than 10ms trying to send the message if it cannot be
            sent immediately because this function is called from the TCP/IP
            RTOS task - blocking in this function will block the TCP/IP RTOS task. */
            break;

        case eInvalidChecksum :
        case eInvalidData :
            /* A reply was received but it was not valid. */
            break;
    }

    return;
}

void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent ) {
    uint32_t ulIPAddress, ulNetMask, ulGatewayAddress, ulDNSServerAddress;
    int8_t cBuffer[ 16 ];

    if( eNetworkEvent == eNetworkUp )
    {
        /* The network is up and configured.  Print out the configuration
        obtained from the DHCP server. */
        FreeRTOS_GetAddressConfiguration( &ulIPAddress,
                                          &ulNetMask,
                                          &ulGatewayAddress,
                                          &ulDNSServerAddress );

        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);

        ulIPAddress = FreeRTOS_inet_addr("192.168.0.13");

        uint16_t usRequestSequenceNumber, usReplySequenceNumber;

        usRequestSequenceNumber = FreeRTOS_SendPingRequest( ulIPAddress, 8, 100 / portTICK_PERIOD_MS );

        if (usRequestSequenceNumber == pdFAIL) {
            ulIPAddress = 111;
        }

        /* Convert the IP address to a string then print it out. */
        //FreeRTOS_inet_ntoa( ulIPAddress, cBuffer );
        //printf( "IP Address: %s\r\n", cBuffer );

        /* Convert the net mask to a string then print it out. */
        //FreeRTOS_inet_ntoa( ulNetMask, cBuffer );
        //printf( "Subnet Mask: %s\r\n", cBuffer );

        /* Convert the IP address of the gateway to a string then print it out. */
        //FreeRTOS_inet_ntoa( ulGatewayAddress, cBuffer );
        //printf( "Gateway IP Address: %s\r\n", cBuffer );

        /* Convert the IP address of the DNS server to a string then print it out. */
        //FreeRTOS_inet_ntoa( ulDNSServerAddress, cBuffer );
        //printf( "DNS server IP Address: %s\r\n", cBuffer );
    }
}

int main() {
    
    uint32_t clock_rate;
    clock_rate = SysCtlClockFreqSet(
        (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
        SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
        SYSTEM_CLOCK);

    volatile uint32_t ui32Loop;

    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    //
    // Check if the peripheral access is enabled.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }

    //
    // Enable the GPIO pin for the LED (PN0).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x0);

//    eth_init();

    xTaskCreate(demoTask, (const portCHAR*)"LED", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    vTaskStartScheduler();

    while(1) {}
    
    return 0;

}
