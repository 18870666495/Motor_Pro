/* --------------------------- (C) COPYRIGHT 2020 Fortiortech ShenZhen -----------------------------
    File Name      : main.c
    Author         : Fortiortech  Appliction Team
    Version        : V1.0
    Date           : 2020-04-10
    Description    : This file contains .C file function used for Motor Control.
----------------------------------------------------------------------------------------------------
                                       All Rights Reserved
------------------------------------------------------------------------------------------------- */
/********************************************************************************
* Header Definition
********************************************************************************/
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

uint8 data g_1mTick = 0;  ///< 1ms滴答信号，每隔1ms在SYSTICK定时器被置1，需在大循环使用处清零
/********************************************************************************
* Internal Routine Prototypes
********************************************************************************/
void HardwareInit(void);
void SoftwareInit(void);

/********************************************************************************
* Macro & Structure Definition
********************************************************************************/

/********************************************************************************
* Function Definition
********************************************************************************/
 
uint16 RealVolt = 0;
 
void main(void)
{
  SoftwareInit();
  HardwareInit();
  
  EA = 1;							// 中斷向量致能
  
  while (1)
  {
    GetCurrentOffset();
    MC_Control();
    RealVolt = DcBus2Volt(ADCSampling.DcBus_Flt);
	#if (AVC_TURBOFAN_Enable == 1)
    {
      if (c_TurboFan.isActive == 0)
      {
        #if (SPEED_MODE == PWMMODE)
        {
          PWM_speedControl();
        }
        #elif (SPEED_MODE == NONEMODE)
        {
          mcSpeedRamp.FlagONOFF   = 1;
          mcSpeedRamp.TargetValue = RPM2SpeedFlt(NONEMODE_SPEED);
        }
        #endif
      }
    }
    #else
    {
      #if (SPEED_MODE == PWMMODE)
      {
        PWM_speedControl();
      }
      #elif (SPEED_MODE == VSPMODE)
      {
        VSP_speedControl();
      }      
      #elif (SPEED_MODE == NONEMODE)
      {
        mcSpeedRamp.FlagONOFF   = 1;
        mcSpeedRamp.TargetValue = RPM2SpeedFlt(NONEMODE_SPEED);
      }
      #endif
    }
    #endif

    #if (ROTATESIGNAL_TYPE == FG_TYPE)
    {
      SignalOutput_Process();
    }
    #elif (ROTATESIGNAL_TYPE == RD_TYPE)
    {
      GP00 = SignalOutput_Process();
    }
    #endif
    
      
    /*TurboFan 處理階段函數 --開始*/    
    #if (AVC_TURBOFAN_Enable == 1)
    {
      TurboFan_ProcessEvent();
    }
    #endif
    /*TurboFan 處理階段函數 --結束*/
  }
}

/* -------------------------------------------------------------------------------------------------
    Function Name  : HardwareInit
    Description    : 硬件初始化，初始化需要使用的硬件设备配置，FOC必须配置的是运放电压、运放初始化、ADC初始化、Driver初始化
                    ，其他的可根据实际需求加。
    Date           : 2020-04-12
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void HardwareInit(void)
{
  uint16 PowerUpCnt = 0;
  
  /************************VREF&VHALF Config************************/
  ClrBit(P3_AN, PIN5);                        //VREF Voltage -->P35 Output
  #if (HW_ADC_REF_Volt == REF_5V0)
  {
    ClrBit(VREF_VHALF_CR, VRVSEL1);           // 01-->VDD5
    SetBit(VREF_VHALF_CR, VRVSEL0);             
  }
  #elif (HW_ADC_REF_Volt == REF_4V5)
  {
    ClrBit(VREF_VHALF_CR, VRVSEL1);           // 00-->4.5V
    ClrBit(VREF_VHALF_CR, VRVSEL0);           
  }
  #elif (HW_ADC_REF_Volt == REF_4V0)
  {
    SetBit(VREF_VHALF_CR, VRVSEL1);           // 11-->4.0V
    SetBit(VREF_VHALF_CR, VRVSEL0);          
  }
  #elif (HW_ADC_REF_Volt == REF_3V0)
  {
    SetBit(VREF_VHALF_CR, VRVSEL1);           // 10-->3.0V 
    ClrBit(VREF_VHALF_CR, VRVSEL0);           
  }
  #else 
  {
    #error "HW_ADC_REF setting error!"
  }
  #endif

  SetBit(VREF_VHALF_CR, VREFEN | VHALFEN);    //VREF_VHALF_CR = 0x11;
  /****************************************************************/
  // 为提高芯片的抗干扰能力，降低芯片功耗，请在具体项目时，将不需要用的GPIO默认都配置为输入上拉。
  // 具体配置可在GPIO_Default_Init设置。
  //    GPIO_Default_Init();
  
  /************************硬件外设初始化**************************/

  #if defined (SPI_DBG_HW)        // 硬件调试模式
  {
    SPI_Init();
    Set_DBG_DMA(&HARD_SPIDATA);
  }
  #elif defined (SPI_DBG_SW)      // 软件调试模式
  {
    SPI_Init();
    Set_DBG_DMA(spidebug);
  }
  #elif defined (UART_DBG)        // UART调试模式
  {
    UART2_Init();
    SetPipe_DMA0(DRAM_UART2);
  }
  #endif

  GPIO_Init();
  ADC_Init();
  Driver_Init();
  AMP_Init();
    
  /*該迴圈會造成系統延遲 1S 啟動*/
  for (PowerUpCnt = 0; PowerUpCnt < SystemPowerUpTime; PowerUpCnt++);
    
  /******比较器中断配置**********/
  CMP3_Interrupt_Init();
  CMP3_Init();
  
  #if (SPEED_MODE == PWMMODE || HPFCOL_Enable == 1)
  {
    TIM3_Initial();
  }
  #endif
  
  #if (ROTATESIGNAL_TYPE == FG_TYPE || HPFCOL_Enable == 1)
  {
    TIM4_Initial();
  }
  #endif
  
  #if (AVC_TURBOFAN_Enable == 1)
  {
    UART1_Init();
  }
  #endif
  
  sysTick_Initial();
  
  SetBit(DRV_SR, SYSTIE);
}


/* -------------------------------------------------------------------------------------------------
    Function Name  : SoftwareInit
    Description    : 软件初始化，初始化所有定义变量，按键初始化扫描
    Date           : 2020-04-12
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void SoftwareInit(void)
{
  #if (HPFCOL_Enable == 1)
  {
    HPFCOL_Initial();
  }
  #endif
  
  #if (MOTOR_STARTDELAY_Enable == 1)
  {
    Motor_DelayStart_CNT = (MOTOR_STARTDELAY_TIME);
    StartDelay_Flag = 0;
  }
  #endif
  
  /*TurboFan 初始化函數 --開始*/  
  #if (AVC_TURBOFAN_Enable == 1)
  {
    TurboFan_Initial();
  }
  #endif
  /*TurboFan 初始化函數 --結束*/
  
  /*信號輸出初始化函數 --開始*/
  #if (ROTATESIGNAL_TYPE != NO_TYPE)
  {
    SignalOutput_initial();
  }
  #endif
  /*信號輸出初始化函數 --結束*/  
  
  /*Coast Mode 初始化函數 --開始*/
  #if (AC_Lose_Function_Enable == 1)
  {
    DELL_coastMode_Initial();
  }
  #endif
  /*Coast Mode 初始化函數 --結束*/ 
  
  MotorcontrolInit(); 
  mcState       = mcReady;
  mcFaultSource.Source = NoFault;
}
