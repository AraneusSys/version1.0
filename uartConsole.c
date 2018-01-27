#include <string.h>
#include <stdio.h>
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#ifdef XDC_TIMESTAMP_SUPPORT
#include <xdc/runtime/Timestamp.h>			//used for Timestamp() calls
#endif

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
//#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/gates/GateMutex.h>


/* TI-RTOS Header files */
#include <ti/drivers/PIN.h>
#include <ti/drivers/UART.h>

/* Utils/Board Header files */
//#include "util.h"
#include "Board.h"

#include "uartConsole.h"
#include "uartConsole_shell.h"
#include <xdc/runtime/Log.h>


static PIN_Handle consoleLedPinHandle;

UART_Handle consoleUartHandle;

// Clock instances for internal periodic events.
static Clock_Struct consolePeriodicClock;

// How often to perform periodic event (in msec)
#define UARTCONSOLE_PERIODIC_EVT_PERIOD				500

// Internal Events for RTOS application
#define UARTCONSOLE_STATE_CHANGE_EVT				0x0001
#define UARTCONSOLE_CHAR_CHANGE_EVT					0x0002
#define UARTCONSOLE_PERIODIC_EVT					0x0004
#define UARTCONSOLE_CONN_EVT_END_EVT				0x0008

// Task configuration
//#define UARTCONSOLETASK_STACKSIZE	1024
#define UARTCONSOLETASK_STACKSIZE	(2048-512)
//#define UARTCONSOLETASK_STACKSIZE	3072
#define UARTCONSOLETASK_PRIORITY	1
Task_Struct uartConsoleTask;
Task_Struct eventsConsoleTask;

Char uartConsoleTaskStack[UARTCONSOLETASK_STACKSIZE];
Char eventsConsoleTaskStack[UARTCONSOLETASK_STACKSIZE];


//#define MESSAGE_FOR_EVENT_ALL     0xFFFFFFFF
//#define MESSAGE_ONE_EVENT_NUMBER   (uint32_t)(1 << 0)
//#define MESSAGE_TWO_EVENT_NUMBER   (uint32_t)(1 << 1)
//#define MESSAGE_THREE_EVENT_NUMBER   (uint32_t)(1 << 2)




// *************************************************

//#define DbgPrint_printf System_printf
#define DbgPrint_printf(f_, ...); System_printf((f_), __VA_ARGS__); System_flush();

#define MAX_NUM_RX_BYTES    16		// Maximum RX bytes to receive in one go
#define MAX_NUM_TX_BYTES    16		// Maximum TX bytes to send in one go

#ifdef	UARTCONSOLE_TXCALLBACK
//#define	BIOS_WAIT_CYCLES	500
#define	BIOS_WAIT_CYCLES	1000
#endif

#ifdef XDC_TIMESTAMP_SUPPORT
extern uint64_t upTime_clk;
extern xdc_runtime_Types_FreqHz freq1; /* Timestamp frequency */
extern xdc_runtime_Types_FreqHz freq2; /* BIOS frequency */
#endif

/***************************************************************************************************
 * CONSTANTS
 ***************************************************************************************************/
const char sgrRandDRxTx_uartConsole_GreetingStr[] =
		"\x1b]0; ARANAUS  System  UART Console\x1b\\"
		"\r\n\r\n"
		"=== ARANAUS Project CC1310/UART Console v.02 ===\r\n"
		"========== Compiled: "__DATE__" "__TIME__" ==========\n\r";



uint8_t rxBuf[MAX_NUM_RX_BYTES];   // Receive buffer
uint8_t txBuf[MAX_NUM_TX_BYTES];   // Transmit buffer
uint32_t wantedRxBytes;            // Number of bytes received so far
uint32_t enteredRxBytes = 0;
//uint32_t wantedTxBytes;            // Number of bytes to be transmitted to console output
static uint32_t wantedTxBytes = 0;          // Number of bytes to be transmitted to console output
PIN_State  hStateHui1;
#define HUI_LED_A     PIN_ID(11)
#define HUI_LED_B     PIN_ID(10)
#define HUI_LED_C     PIN_ID(9)
#define HUI_BUTTON_A  PIN_ID(23)
#define HUI_BUTTON_B  PIN_ID(24)



