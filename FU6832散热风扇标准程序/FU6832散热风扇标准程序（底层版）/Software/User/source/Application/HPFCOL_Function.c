/**
  ******************************************************************************
  * @file           : HPFCOL_Function.c
  * @copyright      : (c) 2022, Fortior Tech
  * @brief          : 
  * @date           : 2023-03-09
  * @version        : 1.0.1
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
typedef enum{
  DetectTrig_Step = 0,       // HP-FCOL 檢測指令階段
  Wait500mS_Step  = 1,       // HP-FCOL 前導碼前緩衝階段
  SendHeader_Step = 2,       // HP-FCOL 發送前導碼階段
  Wait250mS_Step  = 3,       // HP-FCOL 資料傳送前緩衝階段
  SendHPDATA_Step = 4,       // HP-FCOL 資料傳送階段
  SendEnd_Step    = 5,       // HP-FCOL 資料傳輸結束階段
  WaitTffo_Step   = 6        // HP-FCOL 等待通訊緩衝階段
}HPFCOL_STEP;


/**
  * @brief HPFCOL_TyperDef 結構定義
  */
typedef struct
{
  HPFCOL_STEP  Send_Step;  /*!< HP-FCOL 階段 */
  uint16 Tibc_CNT;         /*!< HP-FCOL Tibc 時間計數 */
  
#if (HPFCOL_SWCRC_CHECK_Enable == 1)
  uint8  DATA_Crc;         /*!< HP-FCOL 資料Crc */
  
#endif

  uint8  Trig_Flag;        /*!< HP-FCOL 指令觸發旗標 */
  uint8  Send_Flag;        /*!< HP-FCOL 資料傳送旗標 */
  uint16 Trig_CNT;         /*!< HP-FCOL 指令觸發計數器 */
  uint16 Delay_CNT;        /*!< HP-FCOL 延遲計數器 */
  uint16 PDS_Histroy;      /*!< HP-FCOL 調速值暫存 --補丁--*/
}HPFCOL_TyperDef;


/* Private define ------------------------------------------------------------*/
#define HPFCOL_BitPeriod                      (720)                                   /*!< (單位: uS) HP-FCOL 位元信號週期 */
#define HPFCOL_CommMinTime                    (400)                                   /*!< (單位: mS) HP-FCOL 最低指令時間 */
#define HPFCOL_CommMaxTime                    (600)                                   /*!< (單位: mS) HP-FCOL 最高指令時間 */
#define HPFCOL_Preamble_IdleTime              (500)                                   /*!< (單位: mS) HP-FCOL 前導碼前緩衝時間 */
#define HPFCOL_Transfer_IdleTime              (250)                                   /*!< (單位: mS) HP-FCOL 資料傳送前緩衝時間 */
#define HPFCOL_Tffo_Time                      (1000)                                  /*!< (單位: mS) HP-FCOL 傳輸結束緩衝時間 */
#define HPFCOL_Min_TibcTime                   (2000)                                  /*!< (單位: mS) HP-FCOL 通訊緩衝時間 */
#define HPFCOL_TESTLen                        (64)                                    /*!< HP-FCOL 資料長度 [範圍: 1 ~ 127] */

/* Private macro -------------------------------------------------------------*/
#define HPFCOL_BIT_TIMARR                     (uint16)((float)HPFCOL_BitPeriod / (float)(1000000.0f / FG_TIMCLOCK))
#define HPFCOL_BITTIMUNIT                     (HPFCOL_BIT_TIMARR / 4)

/* Private variables ---------------------------------------------------------*/
extern code unsigned char HPFCOL_Data[64];
HPFCOL_TyperDef HPFCOL;

/* Private function prototypes -----------------------------------------------*/   

/* Private user code ---------------------------------------------------------*/
/**
  * @brief  HP-FCOL 初始化函數
  * @note   本函數僅能在SoftwareInit() 函數中調用。
  * @retval 無
  */
void HPFCOL_Initial(void)
{
  #if (HPFCOL_SWCRC_CHECK_Enable == 1)
  {
    HPFCOL.DATA_Crc = SW_CRC8_Calc(&HPFCOL_Data[0], 63);
  }
  #endif
}


/**
  * @brief  HP-FCOL TIM4 初始化函數
  * @retval 無
  */
