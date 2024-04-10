/**
  ******************************************************************************
  * @file           : PWM_SpeedControl.c
  * @copyright      : (c) 2022, Fortior Tech
  * @brief          : 
  * @date           : 2023-02-14
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
#include "MyProject.h"

#if (HPFCOL_Enable == 1)
 #include "HPFCOL_Function.h"
#endif

/* Private includes ----------------------------------------------------------*/
#include "PWM_SpeedControl.h"

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
#define Motor_rawSpeedCurve(_S1_, _S0_, _D1_, _D0_)   ((float)(RPM2SpeedFlt((_S1_)) - RPM2SpeedFlt((_S0_))) / (float)((_D1_) - (_D0_)))



/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
const uint16 PWMCurve_dutyArray[MOTOR_PWMMODE_NODENUM]  = MOTOR_PWMMODE_NODEDUTY;
const uint16 PWMCurve_speedArray[MOTOR_PWMMODE_NODENUM] = MOTOR_PWMMODE_NODESPEED;
PWMCapture_TyperDef mcPWM_Input;

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/


/**
  * @brief    PWM 工作週期與頻率捕獲函數。
  * @retval   None
  */
void PWM_capture(void)
{
  if (mcPWM_Input.isBusy == 0)
  {
    mcPWM_Input.Compare = TIM3__DR;
    mcPWM_Input.Cycle   = TIM3__ARR;
    mcPWM_Input.isBusy  = 1;
    
    #if (WIDEBAND_PWM_Enable == 1)
    {
      if (mcPWM_Input.wideBand_Flag == 1 && mcPWM_Input.Cycle < 1050)
      {
        if (mcPWM_Input.wideBand_CNT > 10)
        {
          mcPWM_Input.wideBand_CNT  = 0;
          mcPWM_Input.wideBand_Flag = 0;
          SetReg(TIM3_CR0, T3PSC0 | T3PSC1 | T3PSC2, T3PSC0 | T3PSC1 | T3PSC2);
          mcPWM_Input.isBusy  = 0;
        }
        else
        {
          mcPWM_Input.wideBand_CNT++;
        }
      }
      else if (mcPWM_Input.wideBand_Flag == 1 && mcPWM_Input.wideBand_CNT != 0)
      {
        mcPWM_Input.wideBand_CNT--;
      }
    }
    #endif
  }
}

   
/**
  * @brief    PWM 捕獲溢位檢測函數。
  * @retval   None
  */
void PWM_overFlowDetection(void)
{
  if (ReadBit(P0, PIN1))
  {
    mcPWM_Input.Compare = 3000;
    mcPWM_Input.Cycle   = 3000;
  }
  else
  {
    mcPWM_Input.Compare = 0;
    mcPWM_Input.Cycle   = 3000;
  }
  
  mcPWM_Input.isBusy = 1;
  
  
  #if (WIDEBAND_PWM_Enable == 1)
  {
    if (mcPWM_Input.wideBand_Flag == 0)
    {
      SetReg(TIM3_CR0, T3PSC0 | T3PSC1 | T3PSC2, T3PSC2);
      mcPWM_Input.wideBand_Flag = 1;
      mcPWM_Input.isBusy = 0;
    }
  }
  #endif
}

/* -------------------------------------------------------------------------------------------------
    Function Name  : VSPShutDown_Function
    Description    : VSP 輸入偵測停轉副程式
    Date           : 
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void VSPShutDown_Function(void)
{
	if(mcVspInput.ADC7_LPF_Value >= Start_VSP_Voltage)
	{
		mcVspInput.VSP_Off_Filter_Count = 0;
		mcVspInput.VSP_On_Filter_Count++;
		if (mcVspInput.VSP_On_Filter_Count >= MotorOnFilterTime)
		{
			mcVspInput.VSP_On_Filter_Count = 0;
			mcVspInput.OnOff_Flag = 1;
		}
	}
	else
	{
		mcVspInput.VSP_On_Filter_Count = 0;
	}

	if(mcVspInput.ADC7_LPF_Value < Stop_VSP_Voltage)
	{
		mcVspInput.VSP_On_Filter_Count = 0;
		mcVspInput.VSP_Off_Filter_Count++;
		if (mcVspInput.VSP_Off_Filter_Count >= MotorOffFilterTime)
		{
			mcVspInput.VSP_Off_Filter_Count = 0;
			mcSpeedRamp.TargetValue = 0;
			mcSpeedRamp.ActualValue = 0;
			MOE = 0;
			mcVspInput.OnOff_Flag = 2;
		}
	}
	else
	{
		mcVspInput.VSP_Off_Filter_Count = 0;
	}
	
	if(mcVspInput.OnOff_Flag == 1)
	{
		mcSpeedRamp.FlagONOFF = 1;
	}
	else if(mcVspInput.OnOff_Flag == 2)
	{
		mcSpeedRamp.FlagONOFF = 0;
	}  
}

/*---------------------------------------------------------------------------*/
/* Name		:	void VSP_speedControl(void)
/* Input	:	NO
/* Output	:	NO
/* Description:	輸入VSP處理
/*---------------------------------------------------------------------------*/
void VSP_speedControl(void)
{ 
  LPF_MDU(ADC7_DR, 10, mcVspInput.ADC7_LPF_Value, mcVspInput.ADC7_LPF_LSB);
  mcVspInput.ADC7_Value = 32767 - mcVspInput.ADC7_LPF_Value;
  
  VSPShutDown_Function();
  
  if(mcSpeedRamp.FlagONOFF==1)
  {
    if(mcVspInput.ADC7_Value <= Max_VSP_Voltage)
    {
      mcSpeedRamp.TargetValue = (uint16)(Motor_Min_Speed + (VSP_Slope * (mcVspInput.ADC7_Value - (Min_VSP_Voltage))));

      if(mcVspInput.ADC7_Value < Min_VSP_Voltage)
      {
        mcSpeedRamp.TargetValue = Motor_Min_Speed;
      }
    }
    else
    {
      mcSpeedRamp.TargetValue = Motor_Max_Speed;
    }
  }
  else
  {
    mcSpeedRamp.TargetValue = 0;
  }
}

