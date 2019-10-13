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
ÇøÓò»®·Ö£º
boot  app   download  reserved
12K   200K  200k          100k

bootloader region(12k)  : 0x8000000 - 0x8002FFF  
				app region(200k): 0x8003000 - 0x8034FFF
   download region(200k): 0x8035000 - 0x8066FFF
	 reserved region(100k): 0x8067000 - 0x807FFFF
  */

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "bsp_usart.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void IAP_Init(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	uint8_t ack;
	IAP_Init();
	LOG_INFO_APP("\r\nquery is exec iap, timeout=3s");
	if( waitHostAck( HOST_CMD_QUERY_IAP, HOST_CMD_QUERY_IAP_ACK, 3, &ack) )
	{
		LOG_INFO_APP("\r\nenter iap main menu");
		EnterIAP (); 
	}
	/* Keep the user application running */
	else
	{
		JumpToApp();
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
