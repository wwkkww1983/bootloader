/**
  ******************************************************************************
  * @file    app/download.c 
  * @author  YangLi
  * @version V1.0
  * @date    10/10/2019
  * @brief   This file provides the software which allows to download an image 
  *          to internal Flash.
  ******************************************************************************
  * @copy
  */ 

/** @addtogroup IAP
  * @{
  */
/* Includes ------------------------------------------------------------------*/
#include "common.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint8_t file_name[FILE_NAME_LENGTH];
uint8_t tab_1024[1024] =
  {
    0
  };

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Download a file via serial port
  * @param  None
  * @retval 0, -1, -2
  */
int8_t SerialDownload(void)
{
	uint8_t Number[10] = "          ";
	int32_t Size = 0;

	SerialPutString("Waiting for the file to be sent ... (press 'a' to abort)\n\r");

	/* Flash unlock */
	FLASH_Unlock();
	
	Size = Ymodem_Receive(&tab_1024[0]);
	
	/* Flash locked */
	FLASH_Lock();
	
	if (Size > 0)
	{
		SerialPutString("\n\n\r Programming Completed Successfully!\n\r--------------------------------\r\n Name: ");
		SerialPutString(file_name);
		Int2Str(Number, Size);
		SerialPutString("\n\r Size: ");
		SerialPutString(Number);
		SerialPutString(" Bytes\r\n");
		SerialPutString("-------------------\n");
		return 0;
	}
	else if (Size == -1)
	{
		SerialPutString("\n\n\rThe image size is higher than the allowed space memory!\n\r");
		return -1;
	}
	else if (Size == -2)
	{
		SerialPutString("\n\n\rVerification failed!\n\r");
		return -2;
	}
	else if (Size == -3)
	{
		SerialPutString("\r\n\nAborted by user.\n\r");
		return -3;
	}
	else
	{
		SerialPutString("\n\rFailed to receive the file!\n\r");
		return -4;
	}
}

/**
  * @}
  */

/*******************(C)COPYRIGHT 2010 STMicroelectronics *****END OF FILE******/
