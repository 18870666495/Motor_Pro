/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : UART.h
  * @brief          : UART.c 的共用標頭文件
  * @version        : 1.0
  * @copyright      : Fortior Tech
  ******************************************************************************
  * @attention
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */ 
/* 定義以防止遞迴引入 ----------------------------------------------------------*/

#ifndef __UART_H
#define __UART_H

/* 全域連結定義 ----------------------------------------------------------------*/
/** @addtogroup UART
  * @{
  */

#define UART_STOPBITS_1                (0x00) 
#define UART_STOPBITS_2                (UT2MOD1)

#define UART_PARITY_NONE               (0x00)
#define UART_PARITY_EVEN               (UT2MOD1)
#define UART_PARITY_ODD                (UT2MOD1)

#define UART2_MODE_TX_RX               (UT2MOD0)
#define UART1_MODE_TX_RX               (1) 
#define UART_MODE_ONEWIRE              (0x00)

#define UART_WORDLENGTH_8B             (0x00)
#define UART1_WORDLENGTH_9B            (1)
#define UART2_WORDLENGTH_9B            (UT2MOD1)
 

#define UART2_CHANNEL_0                (0x0000)                        // [RxD:P3.6 TxD:P3.7]
#define UART2_CHANNEL_1                (UART2CH)                       // [RxD:P0.1 TxD:P0.0]
#define UART2_CHANNEL_0_TI             (0x0000 | UART2IEN)             // [RxD:P3.6 TxD:P3.7]
#define UART2_CHANNEL_1_TI             (UART2CH | UART2IEN)            // [RxD:P0.1 TxD:P0.0]
#define UART1_USED_TI                  (1)
 

#define UART_BAUD_2400                 (uint16)(1500000.0 / 2401.0)    // UART 鮑率 2400
#define UART_BAUD_4800                 (uint16)(1500000.0 / 4801.0)    // UART 鮑率 4800
#define UART_BAUD_9600                 (uint16)(1500000.0 / 9601.0)    // UART 鮑率 9600


/* 全域連結巨集 ----------------------------------------------------------------*/
#define UART_BAUD_CUSTOM(x)            (uint16)(1500000.0 / (x) + 1)   // UART 鮑率 自定義
#define SetUART_BAUD(__UART, __BAUD__) __UART = ((__UART & 0x7000) | (0x0FFF & __BAUD__));

/* 結構宣告---------------------------------------------------------------------*/
typedef struct
{
  uint8   Rx_DATA[14];
  uint8  *Tx_DATA;
  uint8   tLen;
  uint8   rLen;
  uint8   TxFlag;
  uint8   RxFlag;
  uint16  RxTimeOut;
}UART_HandleTypeDef;


/* 全域連結變數-----------------------------------------------------------------*/
extern UART_HandleTypeDef UART1;

/* 全域連結函數-----------------------------------------------------------------*/
extern void UART1_Init(void);
extern void UART2_Init(void);
extern void Uart1_ReceiveEvent(void);
extern void Uart_CountEvent(void);
extern void Uart1_TransmitEvent(void);
extern void UART1_Transmit(uint8*, uint8);

/**
  * @}
  */
#endif

