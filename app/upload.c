/**
  ******************************************************************************
  * @file    app/upload.c 
  * @author  YangLi
  * @version V1.0
  * @date    10/10/2019
  * @brief   This file provides the software which allows to upload an image 
  *          from internal Flash.
  ******************************************************************************
  * @copy
  *
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
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Upload a file via serial port.
  * @param  None
  * @retval None
  */
void SerialUpload(void)
{
  uint32_t status = 0; 

  SerialPutString("\n\n\rSelect Receive File ... (press any key to abort)\n\r");

  if (GetKey() == CRC16)
  {
    /* Transmit the flash image through ymodem protocol */
    status = Ymodem_Transmit((uint8_t*)ApplicationAddress, (const uint8_t*)"UploadedFlashImage.bin", FLASH_IMAGE_SIZE);

    if (status != 0) 
    {
      SerialPutString("\n\rError Occured while Transmitting File\n\r");
    }
    else
    {
      SerialPutString("\n\rFile Trasmitted Successfully \n\r");
    }
  }
  else
  {
    SerialPutString("\r\n\nAborted by user.\n\r");  
  }
 
}

/**
  * @}
  */

/*******************(C)COPYRIGHT 2010 STMicroelectronics *****END OF FILE******/
