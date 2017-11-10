/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

 /******************************************************************************
 *
 * See the following URL for information on the commands defined in this file:
 * http://www.FreeRTOS.org/FreeRTOS-Plus/FreeRTOS_Plus_UDP/Embedded_Ethernet_Examples/Ethernet_Related_CLI_Commands.shtml
 *
 ******************************************************************************/


/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rtc.h"
#include "eeprom_save.h"
#include "DS18B20_GetTemperature.h"

/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"



extern EventGroupHandle_t TCPEventGroupHandler;	//事件标志组句柄
extern EventGroupHandle_t SwitchEventGroupHandler;	//事件标志组句柄

/*
 * Implements the run-time-stats command.
 */
static BaseType_t prvTaskStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/*
 * Implements the task-stats command.
 */
static BaseType_t prvRunTimeStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/*
 * Implements the get-rtc-time command.
 */
static BaseType_t prvGetRTCTimeCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/*
 * Implements the set-rtc-time command.
 */
static BaseType_t prvSetRTCTimeCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/*
 * Implements the get-TCP-evenflag command.
 */
static BaseType_t prvGetEvenFlagCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvEEPROM_MachineAddrSaveCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvEEPROM_CurrentAddrSaveCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvEEPROM_VibratorAddrSaveCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvEEPROM_InfraredTempAddrSaveCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvSearchDS18B20Command( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvEEPROM_SendTimeSaveCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/* Structure that defines the "run-time-stats" command line command.   This
generates a table that shows how much run time each task has */
static const CLI_Command_Definition_t xRunTimeStats =
{
	"run-time-stats", /* The command string to type. */
	"\r\nrun-time-stats:\r\n Displays a table showing how much processing time each FreeRTOS task has used\r\n",
	prvRunTimeStatsCommand, /* The function to run. */
	0 /* No parameters are expected. */
};

/* Structure that defines the "task-stats" command line command.  This generates
a table that gives information on each task in the system. */
static const CLI_Command_Definition_t xTaskStats =
{
	"task-stats", /* The command string to type. */
	"\r\ntask-stats:\r\n Displays a table showing the state of each FreeRTOS task\r\n",
	prvTaskStatsCommand, /* The function to run. */
	0 /* No parameters are expected. */
};

/* Structure that defines the "GetRTCTime" command line command.  This generates
a table that gives information RTC time in the system. */
static const CLI_Command_Definition_t xGetRTCTime =
{
	"get-rtc-time", /* The command string to type. */
	"\r\nget-rtc-time:\r\n Displays the current RTC clock\r\n",
	prvGetRTCTimeCommand, /* The function to run. */
	0 /* No parameters are expected. */
};
/* Structure that defines the "SetRTCTime" command line command.  
Change information RTC time in the system. */
static const CLI_Command_Definition_t xSetRTCTime =
{
	"set-rtc-time", /* The command string to type. */
	"\r\nset-rtc-time <year> <month> <data> <hour> <minute> <sec> :\r\n Change the  RTC clock\r\nExample:2017 10 31 11 25 00\r\n",
	prvSetRTCTimeCommand, /* The function to run. */
	6 /* No parameters are expected. */
};
/* Structure that defines the get-TCP-evenflag" command line command.  This generates
a table that TCP EvenFlag information in the system. */
static const CLI_Command_Definition_t xGetEvenFlag =
{
	"get-evenflag", /* The command string to type. */
	"\r\nget-evenflag <0|1>:\r\n Displays the 0: TCPEvenFlag\t 1: SwitchEvenFlag\r\n",
	prvGetEvenFlagCommand, /* The function to run. */
	1 /* No parameters are expected. */
};

static const CLI_Command_Definition_t xEEPROM_MachineAddrSave =
{
	"machine-addr-save", /* The command string to type. */
	"\r\nmachine-addr-save <addr>:\r\n Displays  previous MachineAddr:<addr>\t Displays current MachineAddr:<addr>\r\n",
	prvEEPROM_MachineAddrSaveCommand, /* The function to run. */
	1 /* No parameters are expected. */
};

static const CLI_Command_Definition_t xEEPROM_CurrentAddrSave =
{
	"current-addr-save", /* The command string to type. */
	"\r\ncurrent-addr-save <01addr1> <01addr2> <01addr3> <01addr4>:\r\n Displays previous CurrentAddr:<addr1> <addr2> <addr3> <addr4>\r\nDisplays current CurrentAddr:<addr1> <addr2> <addr3> <addr4>\r\n",
	prvEEPROM_CurrentAddrSaveCommand, /* The function to run. */
	CURRENT_NUM /* No parameters are expected. */
};

static const CLI_Command_Definition_t xEEPROM_InfraredTempAddrSave =
{
	"infraredtemp-addr-save", /* The command string to type. */
	"\r\ninfraredtemp-addr-save <02addr1> <02addr2> :\r\n Displays previous InfraredTempAddr:<addr1> <addr2>\r\nDisplays current InfraredTempAddr:<addr1> <addr2> \r\n",
	prvEEPROM_InfraredTempAddrSaveCommand, /* The function to run. */
	INFRAREDTEMP_NUM /* No parameters are expected. */
};
static const CLI_Command_Definition_t xEEPROM_VibratorAddrSave =
{
	"vibrator-addr-save", /* The command string to type. */
	"\r\nvibrator-addr-save <03addr1> <03addr2> :\r\n Displays previous VibratorAddr:<addr1> <addr2>\r\nDisplays current VibratorAddr:<addr1> <addr2> \r\n",
	prvEEPROM_VibratorAddrSaveCommand, /* The function to run. */
	VIBRATOR_NUM /* No parameters are expected. */
};

static const CLI_Command_Definition_t xSearchDS18B20 =
{
	"search-ds18b20-rom", /* The command string to type. */
	"\r\nsearch-ds18b20-rom <0|1> :\r\n Displays the online ds18b20 number\r\n rom code: <8bit ROM>\r\nif search-ds18b20-rom <0> will contiune save search <1>will refresh save",
	prvSearchDS18B20Command, /* The function to run. */
	1 /* No parameters are expected. */
};


static const CLI_Command_Definition_t xEEPROM_SendTimeSave =
{
	"send-time-save", /* The command string to type. */
	"\r\nsend-time-save <ds18b20|01> <current> <infraredtemp> <vibrator> <switch> <heartbeat>:\r\n Displays current Send time:<time>\r\n",
	prvEEPROM_SendTimeSaveCommand, /* The function to run. */
	6 /* No parameters are expected. */
};

/*-----------------------------------------------------------*/

void vRegisterSampleCLICommands( void )
{
	/* Register all the command line commands defined immediately above. */
	FreeRTOS_CLIRegisterCommand( &xTaskStats );
	FreeRTOS_CLIRegisterCommand( &xRunTimeStats );
	FreeRTOS_CLIRegisterCommand( &xGetRTCTime );
	FreeRTOS_CLIRegisterCommand( &xSetRTCTime );
	FreeRTOS_CLIRegisterCommand( &xGetEvenFlag );
	FreeRTOS_CLIRegisterCommand( &xEEPROM_MachineAddrSave );
	FreeRTOS_CLIRegisterCommand( &xEEPROM_CurrentAddrSave );
	FreeRTOS_CLIRegisterCommand( &xEEPROM_InfraredTempAddrSave );
	FreeRTOS_CLIRegisterCommand( &xEEPROM_VibratorAddrSave );
	FreeRTOS_CLIRegisterCommand( &xSearchDS18B20 );
	FreeRTOS_CLIRegisterCommand( &xEEPROM_SendTimeSave );
}
/*-----------------------------------------------------------*/

static BaseType_t prvTaskStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char *const pcHeader = "\r\nTask          State  Priority  Stack	#\r\n************************************************\r\n";

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	/* Generate a table of task stats. */
	strcpy( pcWriteBuffer, pcHeader );
	vTaskList( pcWriteBuffer + strlen( pcHeader ) );

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}
/*-----------------------------------------------------------*/

