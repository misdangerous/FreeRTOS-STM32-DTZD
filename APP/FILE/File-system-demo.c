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

/*******************************************************************************
 * See the URL in the comments within main.c for the location of the online
 * documentation.
 ******************************************************************************/

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"

/* File system includes. */
#include "fat_sl.h"
#include "api_mdriver_sd.h"
#include "usart.h"
#include "rtc.h"
#include "communication_tcp.h"
#include "DS18B20_GetTemperature.h"
#include "sense_modbus.h"
#include "Switch_Task.h"

/* 8.3 format, plus null terminator. */
#define fsMAX_FILE_NAME_LEN				20

/* The number of bytes read/written to the example files at a time. */
#define fsRAM_BUFFER_SIZE 				1024

/* The number of bytes written to the file that uses f_putc() and f_getc(). */
#define fsPUTC_FILE_SIZE				100

/*-----------------------------------------------------------*/

/*
 * Creates and verifies different files on the volume, demonstrating the use of
 * various different API functions.
 */
void vCreateAndVerifySampleFiles( void );


/*
 * Create an example file in a sub-directory using f_putc().
 */
static void prvCreateDemoFileUsing_f_putc( void );
/*
*
*/
static void prvCreatSenseDir(uint8_t function);

static void prvDataInformationWrite(char *pcRAMBuffer,ReportCode_TypeDef pfunction);
/*-----------------------------------------------------------*/

/* A buffer used to both create content to write to disk, and read content back
from a disk.  Note there is no mutual exclusion on this buffer. */
static char cRAMBuffer[ fsRAM_BUFFER_SIZE ];

#define CURRENT_DIR   0
#define INFRARED_DIR  1
#define DS18B20_DIR   2
#define VIBRATOR_DIR  3
#define SWITCH_DIR    4
#define ERROR_DIR     5

/* Names of directories that are created. */
static const char  *pcDirectory[6] = {"Current","Infrared","Normal","Vibrator","SWITCH","Error"};


/*-----------------------------------------------------------*/

void vCreateAndVerifySampleFiles( void )
{
unsigned char ucStatus;

	/* First create the volume. */
	ucStatus = f_initvolume( sd_initfunc );

	/* It is expected that the volume is not formatted. */
	if( ucStatus == F_ERR_NOTFORMATTED )
	{
		/* Format the created volume. */
		ucStatus = f_format( F_FAT12_MEDIA );
	}

	if( ucStatus == F_NO_ERROR )
	{
		prvCreateDemoFileUsing_f_putc();
	}
}

/*-----------------------------------------------------------*/

static void prvCreateDemoFileUsing_f_putc( void )
{
	uint8_t i;
	unsigned char ucReturn;
	char cFileName[ fsMAX_FILE_NAME_LEN ];
	/* Obtain and print out the working directory. */
	f_getcwd( cFileName, fsMAX_FILE_NAME_LEN );
	Computer_485_ModeTx();
	printf( "In directory %s\r\n", cFileName );
	Computer_485_ModeRx();
	
	for(i=0;i<6;i++)
	{
		ucReturn = f_mkdir( pcDirectory[i] );
		configASSERT( ucReturn == F_NO_ERROR );
	}
	for(i=0;i<6;i++)
	{
		prvCreatSenseDir(i);
	}
	
	Computer_485_ModeTx();
	printf( "Create OK!\r\n" );
	Computer_485_ModeRx();
}

/*-----------------------------------------------------------*/

