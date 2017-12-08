#ifndef FREERTOS_IP_CONFIG_H
#define FREERTOS_IP_CONFIG_H

#include <stdlib.h>

#define ipconfigEVENT_QUEUE_LENGTH          55
#define ipconfigIP_TASK_PRIORITY            ( configMAX_PRIORITIES - 2 )
#define ipconfigIP_TASK_STACK_SIZE_WORDS    ( configMINIMAL_STACK_SIZE * 5 )
#define ipconfigNETWORK_MTU                 1526
#define ipconfigUSE_NETWORK_EVENT_HOOK      1
#define ipconfigBYTE_ORDER                  pdFREERTOS_LITTLE_ENDIAN

#endif
