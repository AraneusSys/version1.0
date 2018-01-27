#ifndef _UARTCONSOLE_SHELL_H_
#define _WITTRABS_UARTCONSOLE_SHELL_H_

/***** Includes *****/
#include <xdc/std.h>

#include <ti/sysbios/knl/Semaphore.h>

extern const char uartConsoleShell_PromptStr[];
extern const char uartConsoleShell_TooLongInputStr[];

extern Semaphore_Handle consoleSGRPacketRxSem;

/**
 * Initialize the shell.
 *
 * Called when the shell front-end process starts. This function may
 * be used to start listening for signals.
 */
extern void uartConsoleShell_init(void);

/**
 * Start the shell back-end.
 *
 * Called by the front-end when a new shell is started.
 */
extern void uartConsoleShell_start(void);

/**
 * Process a shell command.
 *
 * This function will be called by the shell GUI / telnet server whan
 * a command has been entered that should be processed by the shell
 * back-end.
 *
 * \param command The command to be processed.
 */
extern void uartConsoleShell_input(char *command);

/**
 * Quit the shell.
 *
 */
extern void uartConsoleShell_quit(char *);


/**
 * Print a string to the shell window.
 *
 * This function is implemented by the shell GUI / telnet server and
 * can be called by the shell back-end to output a string in the
 * shell window. The string is automatically appended with a linebreak.
 *
 * \param str1 The first half of the string to be output.
 * \param str2 The second half of the string to be output.
 */
extern void uartConsoleShell_output(char *str1, char *str2);

/**
 * Print a prompt to the shell window.
 *
 * This function can be used by the shell back-end to print out a
 * prompt to the shell window.
 *
 * \param prompt The prompt to be printed.
 *
 */
//extern void uartConsoleShell_prompt(char *prompt);
extern void uartConsoleShell_prompt(void);


extern void uartConsoleShell_CorsorON(void);

extern uint8_t consoleParseIncomingCmd( char* cmdBuffer, uint8_t cmdLen );

extern void uartConsoleShell_BreakExec(void);

extern void uartConsoleShell_DoFxn(void);

#endif /* _UARTCONSOLE_SHELL_H_ */