void TIM4_HPFCOL_Initial()
{
  ClrBit(PH_SEL, T4SEL);
  ClrBit(TIM4_CR0, T4IRE);
  
  SetReg(TIM4_CR0, (T4PSC2 | T4PSC1 | T4PSC0), FG_ClockBase);
  SetReg(TIM4_CR0, (T4MOD),                    TIM_PWM_Mode);
  SetReg(TIM4_CR0, (T4OCM),                    TIM_OCNPOLARITY_HIGH);
  SetReg(TIM4_CR1, (T4NM1 | T4NM0),            TIM_TriggerFilter_NONE);
  SetReg(PH_SEL1,  (T4CT),                     TIM4_CHANNEL_1);
  SetReg(TIM4_CR1, (T4IPE | T4IFE), TIM_TS_TI1F);
  SetReg(TIM4_CR0, (T4IRE),         TIS_TS_TI1R);
  
  TIM4__ARR  = HPFCOL_BIT_TIMARR;
  TIM4__DR   = HPFCOL_BIT_TIMARR + 1;
  TIM4__CNTR = 0;
  
  PTIM41 = 1;
  PTIM40 = 1;
  
  SetBit(TIM4_CR1, T4EN);
}


/**
  * @brief  HP-FCOL 通訊檢測函數
  * @retval 無
  */
void HPFCOL_CommDetection(void)
{
  static uint8 IF_Flag = 0;

  if (ReadBit(TIM3_CR1, T3IF) && ReadBit(P0, PIN1))
  {
    // 觸發轉速紀錄器
    if (IF_Flag == 0)
    {
      HPFCOL.Delay_CNT = HPFCOL_CommMaxTime;
      IF_Flag = 1;
    }

    // 觸發HP-FCOL 指令旗標
    if (HPFCOL.Send_Flag == 0)
    {
      HPFCOL.Trig_Flag = 1;
    }
  }

  else if (ReadBit(TIM3_CR1, T3IP) || (ReadBit(TIM3_CR1, T3IF) && !ReadBit(P0, PIN1)))
  { 
    // 觸發轉速紀錄器
    if (ReadBit(TIM3_CR1, T3IP))
    {
      IF_Flag = 0;
      HPFCOL.PDS_Histroy = mcPWM_Input.Duty;
    }

    if (HPFCOL.Send_Flag == 0)
    {
      // 確認指令階段
      if ((HPFCOL.Trig_CNT > HPFCOL_CommMinTime) && (HPFCOL.Trig_CNT < HPFCOL_CommMaxTime) && (HPFCOL.Trig_Flag == 1))
      {
        HPFCOL.Tibc_CNT  = (HPFCOL_Min_TibcTime - HPFCOL.Trig_CNT);
        HPFCOL.Send_Step = Wait500mS_Step;
        HPFCOL.Send_Flag = 1;
        SignalOutput_Pause();
      }
      HPFCOL.Trig_CNT = 0;
      HPFCOL.Trig_Flag = 0;
    }
  }
}


/**
  * @brief  HP-FCOL 計數器事件函數
  * @notes  本函數僅能在SYSTick() 中斷裡調用
  * @retval 無
  */
void HPFCOLCounter_Event(void)
{
  static int16 l_Counter = HPFCOL_Preamble_IdleTime;

  // 指令長度計數器
  if ((HPFCOL.Trig_Flag == 1) && (HPFCOL.Trig_CNT < HPFCOL_CommMaxTime) && (HPFCOL.Send_Flag == 0))
  {
    HPFCOL.Trig_CNT++;
  }
  
  if (HPFCOL.Delay_CNT != 0)
  {
    HPFCOL.Delay_CNT--;
  }
  
  switch(HPFCOL.Send_Step)
  {
    case Wait500mS_Step:
    {
      if (l_Counter != 0)
      {
        l_Counter--;
      }
      else
      {
        l_Counter = HPFCOL_Transfer_IdleTime;
        TIM4_HPFCOL_Initial();
        SetBit(PH_SEL, T4SEL);
        HPFCOL.Send_Step = SendHeader_Step;
      }
      break;
    }
    case Wait250mS_Step:
    {
      if (l_Counter != 0)
      {
        l_Counter--;
      }
      else
      {
        TIM4__CNTR = 0;
        TIM4__ARR = HPFCOL_BIT_TIMARR;
        TIM4__DR = HPFCOL_BITTIMUNIT;
        HPFCOL.Send_Step = SendHPDATA_Step;
        SetBit(PH_SEL, T4SEL);
      }
      break;
    }
    case SendEnd_Step:
    {
      HPFCOL.Send_Step = WaitTffo_Step;
      l_Counter = HPFCOL_Tffo_Time;
      break;
    }
    case WaitTffo_Step:
    {
      if (l_Counter != 0)
      {
        l_Counter--;
      }
      else  if (l_Counter == 0) 
      {
        l_Counter = -1;
        #if (ROTATESIGNAL_TYPE != RD_TYPE)
        {
          TIM4_Initial();
        }
        #endif
        SignalOutput_Continue();
      }

    
      
      if (HPFCOL.Tibc_CNT != 0) 
      {
        HPFCOL.Tibc_CNT--;
      }
      else
      {
        //ClrBit(PH_SEL, T4SEL);
        HPFCOL.Send_Step = DetectTrig_Step;
        l_Counter = HPFCOL_Preamble_IdleTime;
        HPFCOL.Send_Flag = 0;
      }
      break;
    }
  }
}


