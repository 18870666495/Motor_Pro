/* --------------------------- (C) COPYRIGHT 2020 Fortiortech ShenZhen -----------------------------
    File Name      : AddFunction.c
    Author         : Fortiortech  Appliction Team
    Version        : V1.0
    Date           : 2020-07-04
    Description    : This file contains .C file function used for Motor Control.
----------------------------------------------------------------------------------------------------
                                       All Rights Reserved
------------------------------------------------------------------------------------------------- */
#include "FU68xx_4.h"
#include "MyProject.h"




/* Private variables ---------------------------------------------------------*/

PWMINPUTCAL        idata   mcPwmInput;
CurrentVarible     idata   mcCurVarible;
FOCCTRL            xdata   mcFocCtrl;
ADCSample_TypeDef  xdata   ADCSampling;
MCRAMP             xdata   mcSpeedRamp;
MotorFRTypeDef     xdata   mcFRState;
VSPINPUTCAL        idata   mcVspInput;
extern uint16 DELL_costMode_SlowWolk(MCRAMP *);

uint16 MotorControlVSP;
uint16 MotorSpeedVSP;
uint16 PWM_Curve_Duty = 0;
uint8 SVPM5FlagTrigger  = 0;
uint16 Open_TailWind_Count = 0;
uint16 Open_TailWind_Flag = 0;

uint16 F_Stall_Value = 0;


#if (Watt_Limit_Enable == 1)
  uint16 Watt_Limit_LPF_Voltage = 0;
  int32  Watt_Limit_32Bits_Value = 0;
  uint16 Watt_Limit_Current      = 0;
  int16  Watt_Limit_Iq           = 0;
#endif
/* -------------------------------------------------------------------------------------------------
    Function Name  : Abs_F32
    Description    : 对变量取32位的绝对值
    Date           : 2020-04-10
    Parameter      : value: [输入/出]
------------------------------------------------------------------------------------------------- */
uint32 Abs_F32(int32 value)
{
  if(value < 0) return (-value);
  else          return (value);
}