//static void func_01();
//static void func_02();
//static void func_03();
//
//
//
//static void func_01()
//{
//    // Define pins used by Human user interface and initial configuration
//    const PIN_Config aPinListHui[] = {
//        HUI_LED_A    | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
//        HUI_LED_B    | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
//        HUI_BUTTON_A | PIN_INPUT_EN  | PIN_PULLUP | PIN_HYSTERESIS,
//        HUI_BUTTON_B | PIN_INPUT_EN  | PIN_PULLUP | PIN_HYSTERESIS,
//        PIN_TERMINATE
//    };
//
//    // Get handle to this collection of pins
//    if (!PIN_open(&hStateHui1, aPinListHui))
//    {
//    }
//
//
//    PIN_Status status = PIN_add(&hStateHui1,HUI_LED_C | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX);
//
//    if (status != PIN_SUCCESS)
//    {
//
//
//    }
//
//    PIN_setPortOutputValue(&hStateHui1, (1<<HUI_LED_C));
//    PIN_setOutputValue(&hStateHui1, PIN_ID(9),1);
//  //  PIN_setPortOutputEnable(&hStateHui1, 0);
//
//
////            huiDoSomething();
//
////    PIN_close(&hStateHui1);
//
//
//}
//
//static void func_02()
//{
//    // Define pins used by Human user interface and initial configuration
//    const PIN_Config aPinListHui[] = {
//        HUI_LED_A    | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
//        HUI_LED_B    | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
//        HUI_BUTTON_A | PIN_INPUT_EN  | PIN_PULLUP | PIN_HYSTERESIS,
//        HUI_BUTTON_B | PIN_INPUT_EN  | PIN_PULLUP | PIN_HYSTERESIS,
//        PIN_TERMINATE
//    };
//
//    // Get handle to this collection of pins
//    if (!PIN_open(&hStateHui1, aPinListHui))
//    {
//    }
//
//
//    PIN_Status status = PIN_add(&hStateHui1,HUI_LED_C | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX);
//
//    if (status != PIN_SUCCESS)
//    {
//    }
//
//
//    PIN_setPortOutputValue(&hStateHui1, (1<<HUI_LED_C));
//    PIN_setOutputValue(&hStateHui1, PIN_ID(9),0);
////    PIN_setPortOutputEnable(&hStateHui1, 0);
////    huiDoSomething();
////    PIN_close(&hStateHui1);
//
//
//}
//
//static void func_03()
//{
//}
//












static Semaphore_Handle consoleUartRxSem;
#ifdef	UARTCONSOLE_TXCALLBACK
Semaphore_Handle consoleUartTxSem;
#endif

uint8_t consoleState = CONSOLEPROCESS_STATE_INPUTCMD;
//uint8_t consoleState = CONSOLEPROCESS_STATE_CW;


//Event_Struct rfTestEvent;
//Event_Handle rfTestEventHandle;







void uartConsole_outputStr(char *str)
{

	 	UART_write(consoleUartHandle, str, strlen(str));
	//	Log_info0((IArg)str);
}


bool isAnythingPressed()
{
	return enteredRxBytes > 0;
}

void writeLog(const char* message)
{
#ifndef	UARTCONSOLE_TXCALLBACK
	//UART_write(consoleUartHandle, message, strlen(message));
	uartConsole_outputStr((char*)message);
#else
	uartConsole_outputStr((char*)message);
#endif
}

void writeLogNL(const char* message){
	//writeLog(message);
	//writeLog("\r\n");

	char buffer[256];
	static size_t len;

	strcpy(buffer, message);
	len = strlen(buffer);
	buffer[len] = '\r';
	buffer[len+1] = '\n';
	buffer[len+2] = 0;

	writeLog(buffer);
}

/** Public Method Implementation **/
void writeLogF(const char *format, ...){
     static uint8_t buffer[256];
     va_list args;

     // Start processing the arguments
     va_start(args, format);

     // Output the parameters into a string
     vsprintf((char *)buffer, format, args);

     // Output the string to the console
     writeLog((char *)buffer);

     // End processing of the arguments
     va_end(args);
}