static BaseType_t prvGetRTCTimeCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	const char * const pcHeader = "\r\nYear\tMonth\tData\tweek\tHour\tMinute\tSec\r\n****************************************************\r\n";
	/* Generate a table of task stats. */
	strcpy( pcWriteBuffer, pcHeader );
	sprintf(pcWriteBuffer+strlen(pcHeader),"%4d\t%4d\t%4d\t%4d\t%4d\t%4d\t%4d\r\n",\
							calendar.w_year,calendar.w_month,calendar.w_date,calendar.week,calendar.hour,calendar.min,calendar.sec);
	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}

/*-----------------------------------------------------------*/

static BaseType_t prvSetRTCTimeCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	const char *pcParameter;
	BaseType_t xParameterStringLength;
	_calendar_obj *temp_calendar;
	uint16_t time_temp;
	const char * const pcHeader = "\r\nSet Complete\r\nYear\tMonth\tData\tHour\tMinute\tSec\r\n****************************************************\r\n";
	
	temp_calendar = pvPortMalloc(sizeof(_calendar_obj));
	/* Obtain the year parameter string. */
	pcParameter = FreeRTOS_CLIGetParameter
					(
						pcCommandString,		/* The command string itself. */
						1,						/* Return the first parameter. */
						&xParameterStringLength	/* Store the parameter string length. */
					);

	/* Sanity check something was returned. */
	configASSERT( pcParameter );
	if(xParameterStringLength == 4)
	{
		time_temp = (*pcParameter-'0')*1000+(*(pcParameter+1)-'0')*100+(*(pcParameter+2)-'0')*10+(*(pcParameter+3)-'0');
	}
	else
	{
		sprintf( pcWriteBuffer, "Error:year must four number" );
		return pdFALSE;
	}
	temp_calendar->w_year = time_temp;
	
	/* Obtain the month parameter string. */
	pcParameter = FreeRTOS_CLIGetParameter
					(
						pcCommandString,		/* The command string itself. */
						2,						/* Return the first parameter. */
						&xParameterStringLength	/* Store the parameter string length. */
					);

	/* Sanity check something was returned. */
	configASSERT( pcParameter );
	if(xParameterStringLength == 2)
	{
		time_temp = (*pcParameter-'0')*10+(*(pcParameter+1)-'0');
		if((time_temp < 1)||(time_temp > 12))
		{
			sprintf( pcWriteBuffer, "Error:month number should from 1 to 12" );
			return pdFALSE;
		}
	}
	else
	{
		sprintf( pcWriteBuffer, "Error:month must two number" );
		return pdFALSE;
	}
	temp_calendar->w_month = (u8)time_temp;
	
	/* Obtain the data parameter string. */
	pcParameter = FreeRTOS_CLIGetParameter
					(
						pcCommandString,		/* The command string itself. */
						3,						/* Return the first parameter. */
						&xParameterStringLength	/* Store the parameter string length. */
					);

	/* Sanity check something was returned. */
	configASSERT( pcParameter );
	if(xParameterStringLength == 2)
	{
		time_temp = (*pcParameter-'0')*10+(*(pcParameter+1)-'0');
		if((time_temp < 1)||(time_temp > 31))
		{
			sprintf( pcWriteBuffer, "Error:data number should from 1 to 31" );
			return pdFALSE;
		}
	}
	else
	{
		sprintf( pcWriteBuffer, "Error:data must two number and number form 1 to 31" );
		return pdFALSE;
	}
	temp_calendar->w_date = (u8)time_temp;
	
	/* Obtain the hour parameter string. */
	pcParameter = FreeRTOS_CLIGetParameter
					(
						pcCommandString,		/* The command string itself. */
						4,						/* Return the first parameter. */
						&xParameterStringLength	/* Store the parameter string length. */
					);

	/* Sanity check something was returned. */
	configASSERT( pcParameter );
	if(xParameterStringLength == 2)
	{
		time_temp = (*pcParameter-'0')*10+(*(pcParameter+1)-'0');
		if(time_temp > 24)
		{
			sprintf( pcWriteBuffer, "Error:hour number should from 0 to 24" );
			return pdFALSE;
		}
	}
	else
	{
		sprintf( pcWriteBuffer, "Error:hour must two number" );
		return pdFALSE;
	}
	temp_calendar->hour = (u8)time_temp;
	
	/* Obtain the minute parameter string. */
	pcParameter = FreeRTOS_CLIGetParameter
					(
						pcCommandString,		/* The command string itself. */
						5,						/* Return the first parameter. */
						&xParameterStringLength	/* Store the parameter string length. */
					);

	/* Sanity check something was returned. */
	configASSERT( pcParameter );
	if(xParameterStringLength == 2)
	{
		time_temp = (*pcParameter-'0')*10+(*(pcParameter+1)-'0');
		if(time_temp > 60)
		{
			sprintf( pcWriteBuffer, "Error:minute number should from 0 to 60" );
			return pdFALSE;
		}
	}
	else
	{
		sprintf( pcWriteBuffer, "Error:minute must two number" );
		return pdFALSE;
	}
	temp_calendar->min = (u8)time_temp;
	
	/* Obtain the sec parameter string. */
	pcParameter = FreeRTOS_CLIGetParameter
					(
						pcCommandString,		/* The command string itself. */
						6,						/* Return the first parameter. */
						&xParameterStringLength	/* Store the parameter string length. */
					);

	/* Sanity check something was returned. */
	configASSERT( pcParameter );
	if(xParameterStringLength == 2)
	{
		time_temp = (*pcParameter-'0')*10+(*(pcParameter+1)-'0');
		if(time_temp > 60)
		{
			sprintf( pcWriteBuffer, "Error:second number should from 0 to 60" );
			return pdFALSE;
		}
	}
	else
	{
		sprintf( pcWriteBuffer, "Error:second must two number" );
		return pdFALSE;
	}
	temp_calendar->sec = (u8)time_temp;
	RTC_Set(temp_calendar->w_year,temp_calendar->w_month,temp_calendar->w_date,temp_calendar->hour,temp_calendar->min,temp_calendar->sec);
	
	/* Generate a table of task stats. */
	strcpy( pcWriteBuffer, pcHeader );
	sprintf(pcWriteBuffer+strlen(pcHeader),"%4d\t%4d\t%4d\t%4d\t%4d\t%4d",\
							temp_calendar->w_year,temp_calendar->w_month,temp_calendar->w_date,temp_calendar->hour,temp_calendar->min,temp_calendar->sec);
	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}