/* -------------------------------------------------------------------------------------------------
    Function Name  : APP_DIV
    Description    : 将所有用到除法的地方，放在同一个中断，以避免中断串扰
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void APP_DIV(void)
{
//  if( mcPwmInput.PWMDivFlag == 1)  //启动除法器，避免与过调值中的除法冲突
//  {
//    DivQ_L_MDU(mcPwmInput.PWMCompareUpdate, 0x0000, mcPwmInput.PWMARRUpdate, mcPwmInput.PWMDuty);
//    mcPwmInput.PWMDivFlag = 0;
//  }
}


int16 QOUT_Value_revise = 0;

uint16 SVPWM_Trans_Time = 0;
uint16 SVPWM_Trans_Flag = 0;

uint16 TorqueLoopTime_Flag = 0;

uint16 Open_FOC_UQFlag = 0;
uint16 Open_FOC_UQTransTime = 0;

int16 SVPWM5_Value = 0;
int16 SVPWM5_Value1 = 0;

uint8 FOC_TSMIN_Change_Flag = 0;
uint16 FOC_TSMIN_Change_Count = 0;
uint16 FOC_TSMIN_Value = 0;

uint16 DQKP_Value = 0;
uint16 DQKI_Value = 0;

uint16 FG_Standard_Value = 0;
uint32 FG_Standard_LPF_Value = 0;
uint8  FG_Standard_Count = 0;
uint16 FG_ARR_Value = 0;
/* -------------------------------------------------------------------------------------------------
    Function Name  : Speed_response
    Description    : 速度响应函数，可根据需求加入控制环，如恒转矩控制、恒转速控制、恒功率控制
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
int16 FocIqTemp = 0;
void Speed_response(void)
{
  if((mcState == mcRun) || (mcState == mcStop))
  {
    switch(mcFocCtrl.CtrlMode)
    {
    case 0:
    {
		if(TailWindDetect.TailWindStartFlag == 1)
		{
			if(++TailWindDetect.TailWindFocIqChangeDelay >= 1)
			{
				TailWindDetect.TailWindFocIqChangeDelay = 1;
	
				FocIqTemp = FOC_IQREF;
				if((FocIqTemp+25) < IQ_TailWind_CCW_CURRENT2)
				{
					FocIqTemp+=25;
					mcFocCtrl.mcIqref = FocIqTemp;
					FOC_IQREF = mcFocCtrl.mcIqref;
					mcFocCtrl.State_Count = 1200;
				}
			}
		}
      if(mcFocCtrl.SpeedFlt > Motor_Loop_Speed)
      {
		TailWindDetect.TailWindFocIqChangeDelay = 0;
		TailWindDetect.TailWindStartFlag = 0;				
		SVPM5FlagTrigger = 0;
		if(mcFocCtrl.State_Count == 0)
		{
			mcFocCtrl.CtrlMode = 1;
		}
        #if (OUTLOOP_Enable == 1)
        {
          if(BEMFDetect.BEMFStartDelayStatus == 0)
          {
            mcSpeedRamp.ActualValue = mcFocCtrl.SpeedFlt;
          }
          else
          {
            BEMFDetect.BEMFStartDelayStatus = 0; 
            mcSpeedRamp.ActualValue = BEMFDetect.BEMFSpeed;
          }
         
        }
        #endif

        if(BEMFDetect.BEMFBrakeStepFlag != 0)//逆风刹车则不减电流
		{
			mcFocCtrl.mcIqref = IQ_Tailwind_RUN_CURRENT_HIGH;
        }
		else
		{
			if((BEMFDetect.BEMF_Function_Flag == 2)&&(BEMFDetect.BEMFBrakeFlag != 1))
			{
				mcFocCtrl.mcIqref = IQ_TailWind_CCW_Run_CURRENT;
			}
			else if(BEMFDetect.BEMFBrakeFlag == 1)
			{
				mcFocCtrl.mcIqref = IQ_TailWind_CCW_Run_CURRENT>>2;
			}
			else
			{
				mcFocCtrl.mcIqref = IQ_RUN_CURRENT;
			}
		}
        PI0_UKH = mcFocCtrl.mcIqref;
        PI1_UKH = mcFocCtrl.mcIqref;
        PI2_UKH = mcFocCtrl.mcIqref;
        PI3_UKH = mcFocCtrl.mcIqref;
		FOC_IQREF = mcFocCtrl.mcIqref;
        mcFocCtrl.TorqueLoopTime = 0;
      }
    }
    break;
    case 1:
    {
      mcFocCtrl.TorqueLoopTime++;
      if(mcFocCtrl.TorqueLoopTime > SPEED_REFRESH_TIME)
      {
        mcFocCtrl.TorqueLoopTime = 0;
        #if (OUTLOOP_Enable == 0)
        {
          mcFocCtrl.mcIqref = FOC_IQREF;
          if (FOC_IQREF < QOUTMAXVALUE)
          {
            mcFocCtrl.mcIqref += QOUTINC;
            if (mcFocCtrl.mcIqref > QOUTMAXVALUE)
            {
              mcFocCtrl.mcIqref = QOUTMAXVALUE;
            }
            FOC_IQREF = mcFocCtrl.mcIqref;
          }
          else if (FOC_IQREF > QOUTMAXVALUE)
          {
            mcFocCtrl.mcIqref -= QOUTINC;
            if (mcFocCtrl.mcIqref < QOUTMAXVALUE)
            {
              mcFocCtrl.mcIqref = QOUTMAXVALUE;
            }
            FOC_IQREF = mcFocCtrl.mcIqref;
          }

        }
        #elif (OUTLOOP_Enable == 1)
        {
          /*-------------------修正低壓電流過衝----------------------*/
			if( Open_TailWind_Count >= OUTloop_trans_time)
			{
				Open_TailWind_Flag = 1; //運轉PWM命令開始接收旗標
				FOC_EKP = OBSW_KP_GAIN_RUN4;             // 估算器里的PI的KP
				FOC_EKI = OBSW_KI_GAIN_RUN4;             // 估算器里的PI的KI
				PI3_KP  = SPEED_KP;		//SKP;
				PI3_KI  = SPEED_KI;		//SKI;
				FOC_OMEKLPF = SPEED_KLPF;
			}
			if(Open_TailWind_Count >= 8)
			{
				
			}
			if(Open_TailWind_Count >= OUTloop_trans_time+5)
			{							
				#if (RUNSVPWM5_Enable == 1)
				{					
					runSVPWM5_Function();
				}
				#endif

				
				#if (FOC_TSMIN_Close_Enable == 1)
				{
					FOC_TSMIN_Close_Function();
				}
				#endif
				if (FOC_DQKP > DQKP)
				{
					FOC_DQKP = FOC_DQKP - _Q12(0.1);
				}
				else
				{
					FOC_DQKP = DQKP;
				}
				if (FOC_DQKI > DQKI)
				{
					FOC_DQKI = FOC_DQKI - _Q15(0.01);
				}
				else
				{
					FOC_DQKI = DQKI;
				}
				DQKI_Value = FOC_DQKI;
				DQKP_Value = FOC_DQKP;									
					
			}
			else if(Open_TailWind_Count < OUTloop_trans_time + 15)
			{
				Open_TailWind_Count++; //運轉PWM命令開始接收旗標計數器
			}	
            if(FOC_QMAX < (QOUTMAX - _Q15(0.005))) //防止PWM給定後的過衝
            {
              FOC_QMAX        += _Q15(0.005);
              FOC_QMIN        += _Q15(-0.005);
            }
            else
            {
              FOC_QMAX        =  QOUTMAX;
              FOC_QMIN        = -QOUTMAX;
            }
            
            BEMFDetect.BEMFBrakeFlag = 0;
            BEMFDetect.BEMFBrakeStepFlag = 0;
            
            if(AC_Lose_Flag == 0)
            {
              //PI3_UKMAX = SOUTMAX;
              if(PI3_UKMAX <= (SOUTMAX - I_Value(0.008)))//防止順風過衝
              {
                PI3_UKMAX       +=I_Value(0.008);
              }
            }
            else if(AC_Lose_Flag == 2)
            {
              //PI3_UKMAX = AC_Lose_Limit_Power_Value;
              if(PI3_UKMAX <= (AC_Lose_Limit_Power_Value - I_Value(0.008)))//防止順風過衝
              {
                PI3_UKMAX       +=I_Value(0.008);
              }
            }
				
          /*-------------------------------------------------------------*/
          mc_ramp(&mcSpeedRamp);
          FOC_IQREF = HW_One_PI(mcSpeedRamp.ActualValue - mcFocCtrl.SpeedFlt);
        }
        #endif //END OUTLoop_Mode
      }
    }
    break;
    }
  }
  if(mcState == mcRun)
  {
    if(mcFocCtrl.RunStateCnt < 15000)
    {
      mcFocCtrl.RunStateCnt++;
    }
    else
    {
      BEMFDetect.BEMFCCWFlag = 0;
    }
  }
}
uint8 mc_ramp_Flag = 2;
/* -------------------------------------------------------------------------------------------------
    Function Name  : Slowdown_Response_Function
    Description    :
    Date           : 2020-05-29
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void Slowdown_Response_Function(void)
{
  static uint16 prev_Target = 0;
  if (FOC__UQ > _Q15(0.97) && (((int16)mcSpeedRamp.TargetValue - (int16)mcFocCtrl.SpeedFlt) > RPM2SpeedFlt(1500)))
  {
    if (abs((int16)prev_Target - (int16)mcSpeedRamp.TargetValue) > RPM2SpeedFlt(500))
    {
      mcSpeedRamp.ActualValue = mcFocCtrl.SpeedFlt + RPM2SpeedFlt(200.0);
      prev_Target = mcSpeedRamp.TargetValue;
    }
  }
}
/* -------------------------------------------------------------------------------------------------
    Function Name  : mc_ramp
    Description    :
    Date           : 2020-05-29
    Parameter      : hSpeedramp: [输入/出]
------------------------------------------------------------------------------------------------- */
void mc_ramp(MCRAMP *hSpeedramp)
{
  
  #if (OUTLOOP_Enable == 1)
  if(mcFocCtrl.SpeedFlt < mcSpeedRamp.TargetValue)
  {
//    if ((mcSpeedRamp.TargetValue - mcFocCtrl.SpeedFlt) > _Q15((MOTOR_MAXSPEED * 0.5) / MOTOR_SPEED_BASE))
//    {
//      hSpeedramp->IncValue = IncValue_H;
//    }
//    else if ((mcSpeedRamp.TargetValue - mcFocCtrl.SpeedFlt) > _Q15((MOTOR_MAXSPEED * 0.1) / MOTOR_SPEED_BASE))
//    {
//      hSpeedramp->IncValue = IncValue_M;
//    }
//    else if ((mcSpeedRamp.TargetValue - mcFocCtrl.SpeedFlt) > _Q15((MOTOR_MAXSPEED * 0.05) / MOTOR_SPEED_BASE))
//    {
//      hSpeedramp->IncValue = IncValue_L;
//    }
//		else
//		{
//			hSpeedramp->IncValue = 5;
//		}
		
		if ((mcSpeedRamp.TargetValue - FOC__EOME) > _Q15((MOTOR_MAXSPEED * 0.8) / MOTOR_SPEED_BASE))
    {
      hSpeedramp->IncValue = 90;
    }
    else if ((mcSpeedRamp.TargetValue - FOC__EOME) > _Q15((MOTOR_MAXSPEED * 0.7) / MOTOR_SPEED_BASE))
    {
      hSpeedramp->IncValue = 80;
    }
    else if ((mcSpeedRamp.TargetValue - FOC__EOME) > _Q15((MOTOR_MAXSPEED * 0.6) / MOTOR_SPEED_BASE))
    {
      hSpeedramp->IncValue = 70;
    }
    else if ((mcSpeedRamp.TargetValue - FOC__EOME) > _Q15((MOTOR_MAXSPEED * 0.5) / MOTOR_SPEED_BASE))
    {
      hSpeedramp->IncValue = 60;
    }
    else if ((mcSpeedRamp.TargetValue - FOC__EOME) > _Q15((MOTOR_MAXSPEED * 0.4) / MOTOR_SPEED_BASE))
    {
      hSpeedramp->IncValue = 50;
    }	
    else if ((mcSpeedRamp.TargetValue - FOC__EOME) > _Q15((MOTOR_MAXSPEED * 0.3) / MOTOR_SPEED_BASE))
    {
      hSpeedramp->IncValue = 40;
    }
    else if ((mcSpeedRamp.TargetValue - FOC__EOME) > _Q15((MOTOR_MAXSPEED * 0.2) / MOTOR_SPEED_BASE))
    {
      hSpeedramp->IncValue = 30;
    }
    else if ((mcSpeedRamp.TargetValue - FOC__EOME) > _Q15((MOTOR_MAXSPEED * 0.1) / MOTOR_SPEED_BASE))
    {
      hSpeedramp->IncValue = 20;
    }			
		else
		{
			hSpeedramp->IncValue = 10;
		}

    #if (AC_Lose_Function_Enable == 1)
    {
      hSpeedramp->IncValue = DELL_costMode_SlowWolk(hSpeedramp);
    }
    #endif
    
    
    
    if(hSpeedramp->ActualValue + hSpeedramp->IncValue < hSpeedramp->TargetValue)// && hSpeedramp->ActualValue - mcFocCtrl.SpeedFlt < 200)
    {
      hSpeedramp->ActualValue += hSpeedramp->IncValue;
    }
    else if (hSpeedramp->TargetValue - hSpeedramp->ActualValue < 40)
    {
      hSpeedramp->ActualValue = hSpeedramp->TargetValue;
    }
    else
    {
      //保留
    }
  }
  else
  {
    if ((hSpeedramp->ActualValue - mcSpeedRamp.TargetValue) > _Q15((MOTOR_MAXSPEED * 0.8) / MOTOR_SPEED_BASE))
    {
      hSpeedramp->DecValue = 50;
    }
    else if ((hSpeedramp->ActualValue - mcSpeedRamp.TargetValue) > _Q15((MOTOR_MAXSPEED * 0.3) / MOTOR_SPEED_BASE))
    {
      hSpeedramp->DecValue = 35;
    }
    else if ((hSpeedramp->ActualValue - mcSpeedRamp.TargetValue) > _Q15((MOTOR_MAXSPEED * 0.15) / MOTOR_SPEED_BASE))
    {
      hSpeedramp->DecValue = 20;
    }
    else
    {
      hSpeedramp->DecValue = 20;
    }
		
		//hSpeedramp->DecValue = DecValue_H;
		
    if(hSpeedramp->ActualValue - hSpeedramp->DecValue > hSpeedramp->TargetValue && hSpeedramp->ActualValue > hSpeedramp->DecValue)
    {
      hSpeedramp->ActualValue -= hSpeedramp->DecValue;
    }
    else if (hSpeedramp->ActualValue != hSpeedramp->TargetValue)
    {
      hSpeedramp->ActualValue = hSpeedramp->TargetValue;
    }
    else
    {
			//hSpeedramp->ActualValue -= hSpeedramp->DecValue;
    }
  }
  #endif
}

