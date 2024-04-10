/**
  ******************************************************************************
  * @file           : turboFanFunction.c
  * @copyright      : (c) 2022, Fortior Tech
  * @brief          : 
  * @date           : 2023-03-16
  * @version        : 1.0.0
  *
  ******************************************************************************
  * @attention
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include <FU68xx_4.h>
/* Private includes ----------------------------------------------------------*/
#include "MyProject.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define AVC_TURBOFAN_ALARMTEST_Enable         (0)                                     /*!< 啟用 AVC 專用 TurboFan 警報測試  @param (0) 禁用 @param (1) 啟用 */
#define AVC_TURBOFAN_MASTER_TIMEOUT           (1000)                                  /*!< (單位: mS) AVC 專用 TurboFan 主機發問逾時時間 [範圍: 500 ~ 2000] */
#define AVC_TURBOFAN_REFRESH_TIM              (500)                                   /*!< (單位: mS) AVC 專用 TurboFan 訊息更新時間 [範圍: 500 ~ 2000] @note 需小於 AVC_TURBOFAN_MASTER_TIMEOUT 參數 */


#define AVC_TURBOFAN_HEADER_ID                ('F')
#define AVC_TURBOFAN_ALARM_CHAR               ('A')
#define AVC_TURBOFAN_NORMAL_CHAR              ('S')


#if   (AVC_TURBOFAN_TYPE == MASTER)
 #define AVC_TURBOFAN_TYPE_ID                 ('M')
 #define AVC_TURBOFAN_GET_ID                  ('S')
 
#elif (AVC_TURBOFAN_TYPE == SLAVE)
 #define AVC_TURBOFAN_TYPE_ID                 ('S')
 #define AVC_TURBOFAN_GET_ID                  ('M')
#endif

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
TURBOFAN_HandleTypeDef c_TurboFan;

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/
/**
  * @brief  AVC_TurboFan 初始化函數
  * @retval 無
  */
void TurboFan_Initial(void)
{
  c_TurboFan.isError   = AVC_TURBOFAN_NORMAL_CHAR;
  
  c_TurboFan.isTirg    = 0; 
  c_TurboFan.TimeCount = AVC_TURBOFAN_DETECTION_TIME;
  
  c_TurboFan.Tx_DATA[0] = AVC_TURBOFAN_HEADER_ID;   // TurboFan 資訊檔頭
  c_TurboFan.Tx_DATA[1] = AVC_TURBOFAN_TYPE_ID;     // TurboFan 模式 ID
  c_TurboFan.Tx_DATA[2] = 'R';                      // TurboFan 電機狀態代號
  c_TurboFan.Tx_DATA[3] = '9';                      // TurboFan 電機狀態階段
  c_TurboFan.Tx_DATA[4] = c_TurboFan.isError;       // TurboFan 警報
  c_TurboFan.Tx_DATA[5] = '_';                      // TurboFan 預留
  c_TurboFan.Tx_DATA[6] = '_';                      // TurboFan 預留
  c_TurboFan.Tx_DATA[7] = 0x00;                     // TurboFan CRC-8 校驗
}


/**
  * @brief  AVC_TurboFan 計數器事件
  * @note   該函數僅能存在於 SYStick_INT 中斷函數中，不可於其他程序執行階段調用
  * @retval 無
  */
void TurboFan_CountEvent(void)
{
  if (c_TurboFan.TimeCount > 0)
    c_TurboFan.TimeCount--;

  if (c_TurboFan.Refresh_TIM > 0)
    c_TurboFan.Refresh_TIM--;
}


/**
  * @brief  AVC_TurboFan 處理事件函數
  * @note   該函數用來處理TurboFan相關運行機制，且僅能存在於主程序中執行，不可於其他程序執行階段調用
  * @retval 無
  */