/*-----------------------------------------------------------*/
static BaseType_t prvGetEvenFlagCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	const char *pcParameter;
	BaseType_t xParameterStringLength;
	pcParameter = FreeRTOS_CLIGetParameter
					(
						pcCommandString,		/* The command string itself. */
						1,						/* Return the first parameter. */
						&xParameterStringLength	/* Store the parameter string length. */
					);
	/* Sanity check something was returned. */
	configASSERT( pcParameter );
	if(*pcParameter == '0')
	{
		sprintf( pcWriteBuffer, "TCP EvenFlag:%x",xEventGroupGetBits(TCPEventGroupHandler));
	}
	else
	{
		sprintf( pcWriteBuffer, "Switch EvenFlag:%x",xEventGroupGetBits(SwitchEventGroupHandler));
	}
	return pdFALSE;
}

/*-----------------------------------------------------------*/

static BaseType_t prvRunTimeStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
const char * const pcHeader = "\r\nTask            Abs Time      % Time\r\n****************************************\r\n";

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	/* Generate a table of task stats. */
	strcpy( pcWriteBuffer, pcHeader );
	vTaskGetRunTimeStats( pcWriteBuffer + strlen( pcHeader ) );

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}
/*-----------------------------------------------------------*/

static BaseType_t prvEEPROM_MachineAddrSaveCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	const char *pcParameter;
	BaseType_t xParameterStringLength;
	uint8_t pbuffer[2];
	uint8_t getnumber;
	uint16_t pAddr;
	
	pcParameter = FreeRTOS_CLIGetParameter
					(
						pcCommandString,		/* The command string itself. */
						1,						/* Return the first parameter. */
						&xParameterStringLength	/* Store the parameter string length. */
					);
	if(xParameterStringLength!=4)
	{
		sprintf( pcWriteBuffer, "Each Machine addr should 4 number");
	}
	else
	{
		EEPROM_Get_Addr(EEPROM_Machine_Addr,pbuffer,&getnumber);
		pAddr = pbuffer[0]<<8|pbuffer[1];
		pbuffer[0] = (pcParameter[0]-'0')*10+(pcParameter[1]-'0');
		pbuffer[1] = (pcParameter[2]-'0')*10+(pcParameter[3]-'0');
		if(EEPROM_Change_Addr(EEPROM_Machine_Addr,pbuffer,2))
			sprintf( pcWriteBuffer  , "Displays previous MachineAddr:%d\r\nDisplays current MachineAddr:%d\r\n",pAddr,pbuffer[0]<<8|pbuffer[1]);
		else
			sprintf( pcWriteBuffer, "Save error,Please try again\r\n");
	}
	return pdFALSE;
}

