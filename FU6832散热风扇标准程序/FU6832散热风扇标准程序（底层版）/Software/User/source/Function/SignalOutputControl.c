/**
  ******************************************************************************
  * @file           : SignalOutputControl.c
  * @copyright      : (c) 2023, Fortior Tech
  * @brief          : 信號輸出相關程序
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
#include <FU68xx_4.h>
#include "MyProject.h"

/* 私有型別      Private typedef ---------------------------------------------*/
#include "SignalOutputControl.h"

/* 私有定義      Private define ----------------------------------------------*/
#ifndef RDSignal_DelayTIM
 #define RDSignal_DelayTIM                    (1000)                                  /*!< RD 信號遲滯時間 */
#endif

/* 私有巨集      Private macro -----------------------------------------------*/
#define speedFlt2FG(_Val_)                    (uint16)(FG_TIMCLOCK / ((float)SpeedFlt2RPM(_Val_) / 30.0))
  
/* 私有變數      Private variables -------------------------------------------*/
/* 私有函式宣告  Private function prototypes ---------------------------------*/
/* 私有函式      Private functions -------------------------------------------*/


#if (ROTATESIGNAL_TYPE == FG_TYPE)
typedef struct {
  uint8  isPause;
  uint8  Update;
  int16  delayTime;
  uint16 FG_ARR_Val;
  uint16 Fake_SPD;
  
}signalOutput_HandleType;

#elif (ROTATESIGNAL_TYPE == RD_TYPE)
typedef struct{
  uint8  prev_State;
  uint8  RD_Level;
  uint8  state;
  uint8  isPause;
  int16  delayTime;
  int16  wait_CNT;
}signalOutput_HandleType;

#endif


signalOutput_HandleType SO_Var;

void SignalOutput_initial(void) 
{
  #if (ROTATESIGNAL_TYPE == FG_TYPE)
  {
    SO_Var.isPause   = 0;
    SO_Var.Update    = 0;
    SO_Var.delayTime = 800;
  }
  #elif (ROTATESIGNAL_TYPE == RD_TYPE)
  {
    SO_Var.state     = 1;
    SO_Var.RD_Level  = SO_Var.state;
    SO_Var.delayTime = 10000;
  }
  #endif
}


uint8 SignalOutput_Process(void)
{
  
  #if (ROTATESIGNAL_TYPE == FG_TYPE)
  {
    if (mcFocCtrl.CtrlMode == 1 && SO_Var.delayTime == 0 && SO_Var.isPause == 0)
    {
      if (SO_Var.Fake_SPD != 0)
      {
        SO_Var.FG_ARR_Val = speedFlt2FG(SO_Var.Fake_SPD);
        SO_Var.Fake_SPD   = 0;
        SO_Var.delayTime  = 1000;
      }
      else
      {
        SO_Var.FG_ARR_Val = speedFlt2FG(mcFocCtrl.SpeedFlt);
        SO_Var.delayTime  = 200;
      }
      
      SO_Var.Update = 1;
      if (!ReadBit(PH_SEL, T4SEL))
      {
        TIM4__ARR = SO_Var.FG_ARR_Val;
        TIM4__DR  = (TIM4__ARR >> 1);
        SetBit(PH_SEL, T4SEL);
      }
    }
    
    if ((mcFaultSource.Source != NoFault || mcSpeedRamp.FlagONOFF == 0) && SO_Var.isPause == 0)
    {
      
      if (ReadBit(PH_SEL, T4SEL))
      {
        ClrBit(PH_SEL, T4SEL);
        GP00 = 1;
      }
      SO_Var.Fake_SPD  = 0;
      SO_Var.delayTime = 800;
    }
    return 0x00;
  }
  #elif (ROTATESIGNAL_TYPE == RD_TYPE)
  { 
    if (SO_Var.state == 1 && ((mcFocCtrl.SpeedFlt < RPM2SpeedFlt((MOTOR_MINSPEED * 0.5)) && mcSpeedRamp.FlagONOFF == 1 && mcFocCtrl.CtrlMode == 1) || mcFaultSource.Source != NoFault))
    {
      SO_Var.wait_CNT  = -1;
      SO_Var.delayTime = RDSignal_DelayTIM;
      SO_Var.state = 0;
    }
    else if (SO_Var.state == 0 && (mcFocCtrl.SpeedFlt > RPM2SpeedFlt((MOTOR_MINSPEED * 0.7)) && mcSpeedRamp.FlagONOFF == 1 && mcFocCtrl.CtrlMode == 1))
    {
      if (SO_Var.wait_CNT == -1)
        SO_Var.wait_CNT = 200;
      else if (SO_Var.wait_CNT == 0)
      {
        SO_Var.delayTime = RDSignal_DelayTIM;
        SO_Var.state = 1;
      }
    }
    
    if (SO_Var.delayTime == 0)
      SO_Var.RD_Level = SO_Var.state;
    
    if (SO_Var.isPause == 0)
      return SO_Var.RD_Level;
    else
      return 1;      
  }
  #endif
}


void SignalOutput_Counter(void)
{
  #if (ROTATESIGNAL_TYPE == FG_TYPE)
  {
    if (SO_Var.delayTime > 0 && mcFocCtrl.CtrlMode == 1) 
      SO_Var.delayTime--;
  }
  #elif (ROTATESIGNAL_TYPE == RD_TYPE)
  {
    if (SO_Var.delayTime > 0) 
      SO_Var.delayTime--;
    if (SO_Var.wait_CNT > 0)
      SO_Var.wait_CNT--;
  }
  #endif
}





#if (ROTATESIGNAL_TYPE == FG_TYPE)

void FG_setFakeSpped(uint16 value)
{
  SO_Var.Fake_SPD = value;
}


void FG_Update_Event(void)
{
  if (SO_Var.Update == 1 && SO_Var.isPause == 0)
  {
    TIM4__ARR = SO_Var.FG_ARR_Val;
    TIM4__DR = (TIM4__ARR >> 1);
    SO_Var.Update = 0;
  }
} 

#endif


void SignalOutput_Pause(void)
{
  #if (ROTATESIGNAL_TYPE == FG_TYPE)
  {
    ClrBit(PH_SEL, T4SEL);
    GP00 = 1;
    SO_Var.isPause = 1;
  }
  #elif (ROTATESIGNAL_TYPE == RD_TYPE)
  {
    SO_Var.isPause = 1;
  }
  #endif
}


/**
  * @brief   FG/RD 輸出接續函數
  * @retval  無
  */
void SignalOutput_Continue(void)
{
  #if (ROTATESIGNAL_TYPE == FG_TYPE)
  {
    ClrBit(PH_SEL, T4SEL);
    SO_Var.Update = 0;
  }
  #endif
  SO_Var.isPause  = 0;
}