void TurboFan_ProcessEvent(void)
{
  #if (AVC_TURBOFAN_TYPE == MASTER)
  {
    
    /*主機處理階段 --開始*/
    if (c_TurboFan.Refresh_TIM == 0 || (c_TurboFan.isRespond == 1 && c_TurboFan.Refresh_TIM < (AVC_TURBOFAN_MASTER_TIMEOUT - AVC_TURBOFAN_REFRESH_TIM)))
    {
      c_TurboFan.Tx_DATA[4] = c_TurboFan.isError;
      c_TurboFan.Tx_DATA[7] = SW_CRC8_Calc(c_TurboFan.Tx_DATA, (AVC_TURBOFAN_TxLEN - 1));
      UART1_Transmit(&c_TurboFan.Tx_DATA[0], AVC_TURBOFAN_TxLEN);
      c_TurboFan.Refresh_TIM = AVC_TURBOFAN_MASTER_TIMEOUT;
      
      if (c_TurboFan.isRespond == 0 && c_TurboFan.isTirg == 0)
      {
        c_TurboFan.TimeCount = AVC_TURBOFAN_DETECTION_TIME;
        c_TurboFan.isTirg = 1;
      }
      
      c_TurboFan.isRespond = 0;
    }
    /*主機處理階段 --結束*/
    
  }
  #elif (AVC_TURBOFAN_TYPE == SLAVE)
  {
    
    /*從機處理階段 --開始*/
    if (c_TurboFan.isRespond == 1 || c_TurboFan.Refresh_TIM == 0)
    {
      c_TurboFan.Refresh_TIM = AVC_TURBOFAN_MASTER_TIMEOUT;
      
      if (UART1.TxFlag == 0 && c_TurboFan.isRespond == 1)
      {
        UART1.Tx_DATA[4] = c_TurboFan.isError;
        UART1.Tx_DATA[AVC_TURBOFAN_TxLEN - 1] = SW_CRC8_Calc(UART1.Tx_DATA, (AVC_TURBOFAN_TxLEN - 1));
        UART1_Transmit(&c_TurboFan.Tx_DATA[0], AVC_TURBOFAN_TxLEN);
     //   UART1.RxFlag = 0;
      }
      
      if (c_TurboFan.isRespond == 0 && c_TurboFan.isTirg == 0)
      {
        c_TurboFan.TimeCount = AVC_TURBOFAN_DETECTION_TIME;
        c_TurboFan.isTirg = 1;
      }
      
      c_TurboFan.isRespond = 0;
    }
    /*從機處理階段 --結束*/
    
  }
  #else
  {
    #error "AVC_TURBOFAN_TYPE setting error."
  }
  #endif

  /*數據接收處理階段 --開始*/
  if (UART1.RxFlag == 1)
  {
    if (UART1.rLen == AVC_TURBOFAN_TxLEN && UART1.Rx_DATA[0] == AVC_TURBOFAN_HEADER_ID)
    {
      if (SW_CRC8_Calc(UART1.Rx_DATA, (AVC_TURBOFAN_TxLEN - 1)) == UART1.Rx_DATA[(AVC_TURBOFAN_TxLEN - 1)] && UART1.Rx_DATA[1] == AVC_TURBOFAN_GET_ID)
      {
        c_TurboFan.isRespond = 1;
        
        if (UART1.Rx_DATA[4] == AVC_TURBOFAN_ALARM_CHAR && c_TurboFan.isTirg == 0)
        {
          c_TurboFan.isTirg = 1;
          c_TurboFan.TimeCount = AVC_TURBOFAN_DETECTION_TIME;
        }
        else if (UART1.Rx_DATA[4] == AVC_TURBOFAN_NORMAL_CHAR && c_TurboFan.isTirg == 1)
        {
          c_TurboFan.isTirg = 0;
          c_TurboFan.TimeCount = AVC_TURBOFAN_DETECTION_TIME;
        }
      }
    }
    UART1.RxFlag = 0;
    UART1.rLen = 0;
    memset(UART1.Rx_DATA, 0, strlen(UART1.Rx_DATA));
  }
  /*數據接收處理階段 --結束*/
 
  /*自檢測狀態更新階段 --開始*/
  #if (AVC_TURBOFAN_ALARMTEST_Enable == 1)
  {
    if (mcPwmInput.PWMDuty < _Q15(0.3))
    {
      c_TurboFan.isError = AVC_TURBOFAN_ALARM_CHAR;
    }
    else
    {
      c_TurboFan.isError = AVC_TURBOFAN_NORMAL_CHAR;
    }
    
  }
  #elif (AVC_TURBOFAN_ALARMTEST_Enable == 0)
  {
    if ((mcFocCtrl.SpeedFlt < (mcSpeedRamp.TargetValue >> 1) && mcState == mcRun) || mcFaultSource.Source != NoFault)
    {
      c_TurboFan.isError = AVC_TURBOFAN_ALARM_CHAR;
    }
    else if (mcFocCtrl.SpeedFlt > (mcSpeedRamp.TargetValue * 0.7) && mcState == mcRun)
    {
      c_TurboFan.isError = AVC_TURBOFAN_NORMAL_CHAR;
    }
    
    
    if (UART1.TxFlag == 0)
    {
      /*所有狀態指示更新皆在此處進行撰寫 -- 開始*/
      if (mcFaultSource.Source != NoFault)
      {
        c_TurboFan.Tx_DATA[2] = 'R';
        if (c_TurboFan.isActive == 1)
          c_TurboFan.Tx_DATA[3] = 'T';
        else
          c_TurboFan.Tx_DATA[3] = '9';
      }
      else
      {
        c_TurboFan.Tx_DATA[2] = 'P';
        c_TurboFan.Tx_DATA[3] = 'x';
        
      }
      /*所有狀態指示更新皆在此處進行撰寫 -- 結束*/
    }
  }
  #else
  {
    #error "AVC_TURBOFAN_ALARMTEST_Enable setting error."
  }
  #endif
  /*自檢測狀態更新階段 --結束*/
  
  /*TurboFan 觸發處理階段 --開始*/
  if (c_TurboFan.TimeCount == 0)
  {
    #if (AVC_TURBOFAN_ACTIVE_Enable == 1)
    {
      c_TurboFan.isActive = c_TurboFan.isTirg;  // 更新觸發狀態
    }
    #endif
  }
  
  if (c_TurboFan.isActive == 1)
  {
    mcSpeedRamp.TargetValue = RPM2SpeedFlt(AVC_TURBOFAN_RUN_SPEED);
  }
  /*TurboFan 觸發處理階段 --結束*/
}