static BaseType_t prvEEPROM_CurrentAddrSaveCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	const char *pcParameter[CURRENT_NUM];
	BaseType_t xParameterStringLength;
	uint8_t pbuffer[CURRENT_NUM*2];
	uint8_t getnumber,i;
	uint8_t previousAddr[CURRENT_NUM*2];
	
	for(i=0;i<CURRENT_NUM;i++)
	{
		pcParameter[i] = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							i+1,						/* Return the first parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);
		if(xParameterStringLength!=4)
		{
			sprintf( pcWriteBuffer, "Current addr%d should 4 number",i);
			return pdFALSE;
		}
		else
		{
			if((pcParameter[i][0] != '0')||(pcParameter[i][1] != '1'))
			{
				sprintf( pcWriteBuffer, "Current addr begin two number should 01 ,exmaple:0100");
				return pdFALSE;
			}
		}
	}
	EEPROM_Get_Addr(EEPROM_Current_Addr,previousAddr,&getnumber);
	for(i=0;i<CURRENT_NUM;i++)
	{
		pbuffer[0+i*2] = (pcParameter[i][0]-'0')*10+(pcParameter[i][1]-'0');
		pbuffer[1+i*2] = (pcParameter[i][2]-'0')*10+(pcParameter[i][3]-'0');
	}
	if(EEPROM_Change_Addr(EEPROM_Current_Addr,pbuffer,CURRENT_NUM*2))
		sprintf( pcWriteBuffer  , "Displays previous CurrentAddr:%02d%02d %02d%02d %02d%02d %02d%02d\r\nDisplays current CurrentAddr:%02d%02d %02d%02d %02d%02d %02d%02d\r\n",\
																previousAddr[0],previousAddr[1],previousAddr[2],previousAddr[3],previousAddr[4],previousAddr[5],previousAddr[6],previousAddr[7],\
																pbuffer[0],pbuffer[1],pbuffer[2],pbuffer[3],pbuffer[4],pbuffer[5],pbuffer[6],pbuffer[7]);
	else
		sprintf( pcWriteBuffer, "Save error,Please try again\r\n");
	
	
	return pdFALSE;
}


static BaseType_t prvEEPROM_VibratorAddrSaveCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	const char *pcParameter[VIBRATOR_NUM];
	BaseType_t xParameterStringLength;
	uint8_t pbuffer[VIBRATOR_NUM*2];
	uint8_t getnumber,i;
	uint8_t previousAddr[VIBRATOR_NUM*2];
	
	for(i=0;i<VIBRATOR_NUM;i++)
	{
		pcParameter[i] = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							i+1,						/* Return the first parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);
		if(xParameterStringLength!=4)
		{
			sprintf( pcWriteBuffer, "Vibrator addr%d should 4 number",i);
			return pdFALSE;
		}
		else
		{
			if((pcParameter[i][0] != '0')||(pcParameter[i][1] != '3'))
			{
				sprintf( pcWriteBuffer, "Vibrator addr begin two number should 03 ,exmaple:0300");
				return pdFALSE;
			}
		}
	}
	EEPROM_Get_Addr(EEPROM_Vibrator_Addr,previousAddr,&getnumber);
	for(i=0;i<VIBRATOR_NUM;i++)
	{
		pbuffer[0+i*2] = (pcParameter[i][0]-'0')*10+(pcParameter[i][1]-'0');
		pbuffer[1+i*2] = (pcParameter[i][2]-'0')*10+(pcParameter[i][3]-'0');
	}
	if(EEPROM_Change_Addr(EEPROM_Vibrator_Addr,pbuffer,VIBRATOR_NUM*2))
		sprintf( pcWriteBuffer  , "Displays previous VibratorAddr:%02d%02d %02d%02d\r\nDisplays current VibratorAddr:%02d%02d %02d%02d\r\n",\
																previousAddr[0],previousAddr[1],previousAddr[2],previousAddr[3],\
																pbuffer[0],pbuffer[1],pbuffer[2],pbuffer[3]);
	else
		sprintf( pcWriteBuffer, "Save error,Please try again\r\n");
	
	return pdFALSE;
}
static BaseType_t prvEEPROM_InfraredTempAddrSaveCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	const char *pcParameter[INFRAREDTEMP_NUM];
	BaseType_t xParameterStringLength;
	uint8_t pbuffer[INFRAREDTEMP_NUM*2];
	uint8_t getnumber,i;
	uint8_t previousAddr[INFRAREDTEMP_NUM*2];
	
	for(i=0;i<INFRAREDTEMP_NUM;i++)
	{
		pcParameter[i] = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							i+1,						/* Return the first parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);
		if(xParameterStringLength!=4)
		{
			sprintf( pcWriteBuffer, "InfraredTemp addr%d should 4 number",i);
			return pdFALSE;
		}
		else
		{
			if((pcParameter[i][0] != '0')||(pcParameter[i][1] != '2'))
			{
				sprintf( pcWriteBuffer, "InfraredTemp addr begin two number should 02 ,exmaple:0200");
				return pdFALSE;
			}
		}
	}
	EEPROM_Get_Addr(EEPROM_InfraredTemp_Addr,previousAddr,&getnumber);
	for(i=0;i<INFRAREDTEMP_NUM;i++)
	{
		pbuffer[0+i*2] = (pcParameter[i][0]-'0')*10+(pcParameter[i][1]-'0');
		pbuffer[1+i*2] = (pcParameter[i][2]-'0')*10+(pcParameter[i][3]-'0');
	}
	if(EEPROM_Change_Addr(EEPROM_InfraredTemp_Addr,pbuffer,INFRAREDTEMP_NUM*2))
		sprintf( pcWriteBuffer  , "Displays previous InfraredTempAddr:%02d%02d %02d%02d\r\nDisplays current InfraredTempAddr:%02d%02d %02d%02d\r\n",\
																previousAddr[0],previousAddr[1],previousAddr[2],previousAddr[3],\
																pbuffer[0],pbuffer[1],pbuffer[2],pbuffer[3]);
	else
		sprintf( pcWriteBuffer, "Save error,Please try again\r\n");
	
	return pdFALSE;
}

static BaseType_t prvSearchDS18B20Command( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	const char *pcParameter;
	BaseType_t xParameterStringLength;
	uint8_t searchnum,savelen,i,j,k,temp,m=0;
	uint8_t ROM_Buff[MAXNUM][8]; 
	char cDS18B20Rom[200];
	uint8_t Unfind[MAXNUM][8];
	
	Search_DS18B20(&searchnum,ROM_Buff);
	sprintf( pcWriteBuffer  ,"DS18B20 online number is %d\r\n",searchnum);
	for(i=0;i<searchnum;i++)
	{
		sprintf( cDS18B20Rom ,"The %d Rom code is %02X%02X%02X%02X%02X%02X%02X%02X \r\n",i+1,ROM_Buff[i][0],ROM_Buff[i][1],ROM_Buff[i][2],ROM_Buff[i][3],\
																		ROM_Buff[i][4],ROM_Buff[i][5],ROM_Buff[i][6],ROM_Buff[i][7]);
		strcat(pcWriteBuffer,cDS18B20Rom);
	}
	pcParameter = FreeRTOS_CLIGetParameter
					(
						pcCommandString,		/* The command string itself. */
						1,						/* Return the first parameter. */
						&xParameterStringLength	/* Store the parameter string length. */
					);
	if(EEPROM_Get_Addr(EEPROM_DS18B20_Addr,(uint8_t *)cDS18B20Rom,&savelen))
	{
		if(cDS18B20Rom[0]<searchnum)
		{
			for(i=0;i<searchnum;i++)
			{
				for(k=0;k<cDS18B20Rom[0];k++)
				{
					for(j=0;j<8;j++)
					{
						if(cDS18B20Rom[1+k*8+j] != ROM_Buff[i][j])
						{
							temp = 1;
							break;
						}
						else
						{
							temp = 0;
						}
					}
					if(!temp)
					{
						break;
					}
				}
				if(temp)
				{
					for(j=0;j<8;j++)
					{
						Unfind[m][j] = ROM_Buff[i][j];
					}
					m++;
				}
			}
		}
	}
	savelen = cDS18B20Rom[0];
	cDS18B20Rom[0] = searchnum;
	for(i=0;i<m;i++)
	{
		for(j=0;j<8;j++)
		{
			cDS18B20Rom[savelen*8+1+i*8+j] = Unfind[i][j];
		}
	}
	if(*pcParameter == '0')
	{
		if(EEPROM_Change_Addr(EEPROM_DS18B20_Addr,(uint8_t *)cDS18B20Rom,searchnum*8+1))
		{
			strcat(pcWriteBuffer,"DS18B20 save OK\r\n");
		}
		for(i=0;i<m;i++)
		{
			sprintf( cDS18B20Rom ,"The new %d Rom code is %02X%02X%02X%02X%02X%02X%02X%02X \r\n",i+1,Unfind[i][0],Unfind[i][1],Unfind[i][2],Unfind[i][3],\
																			Unfind[i][4],Unfind[i][5],Unfind[i][6],Unfind[i][7]);
			strcat(pcWriteBuffer,cDS18B20Rom);
		}
	}
	else
	{
		cDS18B20Rom[0] = searchnum;
		for(i=0;i<searchnum;i++)
		{
			for(j=0;j<8;j++)
			{
				cDS18B20Rom[1+i*8+j] = ROM_Buff[i][j];
			}
		}
		if(EEPROM_Change_Addr(EEPROM_DS18B20_Addr,(uint8_t *)cDS18B20Rom,searchnum*8+1))
		{
			strcat(pcWriteBuffer,"DS18B20 refresh save OK");
		}
	}
	return pdFALSE;
}

