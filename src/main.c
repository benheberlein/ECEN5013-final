#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

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
static const uint8_t ucNetMask[ 4 ] = { 255, 255, 255, 0 };
static const uint8_t ucGatewayAddress[ 4 ] = { 192, 168, 0, 1 };

static const uint8_t ucDNSServerAddress[ 4 ] = { 208, 67, 222, 222 };

void demoTask(void *pvParamters) {

    //
    // Loop forever.
    //
    while(1)
    {
        //
        // Turn on the LED.
        //
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);

        //
        // Delay for a bit.
        //
        vTaskDelay(1000);

        //
        // Turn off the LED.
        //
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x0);

        //
        // Delay for a bit.
        //
        vTaskDelay(1000);
    }
}

int main()
{
    
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

//    eth_init();

    xTaskCreate(demoTask, (const portCHAR*)"LED", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    FreeRTOS_IPInit(ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddr);

    vTaskStartScheduler();

    while(1) {}
    
    return 0;

}
