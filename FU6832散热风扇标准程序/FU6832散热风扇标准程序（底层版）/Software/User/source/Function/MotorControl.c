/* --------------------------- (C) COPYRIGHT 2020 Fortiortech ShenZhen -----------------------------
    File Name      : MotorControl.c
    Author         : Fortiortech  Appliction Team
    Version        : V1.0
    Date           : 2020-04-10
    Description    : This file contains .C file function used for Motor Control.
----------------------------------------------------------------------------------------------------  
                                       All Rights Reserved
------------------------------------------------------------------------------------------------- */
#include "FU68xx_4.h"
#include "MyProject.h"

#if (ROTATESIGNAL_TYPE != NO_TYPE)
 #include "SignalOutputControl.h"
#endif

/* Private variables ----------------------------------------------------------------------------*/
MotStaType mcState;
MotStaM    McStaSet;

uint16 TimeCnt;

#if (MOTOR_STARTDELAY_Enable == 1)
uint16 Align_HistoryCNT = 0;
uint8  StartDelay_Flag  = 0;
#endif

/* -------------------------------------------------------------------------------------------------
    Function Name  : MC_Control
    Description    : 电机控制状态机
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void MC_Control(void)
{
  
  #if (AC_Lose_Function_Enable == 1)
  {
    if (DELL_coastMode_State())
    {
      mcFocCtrl.State_Count = 500;
      mcState = mcStop;
    }
  }
  #endif
    
  
  if (mcFaultSource.Source != NoFault)
  {
    mcState = mcFault;
  }
  switch(mcState)
  {
    case mcReady:
    {
		if (mcSpeedRamp.FlagONOFF == 0)
		{
			mcState = mcStop;
		}
		else
		{				
			Motor_Ready();
			
			if ((mcCurOffset.OffsetFlag == 1) && (mcSpeedRamp.FlagONOFF == 1)) 
			{
				mcCurOffset.OffsetFlag  = 0;
				mcState = mcInit; 
			}
		}
		break;
	}
    case mcInit:
    {
		if (mcSpeedRamp.FlagONOFF == 0)
		{
			mcState = mcStop;
		}
		else
		{
			Motor_Init();
			#if (TailWindEnable == 0)
			{
				#if (MOTOR_ALIGN_Enable == 1)
				{
					mcState = mcAlign;
					Align_Calib.Counter = ALIGN_TIME;
				}
				#elif (MOTOR_ALIGN_Enable == 0)
				{
					mcState = mcStart;
				}
				#endif
			}
			#elif (TailWindEnable == 1)
			{
				mcState = mcTailWind;
			}
			#endif
		}
    }
	break;
    case mcTailWind:
	{

		if (mcSpeedRamp.FlagONOFF == 0)
		{
			mcState = mcStop;
		}
		else
		{
			Motor_TailWind();			
			if (mcFocCtrl.StartStaFlag == 1)
			{
				mcState = mcRun;
			}
		}
    }
		break;
    case mcAlign:
    {
		if (mcSpeedRamp.FlagONOFF == 0)
		{
			mcState = mcStop;
		}
		else
		{
			#if (TailWindEnable == 1)
			{
				BEMFDetect.BEMF_Function_Count = 0;
				ClrBit(CMP_CR0, CMP2IM1 | CMP2IM0 | CMP1IM1 | CMP1IM0 | CMP0IM1 | CMP0IM0);
				ClrBit(CMP_CR2, CMP0EN);
			}
			#endif
			
			#if (MOTOR_STARTDELAY_Enable == 1)
			{
				if (Motor_DelayStart_CNT == 0)
				{
					if (StartDelay_Flag == 0)
					{
						mcState = mcReady;
					}
					else
					{
						Motor_Align();
					}
					StartDelay_Flag = 1;
				}
				else
				{
					#if (MOTOR_BRAKE_MODE == ALWAYSBRAKE || MOTOR_BRAKE_MODE == ONLYSTARTBRAKE)
					{
						if (McStaSet.SetFlag.StartBreakFlag == 0)
						{
							FOC_CR1 = 0x00;
							ClrBit(DRV_CR, FOCEN);
							DRV_DR   = DRV_ARR + 1;
							DRV_CMR  = 0x00;
							DRV_CMR |= 0x015;
							ClrBit(DRV_CR, OCS);
							SetBit(DRV_CR, DRVEN);
							MOE = 1;
							McStaSet.SetFlag.StartBreakFlag = 1;
						}
						
					}
					#endif
					// do something.
				}
			}
			#elif (MOTOR_STARTDELAY_Enable == 0)
			{
				Motor_Align();
			}
			#endif
			
			#if (AlignTestMode == 1)
			{
				while(1);
			}
			#endif
		}
      break;
    }
    
    case mcStart:
    {
		if (mcSpeedRamp.FlagONOFF == 0)
		{
			mcState = mcStop;
		}
		else
		{
			Motor_Open();
		}
    }
    break;
    case mcRun:
    {
		if ((mcSpeedRamp.TargetValue == 0) || (mcSpeedRamp.FlagONOFF == 0))
		{
			mcState = mcStop;
			FOC_IQREF = 0;
			mcFocCtrl.State_Count = 1000;
		}
    }
		break;
    case mcStop:
    {
		if (McStaSet.SetFlag.StopSetFlag == 0)
		{
			McStaSet.SetFlag.StopSetFlag = 1;
			#if (MOTOR_BRAKE_MODE == ALWAYSBRAKE || MOTOR_BRAKE_MODE == ONLYSTOPBRAKE)
			{
				McStaSet.SetFlag.brakeSetFlag = 1;
				ClrBit(DRV_CR, DRVEN);
				MOE = 0;
				FOC_CR1 = 0x00;
				ClrBit(DRV_CR, FOCEN);
				DRV_DR   = (DRV_ARR + 1);
				DRV_CMR  = 0x00;
				DRV_CMR |= 0x015;
				ClrBit(DRV_CR, OCS);
				SetBit(DRV_CR, DRVEN);
				MOE = 1;
			}
			#else
			{
				FOC_CR1 = 0x00;
				ClrBit(DRV_CR, FOCEN);
				ClrBit(DRV_CR, DRVEN);
				MOE = 0;
			}
			#endif
		}
		
		if(mcSpeedRamp.FlagONOFF == 1 && mcFocCtrl.State_Count == 0)
		{
			BEMFDetect.BEMFBrakeFlag = 0;
			SetBit(DRV_CR , DRVEN);
			DRV_DR  = 0;
			mcState = mcReady;
		}
    }
    break;
    case mcBrake: break;
    case mcFault:
    {
		if (mcFocCtrl.FaultStopFlag == 0)
		{
			FaultProcess();
			mcFocCtrl.FaultStopFlag = 1;
		}
		ClrBit(CMP_CR0, CMP2IM1 | CMP2IM0 | CMP1IM1 | CMP1IM0 | CMP0IM1 | CMP0IM0);
		ClrBit(CMP_CR2, CMP0EN);
		_nop_();
      
		#if (MOTOR_BRAKE_MODE == ALWAYSBRAKE || MOTOR_BRAKE_MODE == ONLYSTOPBRAKE)
		{
			if(McStaSet.SetFlag.brakeSetFlag == 0)
			{
			McStaSet.SetFlag.brakeSetFlag = 1;
			ClrBit(DRV_CR, DRVEN);
			MOE = 0;
			FOC_CR1 = 0x00;
			ClrBit(DRV_CR, FOCEN);
			
			if (ADCSampling.DcBus_Flt < Volt2DcBus(6.5))
			{
				DRV_DR   = (DRV_ARR * 0.92);
			}
			else
			{
				DRV_DR   = (DRV_ARR + 1);
			}
			
			DRV_CMR  = 0x00;
			DRV_CMR |= 0x015;
			ClrBit(DRV_CR, OCS);
			SetBit(DRV_CR, DRVEN);
			MOE = 1;
			}
		}
		#endif
		if (mcFaultSource.Source == NoFault)
		{
      mcState = mcReady;
		}
      break;
    }
    default:
    break;
  }
}

