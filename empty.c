#include "config_araneus_system.h"



UART_Handle uartHandle;
/*
 *  ======== main ========
 */
/*
 *  ======== main ========
 */
int main(void)
{


  Board_initGeneral();
  // Board_initEMAC();
  Board_initGPIO();
  // Board_initI2C();
  // Board_initSDSPI();
  // Board_initSPI();
   Board_initUART();
  // Board_initUSB(Board_USBDEVICE);
  // Board_initUSBMSCHFatFs();
  // Board_initWatchdog();
  // Board_initWiFi();




	UART_Params uartParams;
    UART_Params_init(&uartParams);
    // UART READ
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readMode = UART_MODE_CALLBACK;
    uartParams.readCallback = uartReadCallback;
    //uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 921600;
    uartHandle  = UART_open(Board_UART0, &uartParams);

    uartConsole_createTask(uartHandle);


    /* Start BIOS */

    BIOS_start();
    return (0);
}
