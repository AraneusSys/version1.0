#include <string.h>
#include <stdio.h>
#include <stdlib.h>     /* atoi */

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
#include <ti/sysbios/hal/Seconds.h>
#include <time.h>
#include "uartConsole.h"
#include "uartConsole_shell.h"

struct ptEntry {
  char *cmdStr;
  void (* pfunc)(char *str);
};

#define SHELL_PROMPT uartConsoleShell_PromptStr	//"SGR> "
#define CONSOLERX_FILTER_REPORTFULL 0
#define CONSOLERX_FILTER_REPORTIDASPROMTPAYLOAD 1
#define CONSOLERX_FILTER_REPORTPAYLOAD 2

/*---------------------------------------------------------------------------*/
struct parseTabEntry {
	char *cmdStr;
	uint8_t (* pFunc)(char *str);
	char *helpStr;
};



static uint8_t testCmd(char *cmdBuffer);
static uint8_t timeCmd(char *cmdBuffer);
static uint8_t helpCmd(char *cmdBuffer);
static uint8_t unknownCmd(char *cmdBuffer);


static const struct parseTabEntry parseTabTest[] =
{
	{"test",    testCmd,	"test command"},
	{"time",	timeCmd,	"displays time"},
	{"help",	helpCmd,	"this help"},


	{NULL,		unknownCmd}
};



/*---------------------------------------------------------------------------*/
Semaphore_Handle consoleSGRPacketRxSem;
extern uint8_t tdoaDefaultProfile;
extern int8_t tdoaDefaultPower;

/***************************************************************************************************
 * CONSTANTS
 ***************************************************************************************************/
const char wittraBS_uartConsoleShell_GreetingStr[] =
		"\r\n"
		"Welcome to Wittra BS Shell for Sub-1GHz Console v.02\r\n"
		"---------- Compiled: "__DATE__" "__TIME__" ----------\n\r";

const char ConsoleOKstr[] =
		"\r\n"
		"OK\r\n";

const char ConsoleTypeAnyKeyStr[] =
		"Type any key to stop command execution.\r\n";

const char uartConsoleShell_TooLongInputStr[] =
		"\r\n"
		"Input command line should be less than 16 symbols. Try again.\r\n";


const char uartConsoleShell_PromptStr[] =
		"\r\n"
		"SGR";

ConsoleInputProcess_Status inputStatus = ConsoleInputProcess_Status_CmdIdError;





static uint8_t timeCmd(char *cmdBuffer) {


    return CONSOLEPROCESS_STATE_INPUTCMD;

}

static uint8_t testCmd(char *cmdBuffer) {
    return CONSOLEPROCESS_STATE_INPUTCMD;
}


static void ConsolePrintOK(void) {

    // Display this command prints
	uartConsole_outputStr((char*)ConsoleOKstr);

}


static uint8_t helpCmd(char *cmdBuffer) {
    const struct parseTabEntry *p;
    uint8_t cmdLine;

	inputStatus = ConsoleInputProcess_Status_Success;
	ConsolePrintOK();


    for(p = parseTabTest; p->cmdStr != NULL; ++p) {
    	uartConsole_outputStr((char*)p->cmdStr);
    	uartConsole_outputStr((char*)"\t- ");
    	uartConsole_outputStr((char*)p->helpStr);
    	uartConsole_outputStr((char*)"\n\r");
    }

	return CONSOLEPROCESS_STATE_INPUTCMD;

}



static uint8_t unknownCmd(char *cmdBuffer)
{

	if(strlen(cmdBuffer) > 0) {
		uartConsole_outputStr("\n\rUnknown command: ");
		uartConsole_outputStr(cmdBuffer);
	}

	inputStatus = ConsoleInputProcess_Status_CmdIdError;
	uartConsole_outputStr("");

	return CONSOLEPROCESS_STATE_INPUTCMD;
}



/*
 * ---------------------------------------------------------------------------
 */

uint8_t consoleParseIncomingCmd( char* cmdBuffer, uint8_t cmdLen )
{
    uint8_t consoleState;
    const struct parseTabEntry *p;
    inputStatus = ConsoleInputProcess_Status_CmdIdError;

    for(p = parseTabTest; p->cmdStr != NULL; ++p) {
    	if(strncmp(p->cmdStr, cmdBuffer, strlen(p->cmdStr)) == 0) {
    		break;
    	}
    }

    consoleState = p->pFunc(cmdBuffer);
    return consoleState;
}



static void uartConsoleShell_RawEasyLinkRxFxn(void) {
}


void uartConsoleShell_DoFxn(void) {
	if(consoleState == CONSOLEPROCESS_STATE_RX) {
		uartConsoleShell_RawEasyLinkRxFxn();
	} else {

	}

}

/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
void uartConsoleShell_start(void)
{
	//Create a semaphores for blocking commands
	Semaphore_Params params;
	Error_Block eb;

	// SGR
	// init params
	Semaphore_Params_init(&params);
	Error_init(&eb);

	// create semaphore instance if not already created
	if (consoleSGRPacketRxSem == NULL)
	{
		consoleSGRPacketRxSem = Semaphore_create(0, &params, &eb);
		if (consoleSGRPacketRxSem == NULL)
		{
			//return EasyLink_Status_Mem_Error;
		}

		//Semaphore_post(consoleSGRPacketRxSem);
	}
	else
	{
		//already configured and taken consoleSGRPacketRxSem, so release it
		//Semaphore_post(consoleSGRPacketRxSem);
	}


	uartConsole_outputStr((char*)wittraBS_uartConsoleShell_GreetingStr);
	uartConsole_outputStr("Type '?' and return for help\n\r");

	uartConsole_outputStr((char*)ConsoleTypeAnyKeyStr);


}
/*---------------------------------------------------------------------------*/
void uartConsoleShell_prompt(void)
{
	uartConsole_outputStr((char*)SHELL_PROMPT);
	uartConsole_outputStr("> ");

}
/*---------------------------------------------------------------------------*/



void uartConsoleShell_CorsorON(void)
{
	 uartConsole_outputStr((char*)"\x1b[?25h");  // consoleANSI_setCursorOnStr

}

