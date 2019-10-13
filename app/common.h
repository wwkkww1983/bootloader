/**
  ******************************************************************************
  * @file    app/common.h 
  * @author  YangLi
  * @version V1.0
  * @date    10/10/2019
  * @brief   This file provides all the headers of the common functions.
  ******************************************************************************
  * @copy
  *
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _COMMON_H
#define _COMMON_H

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "string.h"
#include "stm32f10x.h"
#include "ymodem.h"
#include "bsp_usart.h"

/* Exported types ------------------------------------------------------------*/
typedef  void (*pFunction)(void);

/* Exported constants --------------------------------------------------------*/
/* Constants used by Serial Command Line Mode */
#define CMD_STRING_SIZE       128

#define ApplicationAddress    0x8003000
#define DownloadAddress       0x8035000
#define ReserveddAddress      0x8067000

#define ApplicationRegionSize 0x32000  /* 200Kbyte */
#define DownloadRegionSize    0x32000  /* 200Kbyte */
#define ReservedRegionSize    0x19000  /* 100Kbyte */

#define HOST_CMD_QUERY_IAP 0xA5  // 查询是否执行IAP
#define HOST_CMD_QUERY_IAP_ACK  0x5A

#define HOST_CMD_EXEC_IAP  0xA6  // 告诉远程执行IAP
#define HOST_CMD_EXEC_IAP_ACK  0x6A

#define HOST_CMD_EXEC_APP  0xA7  // 告诉远程跳转到APP
#define HOST_CMD_EXEC_APP_ACK  0x7A

#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)
 #define PAGE_SIZE                         (0x400)    /* 1 Kbyte */
 #define FLASH_SIZE                        (0x20000)  /* 128 KBytes */
#elif defined STM32F10X_CL
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x40000)  /* 256 KBytes */
#elif defined STM32F10X_HD || defined (STM32F10X_HD_VL)
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x80000)  /* 512 KBytes */
#elif defined STM32F10X_XL
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x100000) /* 1 MByte */
#else 
 #error "Please select first the STM32 device to be used (in stm32f10x.h)"    
#endif

/* Compute the FLASH upload image size */  
#define FLASH_IMAGE_SIZE                   (uint32_t) (FLASH_SIZE - (ApplicationAddress - 0x08000000))

/* Exported macro ------------------------------------------------------------*/
/* Common routines */
#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

#define SerialPutString(x) Serial_PutString((uint8_t*)(x))

#define __LOG_INFO_APP    1
#define __LOG_ERR_APP     1
#define LOG_INFO_APP(fmt,arg...)      do{\
                                          if(__LOG_INFO_APP)\
											   printf("\r\n<<-INFO_APP->> "fmt" ",##arg);\
                                          }while(0)
#define LOG_INFO_APP_1(fmt,arg...)      do{\
                                          if(__LOG_INFO_APP)\
											   printf(""fmt" ",##arg);\
                                          }while(0)
#define LOG_ERR_APP(fmt,arg...)      do{\
                                          if(__LOG_INFO_APP)\
											   printf("\r\n<<-ERR_APP->> "fmt" ",##arg);\
                                          }while(0)

/* Exported functions ------------------------------------------------------- */
void Int2Str(uint8_t* str,int32_t intnum);
uint32_t Str2Int(uint8_t *inputstr,int32_t *intnum);
uint32_t GetIntegerInput(int32_t * num);
uint32_t SerialKeyPressed(uint8_t *key);
uint8_t GetKey(void);
void SerialPutChar(uint8_t c);
void Serial_PutString(uint8_t *s);
void GetInputString(uint8_t * buffP);
uint32_t FLASH_PagesMask(__IO uint32_t Size);
void FLASH_DisableWriteProtectionPages(void);
void EnterIAP(void);
void JumpToApp(void);
int8_t SerialDownload(void);
void SerialUpload(void);
void delay_ms( __IO uint32_t _T );
										  
uint8_t GetKey_Timeout(void);										  
int8_t waitHostAck(uint8_t cmd, uint8_t wanted_ack, uint32_t timeoutS, uint8_t *actual_ack);
#endif  /* _COMMON_H */

/*******************(C)COPYRIGHT 2010 STMicroelectronics *****END OF FILE******/