void File_Add_Data(_calendar_obj *pCalendar,ReportCode_TypeDef function)
{
	unsigned char ucReturn;
	F_FILE *pxFile;
	char cFileName[ fsMAX_FILE_NAME_LEN ];
	switch(function)
	{
		case DS18B20_Data:
			sprintf( cFileName, "/%s", pcDirectory[DS18B20_DIR]);
			/* Move into the DS18B20 sub-directory. */
			ucReturn = f_chdir( cFileName );
			break;
		case InfraredTemp_Data:
			sprintf( cFileName, "/%s", pcDirectory[INFRARED_DIR] );
			/* Move into the InfraredTemp sub-directory. */
			ucReturn = f_chdir( cFileName );
			break;
		case Current_Data:
			sprintf( cFileName, "/%s", pcDirectory[CURRENT_DIR] );
			/* Move into the InfraredTemp sub-directory. */
			ucReturn = f_chdir( cFileName );
			break;
		case Vibrator_Data:
			sprintf( cFileName, "/%s", pcDirectory[VIBRATOR_DIR] );
			/* Move into the InfraredTemp sub-directory. */
			ucReturn = f_chdir( cFileName );
			break;
		case Switch_Data:
			sprintf( cFileName, "/%s", pcDirectory[SWITCH_DIR]);
			/* Move into the InfraredTemp sub-directory. */
			ucReturn = f_chdir( cFileName );
			break;
		case Error_Code:
			sprintf( cFileName, "/%s", pcDirectory[ERROR_DIR]);
			/* Move into the InfraredTemp sub-directory. */
			ucReturn = f_chdir( cFileName );
			break;
		default:
			break;
	}
	configASSERT( ucReturn == F_NO_ERROR );
	
	sprintf( cFileName, "%04d%02d%02d.csv",calendar.w_year , calendar.w_month , calendar.w_date );
	/* Open the file, creating the file if it does not already exist. */
	pxFile = f_open( cFileName, "a" );
	configASSERT( pxFile );
	sprintf( cRAMBuffer, "%02d:%02d:%02d," , calendar.hour , calendar.min , calendar.sec );
	////////////////////////////////////////////////////////////////////////////////////////////////
	prvDataInformationWrite(cRAMBuffer,function);
	/////////////////////////////////////////////////////////////////
	strcat(cRAMBuffer,"\r\n");
	ucReturn = f_write(cRAMBuffer,strlen(cRAMBuffer),1,pxFile);
	configASSERT( ucReturn == 1 );
	/* Close the file. */
	f_close( pxFile );
	/* Move back to the root directory. */
	ucReturn = f_chdir( "/" );
	configASSERT( ucReturn == F_NO_ERROR );
}
/*-----------------------------------------------------------*/

static void prvDataInformationWrite(char *pcRAMBuffer,ReportCode_TypeDef pfunction)
{
	char DataBuffer[200];
	uint8_t i,j;
	switch(pfunction)
	{
		case DS18B20_Data:
			for(i=0;i<DS18B20Temp.SetNum;i++)
			{
				sprintf(DataBuffer,"%02x%02x%02x%02x%02x%02x,%.2f,",DS18B20Temp.TempSense[i].ID[1],DS18B20Temp.TempSense[i].ID[2],DS18B20Temp.TempSense[i].ID[3],\
														DS18B20Temp.TempSense[i].ID[4],DS18B20Temp.TempSense[i].ID[5],DS18B20Temp.TempSense[i].ID[6],\
														((float)DS18B20Temp.TempSense[i].Data/100) );
				strcat(pcRAMBuffer,DataBuffer);
			}
			break;
		case InfraredTemp_Data:
			for(i=0;i<INFRAREDTEMP_NUM;i++)
			{
				sprintf(DataBuffer,"%02d%02d,%d,",Sense_Data.InfraredTemp[i].Addr>>8, (uint8_t)Sense_Data.InfraredTemp[i].Addr, Sense_Data.InfraredTemp[i].Data);
				strcat(pcRAMBuffer,DataBuffer);
			}
			break;
		case Current_Data:
			for(i=0;i<CURRENT_NUM;i++)
			{
				sprintf(DataBuffer,"%02d%02d,%.1f,",Sense_Data.Current[i].Addr>>8, (uint8_t)Sense_Data.Current[i].Addr, (float)Sense_Data.Current[i].Data / 10);
				strcat(pcRAMBuffer,DataBuffer);
			}
			break;
		case Vibrator_Data:
			for(i=0;i<VIBRATOR_NUM;i++)
			{
				sprintf(DataBuffer,"%02d%02d,",Sense_Data.Vibrator[i].Addr>>8, (uint8_t)Sense_Data.Vibrator[i].Addr);
				strcat(pcRAMBuffer,DataBuffer);
				for(j=0;j<3;j++)
				{
					sprintf(DataBuffer,"%.4f,",(float)Sense_Data.Vibrator[i].AverData[j] / 10000);
					strcat(pcRAMBuffer,DataBuffer);
				}
				for(j=0;j<3;j++)
				{
					sprintf(DataBuffer,"%.4f,",(float)Sense_Data.Vibrator[i].MaxData[j] / 10000);
					strcat(pcRAMBuffer,DataBuffer);
				}
			}
			break;
		case Switch_Data:
			for(i=0;i<SWITCH_INPUT_NUM;i++)
			{
				sprintf(DataBuffer,"IN%d,%d,",i,Switch_Struction.IN[i]);
				strcat(pcRAMBuffer,DataBuffer);
			}
			for(i=0;i<SWITCH_OUTPUT_NUM;i++)
			{
				sprintf(DataBuffer,"OUT%d,%d,",i,Switch_Struction.OUT[i]);
				strcat(pcRAMBuffer,DataBuffer);
			}
			break;
		default:
			break;
	}
}