void writeLogFNL(const char *format, ...){
	/*
    static uint8_t buffer[256];
    va_list args;

    // Start processing the arguments
    va_start(args, format);

    // Output the parameters into a string
    vsprintf((char *)buffer, format, args);

    // Output the string to the console
    writeLog((char *)buffer);

    // End processing of the arguments
    va_end(args);

    writeLog("\r\n");
    */
    char buffer[256];
    va_list args;
    static size_t len;

    // Start processing the arguments
    va_start(args, format);

    // Output the parameters into a string
    vsprintf((char *)buffer, format, args);

    // End processing of the arguments
    va_end(args);

 	len = strlen(buffer);
 	buffer[len] = '\r';
 	buffer[len+1] = '\n';
 	buffer[len+2] = 0;

    // Output the string to the console
    writeLog((char *)buffer);

}
/* ---------------------------------------------- */

/*********************************************************************
 * @fn      uartConsole_heartBeatHandler
 *
 * @brief   Handler function for clock timeouts.
 *
 * @param   arg - event type
 *
 * @return  None.
 */
static void uartConsole_heartBeatHandler(UArg arg)
{
	// Store the event.
	//events |= arg;

	// Wake up the application.
	//Semaphore_post(sem);

	// LED Toggle !PIN_getInputValue(pinId)
//    PIN_setOutputValue(consoleLedPinHandle, Board_LED, !PIN_getInputValue(Board_LED));

	// Do Nothing. TBD.

}


/*
 *  ======== uartReadCallback ========
 */
void uartReadCallback(UART_Handle handle, void *rxBuf, size_t size)
{
	enteredRxBytes = size;
    Semaphore_post(consoleUartRxSem);

    /* warning It is STRONGLY discouraged to call UART_read from its own callback function (UART_MODE_CALLBACK). */ /* !? */
}


void uartConsoleInputProcessing(uint8_t ch)
{
    static char cmdBuffer[CONSOLEPROCESS_MAX_CMD_MSG_LEN];
    static uint8_t cmdLen = 0;

	/* */
	    switch( consoleState )
	    {
	        case CONSOLEPROCESS_STATE_INPUTCMD:
	            if((ch == '\n') || (ch == '\r'))
	            {
	                //command entry complete, process it
	            	cmdBuffer[cmdLen] = 0;		// str limiter for debug print.
	            	consoleState = consoleParseIncomingCmd(cmdBuffer, cmdLen);
	                //consoleState = CONSOLEPROCESS_STATE_INPUTCMD;
	                cmdLen = 0;

	                if(consoleState == CONSOLEPROCESS_STATE_INPUTCMD) {
	                	// Display Console prompt
	                	//wantedTxBytes += sprintf((char*)&txBuf[wantedTxBytes], uartConsolePrompt);
	                	uartConsoleShell_prompt();
	                	uartConsoleShell_CorsorON();
	                }
	            }
	            else if(ch == '\b')
	            {
	            	// Pressing the backspace key on a computer terminal would generate the ASCII code 08, BS or Backspace,
	            	// a control code which would delete the preceding character. That control code could also be accessed
	            	// by pressing Control-H, as H is the eighth letter of the Latin alphabet.
	            	// \b (8) is a nondestructive backspace. It moves the cursor backward, but doesn't erase what's there.
	            	if(cmdLen > 0) {
	                	wantedTxBytes += sprintf((char*)&txBuf[wantedTxBytes], "\b \b");
	                	cmdLen--;
	            	}
	            }
	            else
	            {
	                if(cmdLen < CONSOLEPROCESS_MAX_CMD_MSG_LEN)
	                {
	                    //add this char to the command string
	                    cmdBuffer[cmdLen++] = ch;
	            	    // Copy bytes from RX buffer to TX buffer (echo)
	        	    	txBuf[wantedTxBytes++] = ch;

	                    //System_printf("cmdLen: %u, cmdBuffer: %s\n", cmdLen, cmdBuffer);
	                    //System_flush();
	                }
	                else
	                {

	                	uartConsole_outputStr((char*)uartConsoleShell_TooLongInputStr);
	                	uartConsoleShell_prompt();
	                    consoleState = CONSOLEPROCESS_STATE_INPUTCMD;
	                    cmdLen = 0;
	                    uartConsoleShell_CorsorON();
	                }
	            }
	            break;
	        default:
                if(consoleState != CONSOLEPROCESS_STATE_INPUTCMD) {
                	//Break Cmd execution
                	uartConsoleShell_prompt();
    	            consoleState = CONSOLEPROCESS_STATE_INPUTCMD;
    	            cmdLen = 0;
    	            uartConsoleShell_CorsorON();
                }
	            break;
	    }

}