/**
  * @brief    PWM 調速控制函數。
  * @retval   None
  */
void PWM_speedControl(void)
{
  uint16 MotorSpeedVSP;
  uint16 NodeNUM = 0;
  if (mcPWM_Input.isBusy == 1)
  {
    mcPWM_Input.Duty = _Q15((float)(mcPWM_Input.Compare) / (float)mcPWM_Input.Cycle);
    
    #if (HPFCOL_Enable == 1)
    {
      mcPWM_Input.Duty = HPFCOL_SpeedControl(mcPWM_Input.Duty);
    }
    #endif
    
    /*PWM 倒相控制 --開始*/
    #if (PWM_INVERT_Enable == 0)
    {
      MotorSpeedVSP = mcPWM_Input.Duty;
    }
    #elif(PWM_INVERT_Enable == 1)
    {        
      MotorSpeedVSP = (32767 - mcPWM_Input.Duty);
    }
    #endif
    /*PWM 倒相控制 --結束*/
    
    
    #if (AC_Lose_Function_Enable == 1)
    {
      DELL_coastMode_IC_Event(MotorSpeedVSP);
    }
    #endif
    
    if (MotorSpeedVSP <= PWMCurve_dutyArray[0])
    {
      mcSpeedRamp.TargetValue = RPM2SpeedFlt(PWMCurve_speedArray[0]);
    }
    else if (MotorSpeedVSP < PWMCurve_dutyArray[MOTOR_PWMMODE_NODENUM - 1])
    {
      for (NodeNUM = 1; NodeNUM < MOTOR_PWMMODE_NODENUM; NodeNUM++)
      {
        if (MotorSpeedVSP > PWMCurve_dutyArray[NodeNUM - 1] && MotorSpeedVSP <= PWMCurve_dutyArray[NodeNUM])
        {
          mcSpeedRamp.TargetValue = RPM2SpeedFlt(PWMCurve_speedArray[NodeNUM - 1]) + Motor_rawSpeedCurve(PWMCurve_speedArray[NodeNUM], PWMCurve_speedArray[NodeNUM - 1], PWMCurve_dutyArray[NodeNUM], PWMCurve_dutyArray[NodeNUM - 1]) * (MotorSpeedVSP - PWMCurve_dutyArray[NodeNUM - 1]);
        }
      }
    }
    else
    {
      mcSpeedRamp.TargetValue = RPM2SpeedFlt(PWMCurve_speedArray[MOTOR_PWMMODE_NODENUM - 1]);
    }
  
    /*PWM 開關機控制 --開始*/
    #if (MOTOR_SHUTDOWN_Enable == 1)
    {
      if ((MotorSpeedVSP < PWMDUTY_TURNOFF && PWMDUTY_TURNOFF < PWMDUTY_TURNON) || (MotorSpeedVSP > PWMDUTY_TURNOFF && PWMDUTY_TURNOFF > PWMDUTY_TURNON))
      {
        mcPWM_Input.turnOn_CNT = 0;
        if (mcPWM_Input.turnOff_CNT++ >= 20)
        {
          mcPWM_Input.turnOff_CNT = 0;
          mcSpeedRamp.FlagONOFF = 0;
          Open_TailWind_Count = 0;
          mcSpeedRamp.TargetValue = 0;
          mcSpeedRamp.ActualValue = 0;
          Open_TailWind_Flag = 0;
        }
      }
      else if ((MotorSpeedVSP > PWMDUTY_TURNON && PWMDUTY_TURNOFF < PWMDUTY_TURNON) || (MotorSpeedVSP < PWMDUTY_TURNON && PWMDUTY_TURNOFF > PWMDUTY_TURNON))
      {
        mcPWM_Input.turnOff_CNT = 0;
        if (mcPWM_Input.turnOn_CNT++ >= 10)
        {
          mcPWM_Input.turnOn_CNT = 0;
          mcSpeedRamp.FlagONOFF = 1;
        }
      }
    }
    #endif
    /*PWM 開關機控制 --結束*/
    
    mcPWM_Input.isBusy = 0;
  }
}