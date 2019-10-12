/**
  ******************************************************************************
  * @file    app/main.c 
  * @author  YangLi
  * @version V1.0
  * @date    10/10/2019
  * @brief   Main program body
  ******************************************************************************
  * @copy
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

void SerialPutCharTimeout(uint8_t c)
{
	uint32_t timeout = 514000;
	USART_SendData(USART_1, c);
	while ( ( USART_GetFlagStatus(USART_1, USART_FLAG_TXE) == RESET ) && ( timeout ) )
	{
		timeout --;
	}
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	uint8_t ack;
	/* Flash unlock */
	FLASH_Unlock();
	IAP_Init();
	LOG_INFO_APP("\r\nselect op, 1->flash, 2->run app");
	
	LOG_INFO_APP("\r\nquery is exec iap, timeout=3s");
	if( waitHostAck( HOST_CMD_QUERY_IAP, HOST_CMD_QUERY_IAP_ACK, 3, &ack) )
	{
		LOG_INFO_APP("\r\nenter iap main menu");
		EnterIAP (); 
	}
	/* Keep the user application running */
	else
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
