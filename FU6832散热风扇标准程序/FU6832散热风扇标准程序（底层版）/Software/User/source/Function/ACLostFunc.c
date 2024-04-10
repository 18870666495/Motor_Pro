/**
  ******************************************************************************
  * @file           : ACLostFunc.c
  * @copyright      : (c) 2023, Fortior Tech
  * @brief          : DELL Coast Mode 相關功能
  ******************************************************************************
  * @attention
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
  
/* 標頭檔        Includes ----------------------------------------------------*/
#include "FU68xx_4.h"
#include "MyProject.h"

/* 私有型別      Private typedef ---------------------------------------------*/
typedef enum
{
  isReady  = 0,                     ///!< PWM 偵測準備
  isDetect = 1,                     ///!< PWM 偵測中
  isActive = 2,                     ///!< Coast Mode 運作中
  isWait   = 3                      ///!< Coast Mode 等待流程結束
}CoastMode_StateType;


typedef struct{
  CoastMode_StateType state;        ///!< Coast Mode 狀態
  uint16 startDelay;                ///!< Coast Mode 偵測延遲
  uint8  cycle_CNT;                 ///!< Coast Mode 週期計數
  uint8  IF_CNT;                    ///!< Coast Mode 觸發時間計數
  bool   slowWolk_Flag;             ///!< Coast Mode 緩爬坡旗標
  uint16 slowWolk_CNT;              ///!< Coast Mode 緩爬坡計數器
}CoastMode_HandleType;

/* 私有定義      Private define ----------------------------------------------*/
/* 私有巨集      Private macro -----------------------------------------------*/
/* 私有變數      Private variables -------------------------------------------*/
CoastMode_HandleType DELL_CoastMode_Var;

/* 私有函式宣告  Private function prototypes ---------------------------------*/
/* 私有函式      Private functions -------------------------------------------*/
/**
  * @brief  DELL Coast Mode 初始化函式
  * @note   Coast Mode 初始化執行程序
  * @return none
  */
void DELL_coastMode_Initial(void)
{
  DELL_CoastMode_Var.startDelay = 2000;
  DELL_CoastMode_Var.state = isReady;
  DELL_CoastMode_Var.cycle_CNT  = 0;
}


/**
  * @brief  DELL Coast Mode 執行函式
  * @note   Coast Mode 相關執行程序。
  * @return none
  */
void DELL_coastMode_Process(void)
{
  if (DELL_CoastMode_Var.startDelay != 0)
    DELL_CoastMode_Var.startDelay--;
  
  if (DELL_CoastMode_Var.slowWolk_CNT != 0)
    DELL_CoastMode_Var.slowWolk_CNT--;
  
}


/**
  * @brief  DELL Coast Mode 狀態觸發函式
  * @note   回傳 Coast Mode 觸發，由於受到專案標頭檔包裝緣故，狀態機無法傳遞，故使用此函數傳遞狀態；
  *         狀態機此函數只會觸發一次，觸發後狀態變為 isWait 等待 PWM 信號恢復。
  * @return [bool] 回傳觸發狀態
  */
bool DELL_coastMode_State(void)
{
  if (DELL_CoastMode_Var.state == isActive)
  {
    DELL_CoastMode_Var.state = isWait;
    return true;
  }
  return false;
}


/**
  * @brief  DELL Coast Mode 緩爬坡觸發函式
  * @note   回傳是否處於緩爬坡狀態，此函式僅能在順逆風處理時調用。
  * @return [bool] 回傳觸發狀態
  */
bool DELL_coastMode_slowWolkState(void)
{
  return DELL_CoastMode_Var.slowWolk_Flag;
}


/**
  * @brief  DELL Coast Mode IF 捕獲事件函式
  * @note   Coast Mode 捕獲斷電信號處理程序，本函式僅能於TIM3 IF 中斷內調用。
  * @return none  
  */
void DELL_coastMode_IF_Event(void)
{
  if (ReadBit(P0, PIN1) == PWM_OFState && DELL_CoastMode_Var.startDelay == 0 && DELL_CoastMode_Var.state == isDetect)
  {
    // 由於 99.97% 有機率被識別為 0% ，添加nop後再次判定狀態，防止誤觸發
    _nop_();
    if (ReadBit(P0, PIN1) == PWM_OFState)
    {
      MOE = 0;
      
      #if (ROTATESIGNAL_TYPE == FG_TYPE)
      {
        FG_setFakeSpped(mcFocCtrl.SpeedFlt);
      }
      #endif
      
      DELL_CoastMode_Var.cycle_CNT  = 0;
      DELL_CoastMode_Var.startDelay = 200;
      DELL_CoastMode_Var.state      = isActive;
    }
  }
  
  if (ReadBit(P0, PIN1) == PWM_OFState)
    DELL_CoastMode_Var.IF_CNT++;
}


/**
  * @brief  DELL Coast Mode 偵測信號事件函式
  * @note   Coast Mode 捕獲輸入信號處理程序。
  * @return none
  */
void DELL_coastMode_IC_Event(uint16 duty)
{
  if(DELL_CoastMode_Var.state != isDetect && DELL_CoastMode_Var.startDelay == 0)
  {
    if (DELL_CoastMode_Var.cycle_CNT > 5)
      DELL_CoastMode_Var.state = isDetect;
    else if (duty > _Q15(0.01))
      DELL_CoastMode_Var.cycle_CNT++;
    else
      DELL_CoastMode_Var.cycle_CNT = 0;
  }
  
  if (DELL_CoastMode_Var.IF_CNT >= 2 && DELL_CoastMode_Var.IF_CNT < 4)
  {
    DELL_CoastMode_Var.slowWolk_Flag = 1;
    DELL_CoastMode_Var.slowWolk_CNT  = 15000;
  }
  DELL_CoastMode_Var.IF_CNT = 0;
}


/**
  * @brief  DELL Coast Mode 緩爬坡函式
  * @note   Coast Mode 緩爬坡處理程序。
  * @param  [MCRAMP *] 爬坡結構
  * @return [uint16]   遞增值
  */
uint16 DELL_costMode_SlowWolk(MCRAMP *ramp)
{
  if (DELL_CoastMode_Var.slowWolk_CNT != 0 && DELL_CoastMode_Var.slowWolk_Flag == 1)
  {
    if (mcFocCtrl.SpeedFlt > (ramp->TargetValue * 0.8))
    {
      DELL_CoastMode_Var.slowWolk_Flag = 0;
      return ramp->IncValue;
    }
    else
      return 20;
  }
  return ramp->IncValue;
}
