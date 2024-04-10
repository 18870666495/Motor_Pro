/* --------------------------- (C) COPYRIGHT 2020 Fortiortech ShenZhen -----------------------------
    File Name      : Interrupt.c
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

#if (SPEED_MODE == PWMMODE)
 #include "PWM_SpeedControl.h"
#endif

#if (HPFCOL_Enable == 1)
 #include "HPFCOL_Function.h"
#endif

#if (AVC_TURBOFAN_Enable == 1)
 #include "turboFanFunction.h"
#endif

extern uint8 data g_1mTick;                   ///< 1ms滴答信号，每隔1ms在SYSTICK定时器被置1，需在大循环使用处清零
uint16 xdata spidebug[4] = { 0 };

void LVW_TSD_INT(void) interrupt 0 	//LVW & TSD interrupt
{

  if(TSDIF)
  {
    TSDIF = 0;
  }

}


/* -------------------------------------------------------------------------------------------------
    Function Name  : FO_INT
    Description    : FO_INT interrupt，硬件FO过流保护，关断输出，中断优先级最高
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
//void FO_INT(void) interrupt 1               // 硬件FO过流中断，关闭输出
//{
//	if(IF0)
//  {
//		FaultProcess();                         // 关闭输出
//    mcFaultSource = FaultHardOVCurrent;     // 硬件过流保护
//    mcState = mcFault;                      // 状态为mcFault
//    IF0 = 0;                                // clear P00 interrupt flag
//  }
//}


/* -------------------------------------------------------------------------------------------------
    Function Name  : EXTERN_INT
    Description    : 睡眠唤醒
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void EXTERN_INT(void) interrupt 2
{
//    if(SleepSet.SleepFlag == 1)
//    {
//        SleepSet.SleepFlag = 0;
//        SleepSet.SleepEn   = 1;
//    }
//    ClrBit(P1_IF, P11);
}


/* -------------------------------------------------------------------------------------------------
    Function Name  : DRV_ISR
    Description    : FOC中断(Drv中断),每个载波周期执行一次，用于处理响应较高的程序，中断优先级第二。DCEN开了就会产生中断。
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void DRV_ISR(void) interrupt 3
{
  if(ReadBit(DRV_SR, FGIF))
  {
    ClrBit(DRV_SR, FGIF);
  }
  if(ReadBit(DRV_SR, DCIF))
  {
    /** 軟體過電流檢測函數 **/
    Fault_softOverCurrent(&mcCurVarible);
    
    #if defined (SPI_DBG_SW)	//软件调试模式
    {
      spidebug[0] = SOFT_SPIDATA0;
      spidebug[1] = SOFT_SPIDATA1;
      spidebug[2] = SOFT_SPIDATA2;
      spidebug[3] = SOFT_SPIDATA3;
    }
    #endif
    
    #if (AC_Lose_Function_Enable == 1)
    {
      //dosomething.
    }
    #endif
    
  if(TailWindDetect.TailWindDetectInitFlag)
  {
    TailWindSpeedDetect();
  }

    DRV_SR = (DRV_SR | SYSTIF) & (~DCIF);
  }
}

/* -------------------------------------------------------------------------------------------------
    Function Name  : TIM2_INT
    Description    :
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void TIM2_INT(void) interrupt 4
{
  if(ReadBit(TIM2_CR1, T2IR))
  {
    ClrBit(TIM2_CR1, T2IR);
  }
  if(ReadBit(TIM2_CR1, T2IP))
  {
    ClrBit(TIM2_CR1, T2IP);
  }
  if(ReadBit(TIM2_CR1, T2IF))
  {
    ClrBit(TIM2_CR1, T2IF);
  }
}


/* -------------------------------------------------------------------------------------------------
    Function Name  : CMP_ISR
    Description    : CMP0/1/2：顺逆风判断
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void CMP_ISR(void) interrupt 7
{
  if(ReadBit(CMP_SR, CMP0IF) || ReadBit(CMP_SR, CMP1IF) || ReadBit(CMP_SR, CMP2IF)) //当检测到比较器中断时
  {
    /* -----通过BEMF做顺逆风检测功能----- */
    BEMFDetectFunc();
    ClrBit(CMP_SR, CMP0IF | CMP1IF | CMP2IF);
  }
}

/* -------------------------------------------------------------------------------------------------
    Function Name  : TIM3_INT
    Description    : PWM 調速檢測計數器中斷
    Date           : 2020-09-16
    Parameter      : None
------------------------------------------------------------------------------------------------- */
uint8 AC_Lose_Zero_Flag = 0;
uint8 AC_Lose_Zero_Count = 0;
uint8 AC_Lose_Flag = 0;
uint8 AC_Lose_Step = 0;
uint16 AC_Lose_Power_Limit = 0;
uint8 AC_Lose_Step_Flag = 0;
uint8 AC_Lose_Step_Count = 0;
uint8 AC_Lose_FG_Flag = 0;