/*-----------------------------------------------------------*/
static void prvCreatSenseFile(uint8_t function)
{
	unsigned char ucReturn;
	F_FILE *pxFile;
	char cFileName[ fsMAX_FILE_NAME_LEN ];
	F_FIND xFindStruct;
	/* Generate the file name. */
	sprintf( cFileName, "%04d%02d%02d.csv",calendar.w_year , calendar.w_month , calendar.w_date );
	
	/*查找文件是否存在，新建文件需要添加表头*/
	if(F_NO_ERROR != f_findfirst(cFileName,&xFindStruct))
	{
		/* Open the file, creating the file if it does not already exist. */
		pxFile = f_open( cFileName, "w" );
		configASSERT( pxFile );
		/*增加表头*/
		ucReturn = f_rewind(pxFile);
		configASSERT( ucReturn == F_NO_ERROR );
		if(function == VIBRATOR_DIR)
			sprintf( cRAMBuffer, "time,code,x_average,y_average,z_average,x_max,y_max,z_max\r\n" );
		else if(function == ERROR_DIR)
		{
			sprintf( cRAMBuffer, "time,error code\r\n" );
		}
		else
		{
			sprintf( cRAMBuffer, "time,code1,data1,code2,data2,coden,datan\r\n" );
		}
		ucReturn = f_write(cRAMBuffer,strlen(cRAMBuffer),1,pxFile);
		configASSERT( ucReturn == 1 );
		/* Close the file. */
		f_close( pxFile );
	}
	/* Move back to the root directory. */
	ucReturn = f_chdir( "/" );
	configASSERT( ucReturn == F_NO_ERROR );
}

/*-----------------------------------------------------------*/

static void prvCreatSenseDir(uint8_t function)
{
	unsigned char ucReturn;
	char cFileName[ fsMAX_FILE_NAME_LEN ];
	switch(function)
	{
		case DS18B20_DIR:
			sprintf( cFileName, "/%s", pcDirectory[DS18B20_DIR]);
			ucReturn = f_chdir( cFileName );
			prvCreatSenseFile(DS18B20_DIR);
			break;
		case INFRARED_DIR:
			sprintf( cFileName, "/%s", pcDirectory[INFRARED_DIR]);
			ucReturn = f_chdir( cFileName );
			prvCreatSenseFile(INFRARED_DIR);
			break;
		case CURRENT_DIR:
			sprintf( cFileName, "/%s", pcDirectory[CURRENT_DIR] );
			ucReturn = f_chdir( cFileName );
			prvCreatSenseFile(CURRENT_DIR);
			break;
		case VIBRATOR_DIR:
			sprintf( cFileName, "/%s", pcDirectory[VIBRATOR_DIR] );
			ucReturn = f_chdir( cFileName );
			prvCreatSenseFile(VIBRATOR_DIR);
			break;
		case SWITCH_DIR:
			sprintf( cFileName, "/%s", pcDirectory[SWITCH_DIR]);
			ucReturn = f_chdir( cFileName );
			prvCreatSenseFile(SWITCH_DIR);
			break;
		case ERROR_DIR:
			sprintf( cFileName, "/%s", pcDirectory[ERROR_DIR]);
			ucReturn = f_chdir( cFileName );
			prvCreatSenseFile(ERROR_DIR);
			break;
	}
	configASSERT( ucReturn == F_NO_ERROR );
	ucReturn = f_chdir( "/" );
	configASSERT( ucReturn == F_NO_ERROR );
}