/**
  * @brief  HP-FCOL 更新事件函數
  * @retval 無
  */
void HPFCOLUpdate_Event(void)
{
  static uint8 l_Counter = 0, FCOL_Posi = 0, dBit = 0;
  if (ReadBit(TIM4_CR1, T4IR) && HPFCOL.Send_Flag == 1)
  {
    if (HPFCOL.Send_Step == SendHPDATA_Step)
    {
      //ClrBit(TIM4_CR1, T4EN);
      if (FCOL_Posi >= HPFCOL_TESTLen)
      {
        HPFCOL.Send_Step = SendEnd_Step;
        ClrBit(PH_SEL, T4SEL);
        GP00 = 1;
        FCOL_Posi = 0;
        l_Counter  = 0;
      }
    }
  }
    
  if (ReadBit(TIM4_CR1, T4IF) && (HPFCOL.Send_Flag == 1))
  {
    if (HPFCOL.Send_Step == SendHeader_Step)
    {
      if (l_Counter++ == 4)
      {
        ClrBit(PH_SEL, T4SEL);
        HPFCOL.Send_Step = Wait250mS_Step;
        SetReg(TIM4_CR0, (T4OCM), TIM_OCNPOLARITY_LOW);
        GP00 = 1;
        l_Counter = 1;
      }
      else
      {
        if ((l_Counter & 1) == 0)
        {
          SetReg(TIM4_CR0, (T4OCM), TIM_OCNPOLARITY_HIGH);
        }
        else
        {
          SetReg(TIM4_CR0, (T4OCM), TIM_OCNPOLARITY_LOW);
        }
      }
    }
    else if (HPFCOL.Send_Step == SendHPDATA_Step)
    {
      if (FCOL_Posi < HPFCOL_TESTLen)
      {
        #if (HPFCOL_SWCRC_CHECK_Enable == 1)
        {
          if (FCOL_Posi < (HPFCOL_TESTLen - 1))
          {
            dBit = (((HPFCOL_Data[FCOL_Posi] << l_Counter) & 0x80)? (2) : (0));
          }
          else
          {
            dBit = (((HPFCOL.DATA_Crc << l_Counter) & 0x80)? (2) : (0));
          }
        }
        #elif (HPFCOL_SWCRC_CHECK_Enable == 0)
        {
          dBit = (((HPFCOL_Data[FCOL_Posi] << l_Counter) & 0x80)? (2) : (0));
        }
        #endif
        
        TIM4__DR = HPFCOL_BITTIMUNIT + (dBit * HPFCOL_BITTIMUNIT);
        
        if ((++l_Counter) >= 8)
        {
          FCOL_Posi++;
          l_Counter = 0;
        }
      }

    }
  }
}


/**
  * @brief    HP-FCOL 通訊指令識別函數 --補丁--
  * @note     本函數用於接收指令狀態時，使電機維持當前轉速，用以避免接收指令時出現小幅度加速現象。
  * @warning  此補丁為針對 PWM_speedControl() 函數所設計的函數，不得再任何地方調用。
  * @retval   無
  */
uint16 HPFCOL_SpeedControl(uint16 VSP)
{
  return (HPFCOL.Delay_CNT != 0)? (HPFCOL.PDS_Histroy) : (VSP);
}

