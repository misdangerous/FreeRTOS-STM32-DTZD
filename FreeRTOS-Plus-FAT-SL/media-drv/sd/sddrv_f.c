/*
 * FreeRTOS+FAT SL V1.0.1 (C) 2014 HCC Embedded
 *
 * The FreeRTOS+FAT SL license terms are different to the FreeRTOS license
 * terms.
 *
 * FreeRTOS+FAT SL uses a dual license model that allows the software to be used
 * under a standard GPL open source license, or a commercial license.  The
 * standard GPL license (unlike the modified GPL license under which FreeRTOS
 * itself is distributed) requires that all software statically linked with
 * FreeRTOS+FAT SL is also distributed under the same GPL V2 license terms.
 * Details of both license options follow:
 *
 * - Open source licensing -
 * FreeRTOS+FAT SL is a free download and may be used, modified, evaluated and
 * distributed without charge provided the user adheres to version two of the
 * GNU General Public License (GPL) and does not remove the copyright notice or
 * this text.  The GPL V2 text is available on the gnu.org web site, and on the
 * following URL: http://www.FreeRTOS.org/gpl-2.0.txt.
 *
 * - Commercial licensing -
 * Businesses and individuals who for commercial or other reasons cannot comply
 * with the terms of the GPL V2 license must obtain a commercial license before
 * incorporating FreeRTOS+FAT SL into proprietary software for distribution in
 * any form.  Commercial licenses can be purchased from
 * http://shop.freertos.org/fat_sl and do not require any source files to be
 * changed.
 *
 * FreeRTOS+FAT SL is distributed in the hope that it will be useful.  You
 * cannot use FreeRTOS+FAT SL unless you agree that you use the software 'as
 * is'.  FreeRTOS+FAT SL is provided WITHOUT ANY WARRANTY; without even the
 * implied warranties of NON-INFRINGEMENT, MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE. Real Time Engineers Ltd. and HCC Embedded disclaims all
 * conditions and terms, be they implied, expressed, or statutory.
 *
 * http://www.FreeRTOS.org
 * http://www.FreeRTOS.org/FreeRTOS-Plus
 *
 */

#include "../../api/api_mdriver_sd.h"
#include "usart.h"

#include "sdio_sdcard.h"
#include "sys.h"
#include <string.h>
#include <stdio.h>



/* The F_DRIVER structure that is filled with the sd disk versions of the read
sector, write sector, etc. functions. */
static F_DRIVER  t_driver;

static double long maxsector;

/* Disk not initialized yet. */
static char in_use = 0;


/****************************************************************************
 * Read one sector
 ***************************************************************************/
static int sd_readsector ( F_DRIVER * driver, void * data, unsigned long sector )
{
	u8 res;
  /* Not used. */
  ( void ) driver;

  /* Check for valid sector. */
  if ( sector >= maxsector )
  {
    return MDRIVER_SD_ERR_SECTOR;
  }

  if( in_use == 0 )
  {
    return MDRIVER_SD_ERR_NOTAVAILABLE;
  }
	res=SD_ReadDisk(data,sector,1);	 
	while(res)//读出错
	{
		SD_Init();	//重新初始化SD卡
		res=SD_ReadDisk(data,sector,1);	
		Computer_485_ModeTx();
		printf("sd rd error:%d\r\n",res);
		Computer_485_ModeRx();
	}
	return MDRIVER_SD_NO_ERROR;
}

/****************************************************************************
 * Write one sector
 ***************************************************************************/
static int sd_writesector ( F_DRIVER * driver, void * data, unsigned long sector )
{
	u8 res;

	/* Not used. */
	( void ) driver;

	/* Check for valid sector. */
	if ( sector >= maxsector )
	{
	return MDRIVER_SD_ERR_SECTOR;
	}

	if( in_use == 0 )
	{
	  return MDRIVER_SD_ERR_NOTAVAILABLE;
	}
	res=SD_WriteDisk((u8*)data,sector,1);
	while(res)//写出错
	{
		SD_Init();	//重新初始化SD卡
		res=SD_WriteDisk((u8*)data,sector,1);	
		Computer_485_ModeTx();
		printf("sd wr error:%d\r\n",res);
		Computer_485_ModeRx();
	}
	return MDRIVER_SD_NO_ERROR;
}


/****************************************************************************
 *
 * sd_getphy
 *
 * determinate sddrive physicals
 *
 * INPUTS
 *
 * driver - driver structure
 * phy - this structure has to be filled with physical information
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
static int sd_getphy ( F_DRIVER * driver, F_PHY * phy )
{
  /* Not used. */
  ( void ) driver;
	
  phy->number_of_sectors = maxsector;
  phy->bytes_per_sector = SDCardInfo.CardBlockSize;

  return MDRIVER_SD_NO_ERROR;
}


/****************************************************************************
 *
 * sd_release
 *
 * Releases a drive
 *
 * INPUTS
 *
 * driver_pasd - driver pasdeter
 *
 ***************************************************************************/
static void sd_release ( F_DRIVER * driver )
{
  /* Not used. */
  ( void ) driver;

  /* Disk no longer in use. */
  in_use = 0;
}


/****************************************************************************
 *
 * sd_initfunc
 *
 * this init function has to be passed for highlevel to initiate the
 * driver functions
 *
 * INPUTS
 *
 * driver_pasd - driver pasdeter
 *
 * RETURNS
 *
 * driver structure pointer
 *
 ***************************************************************************/
F_DRIVER * sd_initfunc ( unsigned long driver_pasd )
{
	u8 res;
  ( void ) driver_pasd;

  if( in_use )
    return NULL;

	res = SD_Init();
	if(res == 0)
	{
		maxsector = SDCardInfo.CardCapacity / SDCardInfo.CardBlockSize;
	}
  (void)memset( &t_driver, 0, sizeof( F_DRIVER ) );

  t_driver.readsector = sd_readsector;
  t_driver.writesector = sd_writesector;
  t_driver.getphy = sd_getphy;
  t_driver.release = sd_release;

  in_use = 1;

  return &t_driver;
} /* sd_initfunc */

