#ifndef _UARTCONSOLE_H_
#define _UARTCONSOLE_H_

#include <ti/drivers/PIN.h>
#include <ti/drivers/UART.h>
//#include <ti/sysbios/knl/Event.h>


//#define UARTCONSOLE_TXCALLBACK

#define CONSOLEPROCESS_STATE_INPUTCMD						(0)
#define CONSOLEPROCESS_STATE_INPUTPARAM						(CONSOLEPROCESS_STATE_INPUTCMD + 1)
#define CONSOLEPROCESS_STATE_SGRLOG							(CONSOLEPROCESS_STATE_INPUTPARAM + 1)
#define CONSOLEPROCESS_STATE_SGRLOG_T						(CONSOLEPROCESS_STATE_SGRLOG + 1)
#define CONSOLEPROCESS_STATE_RX								(CONSOLEPROCESS_STATE_SGRLOG_T + 1)
#define CONSOLEPROCESS_STATE_IPC							(CONSOLEPROCESS_STATE_RX + 1)
#define CONSOLEPROCESS_STATE_IPC_T							(CONSOLEPROCESS_STATE_IPC + 1)
#define CONSOLEPROCESS_STATE_CW								(CONSOLEPROCESS_STATE_IPC_T + 1)
#define CONSOLEPROCESS_STATE_MS								(CONSOLEPROCESS_STATE_CW + 1)
#define CONSOLEPROCESS_STATE_PERTX							(CONSOLEPROCESS_STATE_MS + 1)
#define CONSOLEPROCESS_STATE_PERRX							(CONSOLEPROCESS_STATE_PERTX + 1)
#define CONSOLEPROCESS_STATE_1TDOA_DEBUG					(CONSOLEPROCESS_STATE_PERRX + 1)
#define CONSOLEPROCESS_STATE_1TDOA_RANGE					(CONSOLEPROCESS_STATE_1TDOA_DEBUG + 1)
#define CONSOLEPROCESS_STATE_1TDOA_TIME						(CONSOLEPROCESS_STATE_1TDOA_RANGE + 1)
#define CONSOLEPROCESS_STATE_1TDOA_DST						(CONSOLEPROCESS_STATE_1TDOA_TIME + 1)

#define CONSOLEPROCESS_MAX_CMD_MSG_LEN		64

/// \brief ConsoleInputProcess and error codes
typedef enum
{
    ConsoleInputProcess_Status_Success			= 0,
    ConsoleInputProcess_Status_CmdIdError		= 1,
    ConsoleInputProcess_Status_CmdLenError		= 2,
    ConsoleInputProcess_Status_ParamError		= 3,
    ConsoleInputProcess_Status_MemoryError		= 4,
	ConsoleInputProcess_Status_ReleaseError		= 5,
} ConsoleInputProcess_Status;


#define MESSAGE_FOR_EVENT_ALL     0xFFFFFFFF
#define MESSAGE_ONE_EVENT_NUMBER   (uint32_t)(1 << 0)
#define MESSAGE_TWO_EVENT_NUMBER   (uint32_t)(1 << 1)
#define MESSAGE_THREE_EVENT_NUMBER   (uint32_t)(1 << 2)




extern uint8_t consoleState;

/*---------------------------------------------------------------------------*/
//#define uartOutputStr(x)     UART_write(consoleUartHandle, x, sizeof(x));

/*---------------------------------------------------------------------------*/

extern void uartReadCallback(UART_Handle handle, void *rxBuf, size_t size);
#ifdef UARTCONSOLE_TXCALLBACK
extern void uartWriteCallback(UART_Handle handle, void *txBuf, size_t size);
#endif

extern void uartConsole_outputStr(char *str);
//extern void uartConsole_outputStrN(char *str, uint8_t n);
//extern void uartConsole_outputStrLen(char *str, unsigned int len);

extern void writeLog(const char* message);
extern void writeLogNL(const char* message);
extern void writeLogF(const char *format, ...);
extern void writeLogFNL(const char *format, ...);

/*
 * Task creation function for the UART Console.
 */
//extern void uartConsole_createTask(UART_Handle uartHandle, PIN_Handle ledPinHandle);
extern void uartConsole_createTask(UART_Handle uartHandle);

extern void events_createTask();






#endif /* _UARTCONSOLE_H_ */