void consoleAUXinit(void)
{
	  //Create a semaphore for blocking commands
	  Semaphore_Params params;
	  Error_Block eb;

	  // init params
	  Semaphore_Params_init(&params);
	  Error_init(&eb);

	  // create semaphore instance if not already created
	  if (consoleUartRxSem == NULL)
	  {
		  consoleUartRxSem = Semaphore_create(0, &params, &eb);
	      if (consoleUartRxSem == NULL)
	      {
	    	  //return Status_Mem_Error;
	    	  if (consoleUartHandle == NULL) {
	    		  System_abort("Error opening the consoleUartRxSem");
	    	  }

	      }

	      //Semaphore_post(consoleRxEvent);
	  }
	  else
	  {
	      //already configured and taken consoleRxEvent, so release it
	      //Semaphore_post(consoleRxEvent);
	  }

}

/*
 *  ======== uartConsoleFxn ========
 */
Void uartConsoleFxn(UArg arg0, UArg arg1)
{
    uint32_t lenTx;
    //uint8_t ownIeeeAddress[IEEE_ADDR_LEN];
    consoleAUXinit();


    uartConsole_outputStr((char*)sgrRandDRxTx_uartConsole_GreetingStr);
    enteredRxBytes = 0;
    uartConsoleShell_start();


    // Start initial read (first call), with dfined size
    wantedRxBytes = 1;
    int rxBytes = UART_read(consoleUartHandle, rxBuf, wantedRxBytes);


   	/* Loop forever receiving commands */
    while (1) {

        if(Semaphore_pend(consoleUartRxSem, 20000/Clock_tickPeriod)) {

        	int i;
        	for(i = 0; i < enteredRxBytes; i++) {
        	    // Process bytes from RX buffer
        		uint8_t ch = rxBuf[i];
            	uartConsoleInputProcessing(ch);
        	}

        	if(consoleState == CONSOLEPROCESS_STATE_INPUTCMD) {
        		// Echo to the screen



        		lenTx = wantedTxBytes;
        		char buf[10] = {0};
        		strncpy(buf, (char*)txBuf, wantedTxBytes);
        		//uartConsole_outputStrLen((char*)txBuf, wantedTxBytes);
        		uartConsole_outputStr(buf);



        		wantedTxBytes -= lenTx;
        	} else {
        		// Do not echo to the screen
        	}

        	UART_readCancel(consoleUartHandle); //to stop prev read

            // Start another read, with size the same as it was during first call to
            UART_read(consoleUartHandle, rxBuf, wantedRxBytes);
        }

        uartConsoleShell_DoFxn();

    }

}












/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      uartConsole_createTask
 *
 * @brief   Task creation function for the Sub-1GHz Radio Carrier Wave
 * 			Test Console.
 *
 * @param	UART_Handle uartHandle
 * 			PIN_Handle ledPinHandle
 *
 * @return  none
 */
//void uartConsole_createTask(UART_Handle uartHandle, PIN_Handle ledPinHandle)
void uartConsole_createTask(UART_Handle uartHandle)

{
  Task_Params taskParams;

  consoleUartHandle = uartHandle;
//  consoleLedPinHandle = ledPinHandle;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = uartConsoleTaskStack;
  taskParams.stackSize = UARTCONSOLETASK_STACKSIZE;
  taskParams.priority = UARTCONSOLETASK_PRIORITY;

  Task_construct(&uartConsoleTask, uartConsoleFxn, &taskParams, NULL);
}


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn     events_createTask
 *
 * @brief   Task creation function for the Sub-1GHz Radio Carrier Wave events system
 *
 * @return  none
 */
//void events_createTask()
//{
//
//
//    Event_Params eventParam;
//    Event_Params_init(&eventParam);
//    Event_construct(&rfTestEvent, &eventParam);
//    rfTestEventHandle = Event_handle(&rfTestEvent);
//
//    Task_Params taskParams;
//	  // Configure task
//	Task_Params_init(&taskParams);
//	taskParams.stack = eventsConsoleTaskStack;
//	taskParams.stackSize = 1024;
//	taskParams.priority = 2;
//
//	Task_construct(&eventsConsoleTask, eventsConsoleFxn, &taskParams, NULL);
//
//
//}