static BaseType_t prvEEPROM_SendTimeSaveCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	const char *pcParameter[6];
	BaseType_t xParameterStringLength;
	uint8_t i;
	uint16_t temp[6];
	uint8_t temp1[2];
	
	
	for(i=0;i<6;i++)
	{
		pcParameter[i] = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							i+1,						/* Return the first parameter. */
							&xParameterStringLength	/* Store the parameter string length. */
						);
		if(xParameterStringLength!=2)
		{
			sprintf( pcWriteBuffer, "send time %d must 2 number",i);
			return pdFALSE;
		}
	}
	for(i=0;i<6;i++)
	{
		temp[0+i] = (pcParameter[i][0]-'0')*10 + (pcParameter[i][1]-'0');
	}
	for(i=0;i<6;i++)
	{
		temp1[0] = temp[i]<<8;
		temp1[1] = (uint8_t)temp[i];
		if(!EEPROM_Change_Addr((EEPROM_Save_TypeDef)(EEPROM_DS18B20_SendTime+i),temp1,2))
		{
			sprintf( pcWriteBuffer  , "Save Error\r\n");
			return pdFALSE;
		}
	}
	sprintf( pcWriteBuffer  , "Displays Send time:%d %d %d %d %d %d\r\n",temp[0],temp[1],temp[2],temp[3],temp[4],temp[5]);
	return pdFALSE;
}

