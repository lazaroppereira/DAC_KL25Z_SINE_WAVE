/* ###################################################################
 **     Filename    : main.c
 **     Project     : DAC_GUILHERME
 **     Processor   : MKL25Z128VLK4
 **     Version     : Driver 01.01
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2016-09-03, 09:29, # CodeGen: 0
 **     Abstract    :
 **         Main module.
 **         This module contains user's application code.
 **     Settings    :
 **     Contents    :
 **         No public methods
 **
 ** ###################################################################*/
/*!
 ** @file main.c
 ** @version 01.01
 ** @brief
 **         Main module.
 **         This module contains user's application code.
 */
/*!
 **  @addtogroup main_module main module documentation
 **  @{
 */
/* MODULE main */

/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "DA1.h"
#include "USB1.h"
#include "USB0.h"
#include "CDC1.h"
#include "Tx1.h"
#include "Rx1.h"
#include "CS1.h"
#include "CS2.h"
#include "WAIT1.h"
#include "WAIT2.h"
#include "UTIL1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* User includes (#include below this line is not maintained by Processor Expert) */

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
#define SINUS_LENGTH 256
static const LDD_DAC_TData SinusOutputData[SINUS_LENGTH] = { 2047U, 2097U,
		2147U, 2198U, 2248U, 2298U, 2347U, 2397U, 2446U, 2496U, 2544U, 2593U,
		2641U, 2689U, 2737U, 2784U, 2830U, 2877U, 2922U, 2967U, 3012U, 3056U,
		3099U, 3142U, 3184U, 3226U, 3266U, 3306U, 3346U, 3384U, 3422U, 3458U,
		3494U, 3530U, 3564U, 3597U, 3629U, 3661U, 3691U, 3721U, 3749U, 3776U,
		3803U, 3828U, 3852U, 3875U, 3897U, 3918U, 3938U, 3957U, 3974U, 3991U,
		4006U, 4020U, 4033U, 4044U, 4055U, 4064U, 4072U, 4079U, 4084U, 4088U,
		4092U, 4093U, 4094U, 4093U, 4092U, 4088U, 4084U, 4079U, 4072U, 4064U,
		4055U, 4044U, 4033U, 4020U, 4006U, 3991U, 3974U, 3957U, 3938U, 3918U,
		3897U, 3875U, 3852U, 3828U, 3803U, 3776U, 3749U, 3721U, 3691U, 3661U,
		3629U, 3597U, 3564U, 3530U, 3494U, 3458U, 3422U, 3384U, 3346U, 3306U,
		3266U, 3226U, 3184U, 3142U, 3099U, 3056U, 3012U, 2967U, 2922U, 2877U,
		2830U, 2784U, 2737U, 2689U, 2641U, 2593U, 2544U, 2496U, 2446U, 2397U,
		2347U, 2298U, 2248U, 2198U, 2147U, 2097U, 2047U, 1997U, 1947U, 1896U,
		1846U, 1796U, 1747U, 1697U, 1648U, 1598U, 1550U, 1501U, 1453U, 1405U,
		1357U, 1310U, 1264U, 1217U, 1172U, 1127U, 1082U, 1038U, 995U, 952U,
		910U, 868U, 828U, 788U, 748U, 710U, 672U, 636U, 600U, 564U, 530U, 497U,
		465U, 433U, 403U, 373U, 345U, 318U, 291U, 266U, 242U, 219U, 197U, 176U,
		156U, 137U, 120U, 103U, 88U, 74U, 61U, 50U, 39U, 30U, 22U, 15U, 10U, 6U,
		2U, 1U, 0U, 1U, 2U, 6U, 10U, 15U, 22U, 30U, 39U, 50U, 61U, 74U, 88U,
		103U, 120U, 137U, 156U, 176U, 197U, 219U, 242U, 266U, 291U, 318U, 345U,
		373U, 403U, 433U, 465U, 497U, 530U, 564U, 600U, 636U, 672U, 710U, 748U,
		788U, 828U, 868U, 910U, 952U, 995U, 1038U, 1082U, 1127U, 1172U, 1217U,
		1264U, 1310U, 1357U, 1405U, 1453U, 1501U, 1550U, 1598U, 1648U, 1697U,
		1747U, 1796U, 1846U, 1896U, 1947U, 1997U };

static LDD_TError Error;
static LDD_TDeviceData *MyDacPtr;

static uint8_t cdc_buffer[USB1_DATA_BUFF_SIZE];
static uint8_t in_buffer[USB1_DATA_BUFF_SIZE];

static void CDC_Run(void) {

	int i, cnt = 0;

	uint32_t val = 0;
	unsigned char buf[16];

	for (;;) {
		while (CDC1_App_Task(cdc_buffer, sizeof(cdc_buffer)) == ERR_BUSOFF) {
			/* device not enumerated */

			WAIT1_Waitms(10);
		}

		if (CDC1_GetCharsInRxBuf()
				!= 0&& CDC1_App_Task(cdc_buffer, sizeof(cdc_buffer)) == ERR_OK) {
			i = CDC1_GetCharsInRxBuf();
			i = 0;
			while (i < sizeof(in_buffer) - 1
					&& CDC1_GetChar(&in_buffer[i]) == ERR_OK) {
				i++;
			}
			in_buffer[i] = '\0';
			(void) CDC1_SendString((unsigned char*) "echo: ");
			(void) CDC1_SendString(in_buffer);
			UTIL1_strcpy(buf, sizeof(buf), (unsigned char*) "val: ");
			UTIL1_strcatNum32u(buf, sizeof(buf), val);
			UTIL1_strcat(buf, sizeof(buf), (unsigned char*) "\r\n");
			(void) CDC1_SendString(buf);
			val++;
		} else {
			WAIT2_Waitms(10);
			cnt++;
			if ((cnt % 1024) == 0) { /* from time to time, write some text */
				(void) CDC1_SendString("1s");
				CDC1_SendBlock((unsigned char*) "hello?\r\n",
						sizeof("hello?\r\n") - 1);
			}
		}
	}
}

int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
	/* Write your local variable definition here */
	unsigned int i;
	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init();
	while(1){
	MyDacPtr = DA1_Init(NULL);
	for (i = 0; i < SINUS_LENGTH; i++) {
		Error = DA1_SetValue(MyDacPtr, SinusOutputData[i]); /* Set converter output */
		WAIT2_Waitms(50);
	}}
//	CDC_Run();
	/*** End of Processor Expert internal initialization.                    ***/

	/* Write your code here */
	/* For example: for(;;) { } */

	/*** Don't write any code pass this line, or it will be deleted during code generation. ***/
	/*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
#ifdef PEX_RTOS_START
	PEX_RTOS_START(); /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
#endif
	/*** End of RTOS startup code.  ***/
	/*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
	for (;;) {
	}
	/*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
 ** @}
 */
/*
 ** ###################################################################
 **
 **     This file was created by Processor Expert 10.5 [05.21]
 **     for the Freescale Kinetis series of microcontrollers.
 **
 ** ###################################################################
 */
