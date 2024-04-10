/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : UART.c
  * @brief          : UART 相關功能
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
/* 通用標頭檔 ------------------------------------------------------------------*/
#include "FU68xx_4.h"

/* 專案標頭檔 ------------------------------------------------------------------*/
#include "MyProject.h"
/* 區域定義 --------------------------------------------------------------------*/
#define UART_READ_TIMEOUT                (40)                                  /*!< (單位: mS) UART 讀取逾時時間  [範圍: 10 ~ 1000]*/
#define UART1_MODE                       (UART_MODE_ONEWIRE)                   /*!< UART1 傳輸模式  @param (UART_MODE_ONEWIRE) 單線模式 @param (UART1_MODE_TX_RX) 一般模式*/

/* 結構宣告---------------------------------------------------------------------*/
/* 變數宣告 --------------------------------------------------------------------*/
UART_HandleTypeDef UART1;

uint8 __U1Tx_Position = 0;

/* 函數宣告 --------------------------------------------------------------------*/

/**
  * @addtogroup UART 函數
  * @{
 */

/**
  * @brief  UART1 初始化函數
  * @retval 無
  */
void UART1_Init(void)
{
  #if (UART1_MODE == UART_MODE_ONEWIRE)
  {
    SetBit(P0_PU , P06);
  }
  #endif
  
  SetBit(PH_SEL, UART1EN);
  
  PI2C_UT11 = 0;
  PI2C_UT10 = 0;
  
  REN     = 1;
  UT_MOD0 = UART1_MODE;                                          // 設定 Rx Tx 模式
  UT_MOD1 = UART_WORDLENGTH_8B;                                  // 設定資料位寬
  ES0     = UART1_USED_TI;                                       // 設定中斷模式
  SetUART_BAUD(UT_BAUD, UART_BAUD_CUSTOM(9600.0));               // 設定傳輸速率
} 


/**
  * @brief  UART2 初始化函數
  * @retval 無
  */
void UART2_Init(void)
{
  SetBit(PH_SEL , UART2EN);
  
  PSPI_UT21 = 0;
  PSPI_UT20 = 0;
  
  SetReg(UT2_CR,    UT2REN,              0);                    // 啟用 Uart2 輸入
  SetReg(UT2_CR,    UT2MOD0,             UART2_MODE_TX_RX);     // 設定 Rx Tx 模式
  SetReg(UT2_CR,    UT2MOD1,             UART_WORDLENGTH_8B);   // 設定資料位寬
  SetReg(UT2_BAUD, (UART2CH | UART2IEN), UART2_CHANNEL_1_TI);   // 設定中斷模式
  
  SetBit(P0_OE , P00);
  SetBit(P0_PU , P00);
  GP00 = 1;
  
  SetUART_BAUD(UT2_BAUD, UART_BAUD_9600);                       // 設定傳輸速率
}


/**
  * @brief  UART1 發送函數
  * @retval 無
  */
void UART1_Transmit(uint8 *Arr, uint8 Len)
{
  __U1Tx_Position = 0;
  UART1.Tx_DATA = Arr;
  UART1.tLen = Len;
  UT_DR = UART1.Tx_DATA[__U1Tx_Position++];
  UART1.TxFlag = 1;
}


/**
  * @brief  UART 計數函數
  * @retval 無
  */
void Uart_CountEvent(void)
{
  if (UART1.rLen != 0)
  {
    if (UART1.RxTimeOut < UART_READ_TIMEOUT)
    {
      UART1.RxTimeOut++;
    }
    else
    {
      UART1.RxFlag    = 1;
      UART1.RxTimeOut = 0;
    }
   }
}


/**
  * @brief  UART1 接收事件
  * @note   該函數僅在中斷中受到調用
  * @retval 無
  */
void Uart1_ReceiveEvent(void)
{
  if (UART1.RxFlag == 0)
  {
    UART1.RxTimeOut = 0;
    UART1.Rx_DATA[UART1.rLen++] = UT_DR;
    if (UART1.rLen >= 13)
    {
      UART1.RxFlag = 1;
    }
  }
}


/**
  * @brief  UART1 傳送事件
  * @note   該函數僅在中斷中受到調用
  * @retval 無
  */
void Uart1_TransmitEvent(void)
{  
  if (UART1.TxFlag == 1)
  {
    if (__U1Tx_Position >= UART1.tLen)
    {
      UART1.TxFlag = 0;
      __U1Tx_Position =  0;
    }
    else
    {
      UT_DR = UART1.Tx_DATA[__U1Tx_Position++];
    }
  } 
}

/**
  @}
*/
