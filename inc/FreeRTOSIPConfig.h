#ifndef FREERTOS_IP_CONFIG_H
#define FREERTOS_IP_CONFIG_H

#include <stdlib.h>

#define ipconfigEVENT_QUEUE_LENGTH                      65
#define ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS          60
#define ipconfigIP_TASK_PRIORITY                        ( configMAX_PRIORITIES - 2 )
#define ipconfigIP_TASK_STACK_SIZE_WORDS                ( configMINIMAL_STACK_SIZE * 5 )
#define ipconfigNETWORK_MTU                             1200
#define ipconfigUSE_NETWORK_EVENT_HOOK                  1
#define ipconfigBYTE_ORDER                              pdFREERTOS_LITTLE_ENDIAN
#define ipconfigSUPPORT_OUTGOING_PINGS                  1
#define ipconfigREPLY_TO_INCOMING_PINGS                 1
#define ipconfigUSE_DHCP                                0
#define ipconfigETHERNET_DRIVER_FILTERS_FRAME_TYPES     1
#define ipconfigDRIVER_INCLUDED_RX_IP_CHECKSUM          1
#define ipconfigARP_CACHE_ENTRIES                       6
#define ipconfigMAX_ARP_RETRANSMISSIONS                 5
#define ipconfigUSE_TCP                                 1
#define ipconfigUSE_TCP_WIN                             0 
#define ipconfigFILTER_OUT_NON_ETHERNET_II_FRAMES       1



#endif