/* -------------------------------------------------------------------------------------------------
    Function Name  : HW_One_PI
    Description    : PI控制
    Date           : 2020-04-10
    Parameter      : Xn1: [输入/出]
------------------------------------------------------------------------------------------------- */
int16 HW_One_PI(int16 Xn1)
{

  PI3_EK = Xn1;
  SetBit(PI_CR, PI3STA);
  while(ReadBit(PI_CR, PIBSY));
  return PI3_UKH;
}


/* -------------------------------------------------------------------------------------------------
    Function Name  : StarRampDealwith
    Description    : 爬坡處理副程式
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void StarRampDealwith(void)
{
  if((mcState == mcRun))
  {
    if(mcFocCtrl.State_Count == 650)
    {
      FOC_EKP = OBSW_KP_GAIN_RUN;              // 估算器里的PI的KP
      FOC_EKI = OBSW_KI_GAIN_RUN;              // 估算器里的PI的KI
    }
    else if(mcFocCtrl.State_Count == 600)
    {
      FOC_EKP = OBSW_KP_GAIN_RUN1;             // 估算器里的PI的KP
      FOC_EKI = OBSW_KI_GAIN_RUN1;             // 估算器里的PI的KI
    }
    else if(mcFocCtrl.State_Count == 500)
    {
      FOC_EKP = OBSW_KP_GAIN_RUN2;             // 估算器里的PI的KP
      FOC_EKI = OBSW_KI_GAIN_RUN2;             // 估算器里的PI的KI
    }
    else if(mcFocCtrl.State_Count == 300)
    {
      FOC_EKP = OBSW_KP_GAIN_RUN3;             // 估算器里的PI的KP
      FOC_EKI = OBSW_KI_GAIN_RUN3;             // 估算器里的PI的KI
    }
    else if(mcFocCtrl.State_Count == 100)
    {
      FOC_EKP = OBSW_KP_GAIN_RUN4;             // 估算器里的PI的KP
      FOC_EKI = OBSW_KI_GAIN_RUN4;             // 估算器里的PI的KI
    }
    else
    {
      //保留
    }
  }
}


/**
  * @brief  單電阻五段式運行切換函式
  * @note   單電阻五段式運行切換，不得在使用Coast Mode 狀態下啟用。
  * @return none
  */
