/*
 * @brief Blinky example using timers
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "chip.h"
#include "board.h"
#include "string.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#if defined(BOARD_EA_DEVKIT_1788) || defined(BOARD_EA_DEVKIT_4088)
#define UART_SELECTION 	LPC_UART0
#define IRQ_SELECTION 	UART0_IRQn
#define HANDLER_NAME 	UART0_IRQHandler
#elif defined(BOARD_NXP_LPCXPRESSO_1769)
#define UART_SELECTION 	LPC_UART3
#define IRQ_SELECTION 	UART3_IRQn
#define HANDLER_NAME 	UART3_IRQHandler
#else
#error No UART selected for undefined board
#endif



/**
 * @brief	Main UART program body
 * @return	Always returns 1
 */
int main(void)
{
	SystemCoreClockUpdate();
	Board_Init();
	Board_UART_Init(UART_SELECTION);
	Board_LED_Set(0, false);

	/* Setup UART for 115.2K8N1 */
	Chip_UART_Init(UART_SELECTION);
	Chip_UART_SetBaud(UART_SELECTION, 115200);
	Chip_UART_ConfigData(UART_SELECTION, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(UART_SELECTION, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(UART_SELECTION);

	char send[] = "BL Begin.\r\n ";
	Chip_UART_SendBlocking(UART_SELECTION, send, sizeof(send));

#define APP_ADDR (0x48200)
	__disable_irq();
	__set_MSP(*(__IO uint32_t *)APP_ADDR);
	__set_PSP(*(__IO uint32_t *)APP_ADDR);
	typedef void (*p_function)(void);
	unsigned int jump_addr = *(volatile unsigned int *)(APP_ADDR + 4);
	p_function JumpToApplication = (p_function)jump_addr;
	JumpToApplication();


	return 0;
}
