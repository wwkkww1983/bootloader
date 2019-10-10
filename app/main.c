/**
  ******************************************************************************
  * @file    IAP/src/main.c 
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    10/15/2010
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/** @addtogroup IAP
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "bsp_usart.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern pFunction Jump_To_Application;
extern uint32_t JumpAddress;

/* Private function prototypes -----------------------------------------------*/
static void IAP_Init(void);

#define  MS_BASE    5140

uint8_t GetKey_Timeout(void)
{
	uint8_t key = 0;
	uint32_t timeout = 514000;
	/* Waiting for user input */
	while (timeout--)
	{
		if (SerialKeyPressed((uint8_t*)&key)) break;
	}

	return key;
}

void SerialPutCharTimeout(uint8_t c)
{
	uint32_t timeout = 514000;
	USART_SendData(USART_1, c);
	while ( ( USART_GetFlagStatus(USART_1, USART_FLAG_TXE) == RESET ) && ( timeout ) )
	{
		timeout --;
	}
}

static int8_t waitIAP(void)
{
	uint8_t key = 0;
	uint8_t timeout = 10;

	while(timeout--)
	{
		SerialPutCharTimeout(0xA5);
		SerialPutString("\r\nsend iap request per 500ms");
		
		if( (key = GetKey_Timeout()) == 0x5A )
		{
		    SerialPutString("\r\nrec remote ack");
			return 1;
		}
		else
		    SerialPutString("\r\nno ack");
		printf("%x", key);
	}
	
	SerialPutString("\r\n10s timeout, no ack ,jump to app");
	return 0;
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	uint8_t key = 0;
	
	/* Flash unlock */
	FLASH_Unlock();
	IAP_Init();
	SerialPutString("\r\nselect op, 1->flash, 2->run app");
//	while(1)
//	{
//		printf("\r\nsend 0xA5");
//		SerialPutCharTimeout(0xA5);
//		delay_ms(500);
//	}
	
//	while( 1 )
//	{
//	    if( (key = GetKey()) )
//		{
//		    printf("%c\r\n", key);
//		}
//	}
	
	if( waitIAP() )
	{
//		SerialPutString("\r\n======================================================================");
//		SerialPutString("\r\n=              (C) COPYRIGHT 2010 STMicroelectronics                 =");
//		SerialPutString("\r\n=                                                                    =");
//		SerialPutString("\r\n=     In-Application Programming Application  (Version 3.3.0)        =");
//		SerialPutString("\r\n=                                                                    =");
//		SerialPutString("\r\n=                                   By MCD Application Team          =");
//		SerialPutString("\r\n======================================================================");
//		SerialPutString("\r\n\r\n");
		SerialPutString("\r\nmain menu");

		Main_Menu (); 
	}
	/* Keep the user application running */
	else
	{
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

  while (1)
  {}
}

/**
  * @brief  Initialize the IAP: Configure RCC, USART and GPIOs.
  * @param  None
  * @retval None
  */
void IAP_Init(void)
{
	USART_Config();
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