void TIM3_INT(void) interrupt 9
{
  #if (HPFCOL_Enable == 1)
  {
    HPFCOL_CommDetection();
  }
  #endif
  
  if (ReadBit(TIM3_CR1, T3IR))
  {
    ClrBit(TIM3_CR1, T3IR);
  }
  if (ReadBit(TIM3_CR1, T3IP))
  {
    #if (SPEED_MODE == PWMMODE)
    {
      PWM_capture();
    }
    #endif
    ClrBit(TIM3_CR1, T3IP);
  }
  
  if (ReadBit(TIM3_CR1, T3IF))
  {
    #if (SPEED_MODE == PWMMODE)
    {
      PWM_overFlowDetection();
    }
    #endif

    #if (AC_Lose_Function_Enable == 1)
    {
      DELL_coastMode_IF_Event();
    }
    #endif
    ClrBit(TIM3_CR1, T3IF);
  }
}


/* -------------------------------------------------------------------------------------------------
    Function Name  : SYStick_INT
    Description    : 1ms定时器中断（SYS TICK中断），用于处理附加功能，如控制环路响应、各种保护等。中断优先级低于FO中断和FOC中断。
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
int16 EK1 = 0;
int16 EK2 = 0;
int16 EK3 = 0;
void SYStick_INT(void) interrupt 10
{
  /* Sys Tick 中斷函數 -開始 */
  if(ReadBit(DRV_SR, SYSTIF))
  {
    /* 預定位強度量測 -開始 */
	if (mcState == mcAlign)
    {
      if (McStaSet.SetFlag.AlignSetFlag == 1)
      {
        if (Align_Calib.Update_CNT != 0)
        {
          Align_Calib.Align_IA = FOC__IAMAX;
          Align_Calib.Align_IB = FOC__IBMAX;
          Align_Calib.Align_IC = FOC__ICMAX;
          Align_Calib.Update_CNT--;
        }
        
        if (Align_Calib.Counter != 0)
        {
          Align_Calib.Counter--;
        }
      }
    }
    /* 預定位強度量測 -結束 */
    
    #if (MOTOR_STARTDELAY_Enable == 1)
    {
      if (Motor_DelayStart_CNT != 0)
        Motor_DelayStart_CNT--;
    }
    #endif
    
    /* 信號輸出函數 -開始 */
    #if (ROTATESIGNAL_TYPE == FG_TYPE)
    {
      SignalOutput_Counter();
    }
    #elif (ROTATESIGNAL_TYPE == RD_TYPE)
    {
      SignalOutput_Counter();
    }
    #endif
    /* 信號輸出函數 -結束 */
    
    /* HPFCOL 計數函數 -開始 */
    #if (HPFCOL_Enable == 1)
    {
      HPFCOLCounter_Event();
    }
    #endif
    /* HPFCOL 計數函數 -結束 */
        
    /* 10 秒計數器 -開始 */
    if (mcFocCtrl.Millis < 10000 && mcFocCtrl.CtrlMode == 1)
    {
      mcFocCtrl.Millis++; 
    }
    /* 10 秒計數器 -結束 */
    
    /*TurboFan 時間計數函數 --開始*/
    #if (AVC_TURBOFAN_Enable == 1)
    {
      TurboFan_CountEvent();
      Uart_CountEvent();
    }
    #endif
    /*TurboFan 時間計數函數 --結束*/
    
    /* 電流限制器 -開始 */
    #if (CURRENT_LIMITER_Enable == 1)
    {
      CurrentLimiter_Process();
    }
    #endif
    /* 電流限制器 -結束 */
    
    
    
    #if (Slowdown_Response_Enable == 1)
    {
      Slowdown_Response_Function();
    }
    #endif
    
    #if (AC_Lose_Function_Enable == 1)
    {
      DELL_coastMode_Process();
      
      /* 未知功能 須詢問 Kevin 添加用意 --開始 */
      if(AC_Lose_Flag == 2)
      {
        PI3_UKMAX = AC_Lose_Limit_Power_Value;
      }
      else
      {
        #if ((CURRENT_LIMITER_Enable != 1) && (Watt_Limit_Enable != 1))
        {
          PI3_UKMAX = SOUTMAX;
        }
        #endif
      }
      /* 未知功能 須詢問 Kevin 添加用意 --結束 */
    }
    #elif (AC_Lose_Function_Enable == 0)
    {
      
      /* 未知功能 須詢問 Kevin 添加用意 --開始 */
      #if ((CURRENT_LIMITER_Enable != 1) && (Watt_Limit_Enable != 1))
      {
        PI3_UKMAX = SOUTMAX;
      }
      #endif
      /* 未知功能 須詢問 Kevin 添加用意 --結束 */
      
    }
    #endif

    SetBit(ADC_CR, ADCBSY);          	// 使能 ADC 的 DCBUS 采样
    EK1 = FOC_EK1;
    EK2 = FOC_EK2;
    EK3 = FOC_EK3;


    mcFocCtrl.QoutValue     = FOC__UQ;
    mcFocCtrl.DoutValue     = FOC__UD;

    /* -----功率滤波----- */
    if(mcState == mcRun)
    {
      mcFocCtrl.CurrentPower = FOC__POW << 1;
      LPF_MDU(mcFocCtrl.CurrentPower, 10, mcFocCtrl.Powerlpf, mcFocCtrl.Powerlpf_LSB);//注意低通滤波器系数范围为0---127
    }
    else
    {
      mcFocCtrl.Powerlpf = 0;
    }

    /* -----速度滤波----- */
    if((mcState == mcStart) || (mcState == mcRun))
    {
      LPF_MDU(FOC__EOME, 10, mcFocCtrl.SpeedFlt, mcFocCtrl.SpeedFlt_LSB);
    }
    else
    {
      mcFocCtrl.SpeedFlt = 0;
    }

    Atan_Us_MDU(FOC__EALP, FOC__EBET, mcFocCtrl.EsValue);

    Speed_response();

    #if (Watt_Limit_Enable == 1)
    {
      Watt_Limit_Function();
    }
    #endif


    /*****DCbus的采样获取值并滤波******/
    #if (HW_RV_MODE == RV6P5 || HW_RV_MODE == RV12)
    {
      ADCSampling.DcBus_SamplingValue = ADC14_DR;
    }
    #elif (HW_RV_MODE == RVEXT)
    {
      ADCSampling.DcBus_SamplingValue = ADC2_DR;
    }
    #endif

    LPF_MDU((ADCSampling.DcBus_SamplingValue) , 50, ADCSampling.DcBus_Flt, ADCSampling.DcBusFlt_Buff);
    //ADCSampling.DcBus_Flt = ADCSampling.DcBus_SamplingValue;

    
    /*-----------------------------------------------------------------------------------------------------*/

    /** 電機保護輪詢函數 **/
    Fault_Detection();

    /*****电机状态机的时序处理*****/
    if(mcFocCtrl.State_Count > 0)
      mcFocCtrl.State_Count--;
    StarRampDealwith();
    if(BEMFDetect.BEMFTimeCount > 0)
      BEMFDetect.BEMFTimeCount--;
		if(BEMFDetect.BEMF_Function_Flag == 1)
		{
			BEMFDetect.BEMF_Function_Count++;
		}
		if(BEMFDetect.BEMF_Function_Flag == 2)
		{
				BEMFDetect.BEMFMotorStartLowSpeedFilter++;
		}		
		if((BEMFDetect.BEMF_Function_Time_Out_Flag == 1)&&(BEMFDetect.BEMF_Function_Time_Out_Count < 5000))
		{
			BEMFDetect.BEMF_Function_Time_Out_Count++;
		}
    DRV_SR = (DRV_SR | DCIF) & (~SYSTIF);
    /* Sys Tick 中斷函數 -結束 */
  }
}


void TIM4_INT(void) interrupt 11
{
  #if (HPFCOL_Enable == 1)
    HPFCOLUpdate_Event();
  #endif
  
  if (ReadBit(TIM4_CR1, T4IR))
  {
    ClrBit(TIM4_CR1, T4IR);
  }
  
  if (ReadBit(TIM4_CR1, T4IF))
  {
    #if (ROTATESIGNAL_TYPE == FG_TYPE)
    {
      FG_Update_Event();
    }
    #endif
    ClrBit(TIM4_CR1, T4IF);
  }
}


/* -------------------------------------------------------------------------------------------------
    Function Name  : CMP3_INT
    Description    : CMP3：硬件比较器过流保护，关断输出，中断优先级最高
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void CMP3_INT(void)  interrupt 12
{
  if(ReadBit(CMP_SR, CMP3IF))
  {
    mcFaultSource.Source = Hard_OverCurrent;                                          // 硬件过流保护
	mcFocCtrl.FaultStopFlag = 0;
    ClrBit(CMP_SR, CMP3IF);
  }
}
