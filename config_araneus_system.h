/*
 * config_araneus_system.h
 *
 *  Created on:
 *      Author:
 */

#ifndef CONFIG_ARANEUS_SYSTEM_H_
#define CONFIG_ARANEUS_SYSTEM_H_


#include <ti/sysbios/knl/Task.h>

// TI-RTOS Header files
#include <ti/drivers/PIN.h>
#include <ti/drivers/UART.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/BIOS.h>
#include <Board.h>

#include "UartConsole.h"
#include "UartConsole_shell.h"









#endif /* CONFIG_ARANEUS_SYSTEM_H_ */
