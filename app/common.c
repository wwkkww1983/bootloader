/**
  ******************************************************************************
  * @file    app/common.c 
  * @author  YangLi
  * @version V1.0
  * @date    10/10/2019
  * @brief   This file provides all the common functions.
  ******************************************************************************
  * @copy
  *
  */ 

/** @addtogroup IAP
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include <stm32f10x.h>
#include "common.h"
#include "ymodem.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
pFunction Jump_To_Application;
uint32_t JumpAddress;
uint32_t BlockNbr = 0, UserMemoryMask = 0;
__IO uint32_t FlashProtection = 0;
extern uint32_t FlashDestination;

/* Private function prototypes -----------------------------------------------*/
static uint16_t FLASH_ReadHalfWord(uint32_t address);
static void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite);
static void FLASH_ReadMoreData(uint32_t startAddress,uint16_t *readData,uint16_t countToRead);
static void UpdateApp(void);
/* Private functions ---------------------------------------------------------*/
static uint16_t FLASH_ReadHalfWord(uint32_t address)
{
  return *(__IO uint16_t*)address; 
}

static void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite)
{    
	uint32_t offsetAddress=startAddress - FLASH_BASE;  
	uint32_t sectorPosition=offsetAddress/PAGE_SIZE;  
	uint32_t sectorStartAddress=sectorPosition*PAGE_SIZE+FLASH_BASE; 
	uint16_t dataIndex;

	if(startAddress<FLASH_BASE||((startAddress+countToWrite*2)>=(FLASH_BASE + PAGE_SIZE * FLASH_SIZE)))
	{
		SerialPutString("in flash write more data, first return");
		return;
	}
	
	FLASH_Unlock(); 

	FLASH_ErasePage(startAddress);

	for(dataIndex=0;dataIndex<countToWrite;dataIndex++)
	{
		FLASH_ProgramHalfWord(startAddress+dataIndex*2,writeData[dataIndex]);
	}

	FLASH_Lock();
}

static void FLASH_ReadMoreData(uint32_t startAddress,uint16_t *readData,uint16_t countToRead)
{
	uint16_t dataIndex;
	for(dataIndex=0;dataIndex<countToRead;dataIndex++)
	{
		readData[dataIndex]=FLASH_ReadHalfWord(startAddress);
		startAddress += 2;
	}
}

static void UpdateApp(void)
{
	uint16_t buff_2k[1024];
	uint16_t size = 2048;
	uint32_t cnt = ApplicationRegionSize/2048; // 200k/2k = 100次
	uint32_t i;
	
	uint32_t rd_addr = DownloadAddress;
	uint32_t wr_addr = ApplicationAddress;
	
	for(i = 0; i < cnt; i++)
	{
		// read for down region
		FLASH_ReadMoreData(rd_addr, buff_2k, size/2);
		
		// write to app region
		FLASH_WriteMoreData(wr_addr, buff_2k, size/2);
		
		rd_addr += size;
		wr_addr += size;
	}
}






void delay_ms( __IO uint32_t _T )
{
	__IO uint32_t i;
	while ( _T-- )
//		for ( i = 0; i < 10280; i++);
	for ( i = 0; i < 5140; i++);
}

/**
  * @brief  Convert an Integer to a string
  * @param  str: The string
  * @param  intnum: The intger to be converted
  * @retval None
  */
void Int2Str(uint8_t* str, int32_t intnum)
{
  uint32_t i, Div = 1000000000, j = 0, Status = 0;

  for (i = 0; i < 10; i++)
  {
    str[j++] = (intnum / Div) + 48;

    intnum = intnum % Div;
    Div /= 10;
    if ((str[j-1] == '0') & (Status == 0))
    {
      j = 0;
    }
    else
    {
      Status++;
    }
  }
}

/**
  * @brief  Convert a string to an integer
  * @param  inputstr: The string to be converted
  * @param  intnum: The intger value
  * @retval 1: Correct
  *         0: Error
  */
uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum)
{
  uint32_t i = 0, res = 0;
  uint32_t val = 0;

  if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
  {
    if (inputstr[2] == '\0')
    {
      return 0;
    }
    for (i = 2; i < 11; i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1; */
        res = 1;
        break;
      }
      if (ISVALIDHEX(inputstr[i]))
      {
        val = (val << 4) + CONVERTHEX(inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* over 8 digit hex --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }
  else /* max 10-digit decimal input */
  {
    for (i = 0;i < 11;i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1 */
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
      {
        val = val << 10;
        *intnum = val;
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
      {
        val = val << 20;
        *intnum = val;
        res = 1;
        break;
      }
      else if (ISVALIDDEC(inputstr[i]))
      {
        val = val * 10 + CONVERTDEC(inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* Over 10 digit decimal --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }

  return res;
}

/**
  * @brief  Get an integer from the HyperTerminal
  * @param  num: The inetger
  * @retval 1: Correct
  *         0: Error
  */
uint32_t GetIntegerInput(int32_t * num)
{
  uint8_t inputstr[16];

  while (1)
  {
    GetInputString(inputstr);
    if (inputstr[0] == '\0') continue;
    if ((inputstr[0] == 'a' || inputstr[0] == 'A') && inputstr[1] == '\0')
    {
      SerialPutString("User Cancelled \r\n");
      return 0;
    }

    if (Str2Int(inputstr, num) == 0)
    {
      SerialPutString("Error, Input again: \r\n");
    }
    else
    {
      return 1;
    }
  }
}

/**
  * @brief  Test to see if a key has been pressed on the HyperTerminal
  * @param  key: The key pressed
  * @retval 1: Correct
  *         0: Error
  */
uint32_t SerialKeyPressed(uint8_t *key)
{

  if ( USART_GetFlagStatus(USART_1, USART_FLAG_RXNE) != RESET)
  {
    *key = (uint8_t)USART_1->DR;
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
  * @brief  Get a key from the HyperTerminal
  * @param  None
  * @retval The Key Pressed
  */
uint8_t GetKey(void)
{
	uint8_t key = 0;
	/* Waiting for user input */
	while (1)
	{
		if (SerialKeyPressed((uint8_t*)&key)) break;
	}
	return key;
}

/*
 * 获取远程数据，超时时间为100ms
 */
uint8_t GetKey_Timeout(void)
{
	uint8_t key = 0;
	uint32_t timeout = 102800;  // 100ms timeout
	/* Waiting for user input */
	while (timeout--)
	{
		if (SerialKeyPressed((uint8_t*)&key)) break;
	}

	return key;
}

/*
 * 向远程发送cmd指令，等待返回指令，超时为sTimeout
 * timeoutS 单位是秒
 */
int8_t waitHostAck(uint8_t cmd, uint8_t wanted_ack, uint32_t timeoutS, uint8_t *actual_ack)
{
	uint8_t key = 0;
	uint32_t timeout = timeoutS*1000/100;
	uint32_t cnt = 0;

	LOG_INFO_APP("\r\nsend cmd(%#X) per 100ms", cmd);
	while(timeout--)
	{
		LOG_INFO_APP_1("\r\nsend %#X......", cmd);
		SerialPutChar( cmd );
		cnt ++;
		
		if( (key = GetKey_Timeout()) == wanted_ack )
		{
			*actual_ack = key;
		    LOG_INFO_APP_1("read: %#X, rec remote iap ack", key);
			return 1;
		}
		else if( key == HOST_CMD_EXEC_APP_ACK )
		{
			*actual_ack = key;
		    LOG_INFO_APP("read: %#X, exec app", key);
			return 0;
		}
		else if( key == HOST_CMD_EXEC_APP_ACK )
		{
			*actual_ack = key;
		    LOG_INFO_APP("read: %#X, exec app", key);
			return 0;
		}
		else
		{
			*actual_ack = key;
		    LOG_INFO_APP_1("read: %#X, no ack, cnt:%u,", key, cnt);
		}
	}
	
	LOG_INFO_APP_1("\r\n%ds timeout", timeoutS);
	return 0;
}

/**
  * @brief  Print a character on the HyperTerminal
  * @param  c: The character to be printed
  * @retval None
  */
void SerialPutChar(uint8_t c)
{
  USART_SendData(USART_1, c);
  while ( USART_GetFlagStatus(USART_1, USART_FLAG_TXE) == RESET )
  {
  }
}

/**
  * @brief  Print a string on the HyperTerminal
  * @param  s: The string to be printed
  * @retval None
  */
void Serial_PutString(uint8_t *s)
{
	Usart_SendString(UART_4, (char *)s);
}

/**
  * @brief  Get Input string from the HyperTerminal
  * @param  buffP: The input string
  * @retval None
  */
void GetInputString (uint8_t * buffP)
{
  uint32_t bytes_read = 0;
  uint8_t c = 0;
  do
  {
    c = GetKey();
    if (c == '\r')
      break;
    if (c == '\b') /* Backspace */
    {
      if (bytes_read > 0)
      {
        SerialPutString("\b \b");
        bytes_read --;
      }
      continue;
    }
    if (bytes_read >= CMD_STRING_SIZE )
    {
      SerialPutString("Command string size overflow\r\n");
      bytes_read = 0;
      continue;
    }
    if (c >= 0x20 && c <= 0x7E)
    {
      buffP[bytes_read++] = c;
      SerialPutChar(c);
    }
  }
  while (1);
  SerialPutString(("\n\r"));
  buffP[bytes_read] = '\0';
}

/**
  * @brief  Calculate the number of pages
  * @param  Size: The image size
  * @retval The number of pages
  */
uint32_t FLASH_PagesMask(__IO uint32_t Size)
{
  uint32_t pagenumber = 0x0;
  uint32_t size = Size;

  if ((size % PAGE_SIZE) != 0)
  {
    pagenumber = (size / PAGE_SIZE) + 1;
  }
  else
  {
    pagenumber = size / PAGE_SIZE;
  }
  return pagenumber;
}

/**
  * @brief  Disable the write protection of desired pages
  * @param  None
  * @retval None
  */
void FLASH_DisableWriteProtectionPages(void)
{
  uint32_t useroptionbyte = 0, WRPR = 0;
  uint16_t var1 = OB_IWDG_SW, var2 = OB_STOP_NoRST, var3 = OB_STDBY_NoRST;
  FLASH_Status status = FLASH_BUSY;

  WRPR = FLASH_GetWriteProtectionOptionByte();

  /* Test if user memory is write protected */
  if ((WRPR & UserMemoryMask) != UserMemoryMask)
  {
    useroptionbyte = FLASH_GetUserOptionByte();

    UserMemoryMask |= WRPR;

    status = FLASH_EraseOptionBytes();

    if (UserMemoryMask != 0xFFFFFFFF)
    {
      status = FLASH_EnableWriteProtection((uint32_t)~UserMemoryMask);
    }

    /* Test if user Option Bytes are programmed */
    if ((useroptionbyte & 0x07) != 0x07)
    { 
      /* Restore user Option Bytes */
      if ((useroptionbyte & 0x01) == 0x0)
      {
        var1 = OB_IWDG_HW;
      }
      if ((useroptionbyte & 0x02) == 0x0)
      {
        var2 = OB_STOP_RST;
      }
      if ((useroptionbyte & 0x04) == 0x0)
      {
        var3 = OB_STDBY_RST;
      }

      FLASH_UserOptionByteConfig(var1, var2, var3);
    }

    if (status == FLASH_COMPLETE)
    {
      SerialPutString("Write Protection disabled...\r\n");

      SerialPutString("...and a System Reset will be generated to re-load the new option bytes\r\n");

      /* Generate System Reset to load the new option byte values */
      NVIC_SystemReset();
    }
    else
    {
      SerialPutString("Error: Flash write unprotection failed...\r\n");
    }
  }
  else
  {
    SerialPutString("Flash memory not write protected\r\n");
  }
}

/**
  * @brief  Display the Main Menu on to HyperTerminal
  * @param  None
  * @retval None
  */
void EnterIAP(void)
{
	uint8_t step = 0;
	uint8_t key = 0;
	int8_t ret = 0;
	uint8_t ack;

	/* Get the number of block (4 or 2 pages) from where the user program will be loaded */
	BlockNbr = (FlashDestination - 0x08000000) >> 12;

  /* Compute the mask to test if the Flash memory, where the user program will be
     loaded, is write protected */
#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)
  UserMemoryMask = ((uint32_t)~((1 << BlockNbr) - 1));
#else /* USE_STM3210E_EVAL */
  if (BlockNbr < 62)
  {
    UserMemoryMask = ((uint32_t)~((1 << BlockNbr) - 1));
  }
  else
  {
    UserMemoryMask = ((uint32_t)0x80000000);
  }
#endif /* (STM32F10X_MD) || (STM32F10X_MD_VL) */


  /* Test if any page of Flash memory where program user will be loaded is write protected */
  if ((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)
  {
    FlashProtection = 1;
  }
  else
  {
    FlashProtection = 0;
  }

  while (1)
  {
    if(FlashProtection != 0)
    {
      SerialPutString("  Disable the write protection ------------------------- 4\r\n\n");
    }
    SerialPutString("==========================================================\r\n\n");
    
	// 告诉上位机当前的状态
	if( step == 0 )  // 第一次进来，执行升级，5min超时
	{
		LOG_INFO_APP_1("\r\nrequest exec iap, timeout=5min(300s)");
	    if( waitHostAck( HOST_CMD_EXEC_IAP, HOST_CMD_EXEC_IAP_ACK, 300, &ack ) )
		{
			key = 0x31;
			step = 1;
		}
		else
		{
			if( ack == HOST_CMD_QUERY_IAP_ACK )
			{
			    continue;
			}
			else if( ack == HOST_CMD_EXEC_APP_ACK )
			{
			    key = 0x33;
			}
			else
				key = 0x33;
		}
	}
	else if( step == 1 )  // 升级完毕，跳转到app
	{
		key = 0x33;
	}

	if (key == 0x31)
	{
		/* Download user application in the Flash */
		SerialPutString("\r\nexec iap download");
		ret = SerialDownload();
		switch( ret )
		{
		    case 0:
				step = 1;
				LOG_INFO_APP("ret=%d, download success, now update app", ret);
				UpdateApp();
				LOG_INFO_APP("update app finished, now jump to app");
				break;
			case -1:
				LOG_ERR_APP("ret=%d, download failed, now jump to app", ret);
				JumpToApp();
				break;
			case -2:
				LOG_ERR_APP("ret=%d, download failed, now jump to app", ret);
				JumpToApp();
				break;
			case -3:
				LOG_ERR_APP("ret=%d, download failed, now jump to app", ret);
				JumpToApp();
				break;
			case -4:
				LOG_ERR_APP("ret=%d, download failed, now jump to app", ret);
				JumpToApp();
				break;
			default:
				break;
		}
		
		step = 1;
	}
	else if (key == 0x32)
	{
		/* Upload user application from the Flash */
		SerialPutString("\r\nexec uploader");
		SerialUpload();
	}
    else if (key == 0x33)
    {
		JumpToApp();
    }
    else if ((key == 0x34) && (FlashProtection == 1))
    {
      /* Disable the write protection of desired pages */
      FLASH_DisableWriteProtectionPages();
    }
    else
    {
      if (FlashProtection == 0)
      {
        SerialPutString("Invalid Number ! ==> The number should be either 1, 2 or 3\r");
      }
      else
      {
        SerialPutString("Invalid Number ! ==> The number should be either 1, 2, 3 or 4\r");
      } 
    }
  }
}

void JumpToApp(void)
{
		LOG_INFO_APP("\r\njump to app");
		/* Test if user code is programmed starting from address "ApplicationAddress" */
		if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
		{
			/* Jump to user application */
			JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
			Jump_To_Application = (pFunction) JumpAddress;
			/* Initialize user application's Stack Pointer */
			__set_MSP(*(__IO uint32_t*) ApplicationAddress);
			Jump_To_Application();
		}
}

/**
  * @}
  */

/*******************(C)COPYRIGHT 2010 STMicroelectronics *****END OF FILE******/