void runSVPWM5_Function(void)
{
  if ((mcFocCtrl.SpeedFlt > _Q15((SVPWM5_SWITCH_THRESHOLD + SVPWM5_BUFFER_THRESHOLD) / MOTOR_SPEED_BASE) && SVPM5FlagTrigger == 0))
  {
    #if (SVPWM5_ATT_LEVEL != 0)
    {
      PI3_UKH = (FOC_IQREF - (FOC_IQREF >> SVPWM5_ATT_LEVEL));
      SVPM5FlagTrigger = 1;
      SetBit(FOC_CR2, F5SEG);
    }
    #endif
  }
  else if (mcFocCtrl.SpeedFlt < _Q15((SVPWM5_SWITCH_THRESHOLD - SVPWM5_BUFFER_THRESHOLD) / MOTOR_SPEED_BASE) && SVPM5FlagTrigger == 1 )
  {
    SVPM5FlagTrigger = 0;
    ClrBit(FOC_CR2, F5SEG);
  }
}


/* -------------------------------------------------------------------------------------------------
    Function Name  : FOC_TSMIN_Close_Function
    Description    : 單電阻採樣關窗控制副程式
    Date           : 2021-05-20
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void FOC_TSMIN_Close_Function(void)
{		
	if(FOC__UQ > _Q15(0.17)) //4cm 約在input PWM Duty0~3%切換
	{
			FOC_TSMIN = 0;
			FOC_TSMIN_Value = 0;
			FOC_TSMIN_Change_Flag = 1;
	}
	else
	{
		if((FOC_TSMIN_Change_Flag == 1)||(FOC_TSMIN_Change_Flag == 3))
		{
			if(((FOC__UQ < _Q15(0.15)))||(FOC_TSMIN_Change_Flag == 3))
			{
				//FOC_TSMIN = (uint16)(_Q16 / PWM_CYCLE * (PWM_DEADTIME + 0.6) / 16);
					if(FOC_TSMIN_Value <= 100)
					{
						FOC_TSMIN_Value +=3;
					}
					else if(FOC_TSMIN_Value > 100)
					{
						FOC_TSMIN_Value = 105;
					}
				FOC_TSMIN = FOC_TSMIN_Value;
				FOC_TSMIN_Change_Flag = 3;
			}
		}
		else if ((FOC_TSMIN_Change_Flag == 0)||(FOC_TSMIN_Change_Flag == 2))
		{
			//FOC_TSMIN = (uint16)(_Q16 / PWM_CYCLE * (PWM_DEADTIME + 0.6) / 16);
			FOC_TSMIN_Value = 105;
			FOC_TSMIN = FOC_TSMIN_Value;
			FOC_TSMIN_Change_Flag = 2;
		}
	}
}


void CurrentLimiter_Process(void)
{
  #if (CURRENT_LIMITER_SOURCE == USED_AD1)
  {
    LPF_MDU(ADC1_DR, 10, mcCurVarible.ADC_IBUS, mcCurVarible.ADC_IBUS_LSB);
  }
  #elif (CURRENT_LIMITER_SOURCE == USED_AD3)
  {
    LPF_MDU(ADC3_DR, 10, mcCurVarible.ADC_IBUS, mcCurVarible.ADC_IBUS_LSB);
  }
  #elif (CURRENT_LIMITER_SOURCE == USED_AD13)
  {
    LPF_MDU(ADC13_DR, 10, mcCurVarible.ADC_IBUS, mcCurVarible.ADC_IBUS_LSB);
  }
  #endif
 
  mcCurVarible.ADC_IBUS_Offset = mcCurVarible.ADC_IBUS ;
  mcCurVarible.ADC_IBUS_Value = mcCurVarible.ADC_IBUS_Offset - (mcCurOffset.Iw_busOffset - 16383);
  mcCurVarible.FOC_IQREF_Value = FOC_IQREF;
  if(mcCurVarible.ADC_IBUS_Value >= CurrentLimit_Val)
  {
    mcCurVarible.FOC_IQREF_Value = mcCurVarible.FOC_IQREF_Value - 1;
    PI3_UKMAX = mcCurVarible.FOC_IQREF_Value;
    FOC_IQREF = mcCurVarible.FOC_IQREF_Value;
  }
  else
  {
    if(PI3_UKMAX <= (SOUTMAX - 1))//防止順風過衝
    {
      PI3_UKMAX += 1;
    }
  }
}


/*---------------------------------------------------------------------------*/
/* Name		:	void Watt_Limit_Function(void)
/* Input	:	NO
/* Output	:	NO
/* Description:新功耗限制
/*---------------------------------------------------------------------------*/
void Watt_Limit_Function(void)
{
  #if (CURRENT_LIMITER_SOURCE == USED_AD1)
  {
    LPF_MDU(ADC1_DR, 10, mcCurVarible.ADC_IBUS, mcCurVarible.ADC_IBUS_LSB);
  }
  #elif (CURRENT_LIMITER_SOURCE == USED_AD3)
  {
    LPF_MDU(ADC3_DR, 10, mcCurVarible.ADC_IBUS, mcCurVarible.ADC_IBUS_LSB);
  }
  #elif (CURRENT_LIMITER_SOURCE == USED_AD13)
  {
    LPF_MDU(ADC13_DR, 10, mcCurVarible.ADC_IBUS, mcCurVarible.ADC_IBUS_LSB);
  }
  #endif
  
  mcFocCtrl.Voltage_Value = ((((float)(((float)(ADCSampling.DcBus_Flt) / 32767.0) * 5)) / RV2) * (RV1 + RV2));
  mcFocCtrl.Current_Value = ((float)(mcFocCtrl.Watt_Limit_Current - 16383)* ((float)(HW_ADC_REF / 2.0 / HW_AMPGAIN / HW_RSHUNT) / 16383.0));

//  LPF_MDU(ADC1_DR, 10, mcCurVarible.ADC_IBUS, mcCurVarible.ADC_IBUS_LSB);
  mcCurVarible.ADC_IBUS_Offset = mcCurVarible.ADC_IBUS ;
  mcCurVarible.ADC_IBUS_Value = mcCurVarible.ADC_IBUS_Offset - (mcCurOffset.Iw_busOffset - 16383);
  mcFocCtrl.Watt_Limit_Current = mcCurVarible.ADC_IBUS_Value;

  mcFocCtrl.Watt_Limit_Value = mcFocCtrl.Voltage_Value * mcFocCtrl.Current_Value;

  mcCurVarible.FOC_IQREF_Value = FOC_IQREF;
  
  if(mcFocCtrl.Watt_Limit_Value >= (Watt_Limit_Target - Watt_Limit_Modeify_Value))
  {
    if(mcCurVarible.FOC_IQREF_Value >= LIMIT_MIN_CURRENT)
    {    
      mcCurVarible.FOC_IQREF_Value = mcCurVarible.FOC_IQREF_Value - 5;
      PI3_UKMAX = mcCurVarible.FOC_IQREF_Value;
      FOC_IQREF = mcCurVarible.FOC_IQREF_Value;      
    }
  }
  else
  {
    if(PI3_UKMAX <= (SOUTMAX - 1))//防止順風過衝
    {
      PI3_UKMAX += 1;
    }
  }
}