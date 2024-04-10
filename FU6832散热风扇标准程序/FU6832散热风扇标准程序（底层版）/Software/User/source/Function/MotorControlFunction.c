/* --------------------------- (C) COPYRIGHT 2020 Fortiortech ShenZhen -----------------------------
    File Name      : MotorControlFunction.c
    Author         : Fortiortech  Appliction Team
    Version        : V1.0
    Date           : 2020-04-10
    Description    : This file contains .C file function used for Motor Control.
----------------------------------------------------------------------------------------------------  
                                       All Rights Reserved
------------------------------------------------------------------------------------------------- */
#include "FU68xx_4.h"
#include "MyProject.h"

CurrentOffset xdata mcCurOffset;
AlignCalib_TypeDef  Align_Calib;

#if (MOTOR_STARTDELAY_Enable == 1)
  uint16 Motor_DelayStart_CNT = 0;
#endif



/* -------------------------------------------------------------------------------------------------
    Function Name  : FOC_Init
    Description    : mcInit状态下，对FOC的相关寄存器进行配置,先清理寄存器，后配置，最后使能
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void FOC_Init(void)
{
  ClrBit(DRV_CR , DRVEN);
  /*使能FOC*/
  ClrBit(DRV_CR , FOCEN);
  SetBit(DRV_CR , FOCEN);

  SetBit(FOC_CR0 , ESCMS);  

  ClrBit(FOC_CR0 , MERRS1); 
  ClrBit(FOC_CR0 , MERRS0); 

  FOC_EOMEKLPF    = 0xff;  

  FOC__EOME     = 0;
  FOC_EFREQACC  = 0;
  FOC_EFREQMIN  = 0;
  FOC_EFREQHOLD = 0;
  /*配置FOC寄存器*/
  FOC_CR1         = 0;                                    // 清零 FOC_CR1
  FOC_CR2         = 0;                                    // 清零 FOC_CR2
  FOC_IDREF       = 0;                                    // 清零 Id
  FOC_IQREF       = 0;                                    // 清零 Iq

  FOC__THETA      = 0;                                    // 清零 角度
  FOC_RTHEACC     = 0;                                    // 清零 爬坡函数的初始加速度
  FOC__RTHESTEP   = 0;                                    // 清零 爬坡速度
  FOC__RTHECNT    = 0;                                    // 清零 爬坡次数
  
  #if (SMO_OffSET_Enable == 1)
  {
    FOC__THECOMP  = _Q15(SMO_OffSET_DEGREE / 180.0);     // SMO 估算補償角
  }
  #elif (SMO_OffSET_Enable == 0)
  {
   FOC__THECOMP  = _Q15(0.0/180.0);         
  }
  #endif
  
  FOC__THECOR     = 0x04;                                 // 误差角度补偿

  FOC_DMAX        =  DOUTMAX;
  FOC_DMIN        = -DOUTMAX;
  FOC_QMAX        =  QOUTMAX;
  FOC_QMIN        = -QOUTMAX;

  /*位置估算参数配置*/
  FOC_EK1         = OBS_K1T;
  FOC_EK2         = OBS_K2T;
  FOC_EK3         = OBS_K3T;
  FOC_EK4         = OBS_K4T;
  FOC_FBASE       = OBS_FBASE;
  FOC_OMEKLPF     = SPEED_KLPF;
  FOC_EBMFK       = OBS_KLPF;

  FOC_TGLI        = 0x00;																	// PWM_TGLI_LOAD;

  ClrBit(FOC_CR2, ESEL);																	// 使用 SMO 估算
  FOC_KSLIDE    = _Q15(OBS_KSLIDE);
  FOC_EKLPFMIN  = OBS_EA_KS;			

  SetBit(FOC_CR1 , SVPWMEN);                              // SVPWM 模式

  if(mcFRState.TargetFR == CW)
  {
      ClrBit(DRV_CR , DDIR);                              // 反转标志位      
  }
  else
  {
      SetBit(DRV_CR , DDIR);                              // 反轉設定      
  }
  
  /**过调制**/
  #if (OverModulation_Enable == 1)
  {
    SetBit(FOC_CR1 , OVMDL);                            // 過調適
  }
  #endif //end OverModulation

//    SetBit(FOC_CR0 , UCSEL);															// 取樣 ADC 通道設定[預設AD14]
  #if (HW_RV_MODE == RV6P5 || HW_RV_MODE == RV12)
  {
    SetBit(FOC_CR0 , UCSEL); 								
  }
  #elif (HW_RV_MODE == RVEXT)
  {
    ClrBit(FOC_CR0 , UCSEL);
  }
  #endif


  /*单电阻采样；需要最小采样窗,FOC_TRGDLY为0，七段式SVPWM方式*/
  #if (Shunt_Resistor_Mode == Single_Resistor)
  {
    SetReg(FOC_CR1 , CSM0 | CSM1 , 0x00);
    FOC_TSMIN  = PWM_TS_LOAD;                           // 最小采样窗口
    FOC_TRGDLY = 0x05;                                  // 采样时刻在中点，一般考虑开关噪声影响，会设置延迟；
                                                        // 0x0c表示延迟12个clock，提前用反码形式，如0x84表示提前12个clock。
    ClrBit(FOC_CR2 , F5SEG);                            // 7段式
    SetReg(CMP_CR1 , CMP3MOD0 | CMP3MOD1, 0x00);
  }
  /*双电阻采样，可设置死区补偿值，在下降沿结束前开始采样Ia，配置81*/
  #elif (Shunt_Resistor_Mode == Double_Resistor)          // double resistor sample
  {
    SetReg(FOC_CR1 , CSM0 | CSM1 , CSM0);

    FOC_TSMIN = PWM_DT_LOAD;                            // 死区补偿值
    FOC_TRGDLY = 0x06;                                  // ADC采样的时刻，采样时刻在计数器零点附近，83为下降沿结束前3个clock采样Ia，与单电阻不同
                                                        // 01为上升沿开始后第一个clock开始采样。根据实际情况调整。
    FOC_TBLO = PWM_DLOWL_TIME;                          //下桥臂最小脉冲，保证采样
    SetReg(CMP_CR1 , CMP3MOD0 | CMP3MOD1 , 0x00);

    /*五段式或七段式选择*/
    #if (SVPMW_Mode == SVPWM_7_Segment)
    {
        ClrBit(FOC_CR2 , F5SEG);                        // 7段式
    }
    #elif (SVPMW_Mode == SVPWM_5_Segment)
    {
        SetBit(FOC_CR2 , F5SEG);                        // 5段式
    }
    #endif

    #if (DouRes_Sample_Mode == DouRes_1_Cycle)
    {
        ClrBit(FOC_CR2 , DSS);                          // 7段式
    }
    #elif (DouRes_Sample_Mode == DouRes_2_Cycle)
    {
        SetBit(FOC_CR2 , DSS);                          // 5段式
    }
    #endif //end DouRes_Sample_Mode
  }
  /*三电阻采样*/
  #elif (Shunt_Resistor_Mode == Three_Resistor)           // signel resistor sample
  {

    SetReg(FOC_CR1, CSM0 | CSM1 , CSM0 | CSM1);         // 三电阻

    FOC_TSMIN  = PWM_DT_LOAD;                           // 死区补偿值
    FOC_TRGDLY = 0x06;                                  // ADC采样的时刻，采样时刻在计数器零点附近，83为下降沿结束前3个clock采样Ia，与单电阻不同。
                                        // 01为上升沿开始后第一个clock开始采样。根据实际情况调整。

    SetReg(CMP_CR1, CMP3MOD0 | CMP3MOD1, CMP3MOD0 | CMP3MOD1);
    FOC_TBLO= PWM_OVERMODULE_TIME;                      // 过调制电流采样处理的TB脉宽

    /*五段式或七段式选择*/
    #if (SVPMW_Mode == SVPWM_7_Segment)
    {
        ClrBit(FOC_CR2 , F5SEG);                        // 7段式
    }
    #elif (SVPMW_Mode == SVPWM_5_Segment)
    {
        SetBit(FOC_CR2 , F5SEG);                        // 5段式
    }
    #endif //end SVPMW_Mode

    #if (DouRes_Sample_Mode == DouRes_1_Cycle)
    {
        ClrBit(FOC_CR2 , DSS);                          // 7段式
    }
    #elif (DouRes_Sample_Mode == DouRes_2_Cycle)
    {
        SetBit(FOC_CR2 , DSS);                          // 5段式
    }
    #endif //end DouRes_Sample_Mode
  }
  #endif  //end Shunt_Resistor_Mode


  /* 使能电流基准校正 */
  #if (currentCalib_Enable == 1)
  {
    if(mcCurOffset.OffsetFlag == 1)
    {
      #if (Shunt_Resistor_Mode == Single_Resistor)    // 单电阻校正
      {
        /*set ibus current sample offset*/
        SetReg(FOC_CR2 , CSOC0 | CSOC1 , 0x00);
        FOC_CSO = mcCurOffset.Iw_busOffset;         // 写入Ibus的偏置
      }
      #elif (Shunt_Resistor_Mode == Double_Resistor)  // 双电阻校正
      {
        /*set ia, ib current sample offset*/
        SetReg(FOC_CR2 , CSOC0 | CSOC1 , CSOC0);
        FOC_CSO  = mcCurOffset.IuOffset;            // 写入IA的偏置

        SetReg(FOC_CR2 , CSOC0 | CSOC1 , CSOC1);
        FOC_CSO  = mcCurOffset.IvOffset;            // 写入IB的偏置
      }
      #elif (Shunt_Resistor_Mode == Three_Resistor)   // 三电阻校正
      {
        /*set ibus current sample offset*/
        SetReg(FOC_CR2 , CSOC0 | CSOC1 , CSOC0);
        FOC_CSO = mcCurOffset.IuOffset;             // 写入IA的偏置

        SetReg(FOC_CR2 , CSOC0 | CSOC1 , CSOC1);
        FOC_CSO = mcCurOffset.IvOffset;             // 写入IB的偏置

        SetReg(FOC_CR2 , CSOC0 | CSOC1 , 0x00);
        FOC_CSO = mcCurOffset.Iw_busOffset;         // 写入IC的偏置
      }
      #endif  //end Shunt_Resistor_Mode
    }
  }
  #endif  //end CalibENDIS
  /*-------------------------------------------------------------------------------------------------
  DRV_CTL：PWM来源选择
  OCS = 0, DRV_COMR
  OCS = 1, FOC/SVPWM/SPWM
  -------------------------------------------------------------------------------------------------*/
  SetBit(DRV_CR , DRVEN);
  /*计数器比较值来源FOC*/
  SetBit(DRV_CR , OCS);
}

/* -------------------------------------------------------------------------------------------------
    Function Name  : Motor_Charge
    Description    : 预充电，当一直处于预充电状态下，不接电机，可用于验证IPM或者Mos。
                     预充电分三步，第一步是对U相进行预充电，第二步是对U,V两相进行预充电;第三步是对U、V、W三相进行预充电。
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void Motor_Charge(void)
{
  DRV_CMR |= 0x3F;                         // U、V、W相输出
}

/* -------------------------------------------------------------------------------------------------
    Function Name  : Motor_Align
    Description    : 预定位函数，当无逆风判断时，采用预定位固定初始位置;当有逆风判断时，采用预定位刹车
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
int16 Align_Value = 0;
void Motor_Align(void)
{
  int16 tmpUD_Value = 0;
  if(McStaSet.SetFlag.AlignSetFlag == 0)
  {
    McStaSet.SetFlag.AlignSetFlag = 1;
    /* -----FOC初始化----- */
    FOC_Init();
    MOE = 0;
    
    SetBit(FOC_CR2, UQD);								// 關閉 UQ PI 控制器
    SetBit(FOC_CR2, UDD);								// 關閉 UD PI 控制器
    
    Align_Calib.Align_IA = 0; 
    Align_Calib.Align_IB = 0;
    Align_Calib.Align_IC = 0;
    Align_Calib.Update_CNT = 0;
  
    //memset(&Align_Calib, 0, sizeof(Align_Calib));
    
    if(BEMFDetect.BEMFCCWFlag == 0)
    {
      FOC__UD = Align_Value;
      FOC__UQ = 0;
    }
    else
    {
      FOC__UD = Align_Value + _Q15(0.1);
      FOC__UQ = 0;
    }

    FOC_EKP     = OBSW_KP_GAIN;
    FOC_EKI     = OBSW_KI_GAIN;
    FOC__THETA  = Angle2RawTHETA(0.0);

    /*輸出致能*/
    DRV_CMR |= 0x3F; 
    MOE = 1;
  }
  else
  {
    if (Align_Calib.Counter == 0)
    {
      mcState = mcStart;
    }
    else if (Align_Calib.Counter <= (ALIGN_TIME - 150))
    {
      FOC__THETA = Angle2RawTHETA(ALIGN_ANGLE);
    }
    else
    {
      if (Align_Calib.Update_CNT == 0)
      {
        tmpUD_Value = (Align_Calib.Align_IA + Align_Calib.Align_IB + Align_Calib.Align_IC) >> 1;
        if ((tmpUD_Value) < (ALIGN_DQ_VALUE - I_Value(0.1)) && FOC__UD < ALIGN_DQ_MAXVALUE)
        {
          FOC__UQ  = 0;
          FOC__UD += _Q15(0.001);
        }
        else if ((tmpUD_Value) > (ALIGN_DQ_VALUE + I_Value(0.1)) && FOC__UD > 0)
        {
          FOC__UQ  = 0;
          FOC__UD -= _Q15(0.001);
        }
        SetReg(FOC_CR2, ICLR, ICLR);
        Align_Calib.Update_CNT  = 1;
      }
    }
  }
}


/* -------------------------------------------------------------------------------------------------
    Function Name  : Motor_Open
    Description    : 开环启动的参数配置
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void Motor_Open(void)
{
  #if (Open_Start_Mode == Open_Start)
  {
    static uint8 OpenRampCycles = 0;
  }
  #endif

  if (McStaSet.SetFlag.StartSetFlag == 0) 
	{
		McStaSet.SetFlag.StartSetFlag = 1;
	}
  FOC_Init();

	//FOC__THETA  = Align_Theta + _Q15(-20.0/180.0);             
  FOC__ETHETA = _Q15(-45.0/180.0);//4096;
	//FOC__ETHETA = FOC__THETA;
		
  //TimeCnt = FOC__ETHETA;
	
//	if (BEMFDetect.BEMFCCWFlag == 1)    
//	{		
//		mcFocCtrl.mcIqref += I_Value(0.5);                    //逆风时，加大启动电流
//	}
//	else if (BEMFDetect.BEMFCCWFlag == 2) 
//	{
//		mcFocCtrl.mcIqref += I_Value(0.3);                    //逆风时，加大启动电流
//	}
  FOC_DQKP = DQKP_START;
  FOC_DQKI = DQKI_START;
	
	/*啟動模式設置*/
  #if (Open_Start_Mode == Omega_Start)              				// Omega 啟動
  {
		FOC_EKP  = OBSW_KP_GAIN;
    FOC_EKI  = OBSW_KI_GAIN;
    FOC_EFREQACC  = Motor_Omega_Ramp_ACC;
    FOC_EFREQMIN  = Motor_Omega_Ramp_Min;
    FOC_EFREQHOLD = Motor_Omega_Ramp_End;
    SetReg(FOC_CR1, EFAE | RFAE | ANGM, EFAE | ANGM);
		
		mcFocCtrl.State_Count = 1500;
		mcState   = mcRun;
  }
  #elif (Open_Start_Mode == Open_Start)											// 強拖啟動
  {
    FOC_RTHEACC   = Motor_Open_Ramp_ACC;     
    FOC__RTHESTEP = Motor_Open_Ramp_Min;      
    FOC__RTHECNT  = MOTOR_OPEN_ACC_CNT;    
    SetReg(FOC_CR1, EFAE | RFAE | ANGM , RFAE);
		if(OpenRampCycles < (MOTOR_OPEN_ACC_CYCLE - 1))
    {
			if(!ReadBit(FOC_CR1 , RFAE))
      {
				SetBit(FOC_CR1 , RFAE);
        OpenRampCycles++;
      }
    }
    else
    {
			mcFocCtrl.State_Count = 2;
      mcState = mcRun;
    }
    FOC_EKP = OBSW_KP_GAIN_RUN4;                           
    FOC_EKI = OBSW_KI_GAIN_RUN4;   
  }
  #elif (Open_Start_Mode == Open_Omega_Start)								// 混合啟動[先強拖後 Omega 啟動]
  {
		FOC_EKP  = OBSW_KP_GAIN;
		FOC_EKI  = OBSW_KI_GAIN;
    FOC_RTHEACC   = MOTOR_OPEN_ACC;      
    FOC__RTHESTEP = Motor_Open_Ramp_Min;    
    FOC__RTHECNT   = MOTOR_OPEN_ACC_CNT;          
    FOC_EFREQACC  = Motor_Omega_Ramp_ACC;
    FOC_EFREQMIN  = MOTOR_OMEGA_ACC_MIN;
    FOC_EFREQHOLD = MOTOR_OMEGA_ACC_END;
    SetReg(FOC_CR1, EFAE | RFAE | ANGM, EFAE | RFAE | ANGM);
		//SetReg(FOC_CR1, EFAE | RFAE | ANGM, EFAE | ANGM);
		mcFocCtrl.State_Count = 700;
    mcState = mcRun;
  }
  #endif 
		
	FOC_IDREF = ID_Start_CURRENT;                         		// D轴启动电流
	
	if (BEMFDetect.BEMFCCWFlag != 0)    
	{		
		 mcFocCtrl.mcIqref = IQ_Tailwind_Start_CURRENT;                    //逆风时，加大启动电流
	}
	else
	{
		 mcFocCtrl.mcIqref = IQ_Start_CURRENT;                 		// Q轴启动电流
	}
	
  //mcFocCtrl.mcIqref = IQ_Start_CURRENT;                 		// Q轴启动电流
  FOC_IQREF = mcFocCtrl.mcIqref;                          	// Q軸啟動電流
	
  ClrBit(FOC_CR2, UQD);
  ClrBit(FOC_CR2, UDD);

}

/* -------------------------------------------------------------------------------------------------
    Function Name  : Motor_TailWind
    Description    : 顺风逆风参数配置函数
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void Motor_TailWind(void)
{
    if(mcFocCtrl.State_Count == 0)
    {
      if(BEMFDetect.BEMFBrakeFlag == 0)
      {
        if(McStaSet.SetFlag.TailWindSetFlag == 0)
        {
            McStaSet.SetFlag.TailWindSetFlag = 1;
            BEMFDetectInit();
        }
        BEMFDealwith();
      }
      else
      {
					mcState             = mcAlign;
					Align_Calib.Counter = TailWindAlign_Time;
      }
    }
}

/* -------------------------------------------------------------------------------------------------
    Function Name  : MC_Stop
    Description    : inital motor control parameter
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void MC_Stop(void)
{
    MOE     = 0;
    ClrBit(DRV_CR , FOCEN);   //关闭FOC                                                                                                         // disable FOC output and initial register
    mcState = mcInit;
}

/* -------------------------------------------------------------------------------------------------
    Function Name  : MotorcontrolInit
    Description    : 控制变量初始化清零,包括保护参数的初始化、电机状态初始化
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void MotorcontrolInit(void)
{
    /* -----保护参数初始化----- */
    faultVarible_Initial();
    
    /* -----过流保护----- */
    memset(&mcCurVarible , 0 , sizeof(CurrentVarible));   // 电流保护的变量清零
    /* -----顺逆风判断参数初始化----- */
    memset(&BEMFDetect , 0 , sizeof(BEMFDetect_TypeDef));//  BEMFDetect所有变量清零

    /*****电机状态机时序变量***********/
    McStaSet.SetMode                   = 0;
    
    /**********电机目标方向*************/
    #if (MOTOR_DIRECTION == 0)
    {
      mcFRState.TargetFR = CW;
    }
    #else
    {
      mcFRState.TargetFR = CCW;
    }
    #endif //end IRMODE

    /* -----外部控制环----- */
    memset(&mcFocCtrl , 0 , sizeof(FOCCTRL));           // mcFocCtrl变量清零
    /* -----ADC采样滤波值----- */
    memset(&ADCSampling, 0, sizeof(ADCSample_TypeDef));    // ADCSample变量清零
    /* -----电流偏置校准变量初始化----- */
    memset(&mcCurOffset , 0 , sizeof(CurrentOffset));  // mcCurOffset变量清零
    mcCurOffset.IuOffsetSum            = 16383;
    mcCurOffset.IvOffsetSum            = 16383;
    mcCurOffset.Iw_busOffsetSum        = 16383;
    /* -----速度环响应参数初始化----- */
    memset(&mcSpeedRamp , 0 , sizeof(MCRAMP));        // mcSpeedRamp变量清零
    /* -----PWM调速变量初始化----- */
    memset(&mcPwmInput , 0 , sizeof(PWMINPUTCAL));    // mcPwmInput变量清零
//    mcPwmInput.PWMARR = 32767;
    
    mcSpeedRamp.FlagONOFF = 1;
}


/* -------------------------------------------------------------------------------------------------
    Function Name  : VariablesPreInit
    Description    : 初始化电机参数
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void VariablesPreInit(void)
{
  mcFaultSource.Source = NoFault;
	/* -----顺逆风参数初始化----- */
  tailWindVarible_Initial();
  /* -----保护参数初始化----- */
  faultVarible_Initial();
  /* -----外部控制环参数初始化----- */
  memset(&mcFocCtrl , 0 , sizeof(FOCCTRL));            // mcFocCtrl变量清零
  memset(&mcVspInput,0, sizeof(VSPINPUTCAL));
  /*****电机状态机时序变量***********/
  McStaSet.SetMode                  = 0x01;           //电流校准标志位置1，其它置0
}

/* -------------------------------------------------------------------------------------------------
    Function Name  : GetCurrentOffset
    Description    : 上电时，先对硬件电路的电流进行采集，写入对应的校准寄存器中。
                     调试时，需观察mcCurOffset结构体中对应变量是否在范围内。采集结束后，OffsetFlag置1。
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void GetCurrentOffset(void)
{
	if(!mcCurOffset.OffsetFlag)
  {
	SetBit(ADC_CR , ADCBSY);            // 使能ADC
    while(ReadBit(ADC_CR , ADCBSY));

    #if (Shunt_Resistor_Mode == Single_Resistor)                   //单电阻模式
    {
	  mcCurOffset.Iw_busOffsetSum += ((ADC4_DR& 0x7ff8));
      mcCurOffset.Iw_busOffset     = mcCurOffset.Iw_busOffsetSum >> 4;
      mcCurOffset.Iw_busOffsetSum -= mcCurOffset.Iw_busOffset;
    }
    #elif (Shunt_Resistor_Mode == Double_Resistor)                 //双电阻模式
    {
	  mcCurOffset.IuOffsetSum     += ((ADC0_DR& 0x7ff8));
      mcCurOffset.IuOffset         = mcCurOffset.IuOffsetSum >> 4;
      mcCurOffset.IuOffsetSum     -= mcCurOffset.IuOffset;

      mcCurOffset.IvOffsetSum     += ((ADC1_DR& 0x7ff8));
      mcCurOffset.IvOffset         = mcCurOffset.IvOffsetSum >> 4;
      mcCurOffset.IvOffsetSum     -= mcCurOffset.IvOffset;
    }
	#elif (Shunt_Resistor_Mode == Three_Resistor)                 //三电阻模式
    {
	  mcCurOffset.IuOffsetSum     += ((ADC0_DR& 0x7ff8));
      mcCurOffset.IuOffset         = mcCurOffset.IuOffsetSum >> 4;
      mcCurOffset.IuOffsetSum     -= mcCurOffset.IuOffset;
			
	  mcCurOffset.IvOffsetSum     += ((ADC1_DR& 0x7ff8));
	  mcCurOffset.IvOffset         = mcCurOffset.IvOffsetSum >> 4;
	  mcCurOffset.IvOffsetSum     -= mcCurOffset.IvOffset;
      
	  mcCurOffset.Iw_busOffsetSum += ((ADC4_DR& 0x7ff8));
	  mcCurOffset.Iw_busOffset     = mcCurOffset.Iw_busOffsetSum >> 4;
	  mcCurOffset.Iw_busOffsetSum -= mcCurOffset.Iw_busOffset;
	}
    #endif

    mcCurOffset.OffsetCount++;
		
    if(mcCurOffset.OffsetCount > Calib_Time)
	{			
		mcCurOffset.OffsetFlag = 1;		
	}
  }
}


/* -------------------------------------------------------------------------------------------------
    Function Name  : Motor_Ready
    Description    : 上电时，关闭输出，先对硬件电路的电流进行采集，在FOC_Init中写入对应的校准寄存器中。
                     调试时，需观察mcCurOffset结构体中对应变量是否在范围内。
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void Motor_Ready(void)
{
	if(McStaSet.SetFlag.CalibFlag == 0)
  {
	McStaSet.SetFlag.CalibFlag = 1;
    ClrBit(DRV_CR , FOCEN);
    ClrBit(DRV_CR , DRVEN);
    MOE = 0;
    SetBit(ADC_MASK, CH4EN | CH2EN | CH1EN | CH0EN);
    mcCurOffset.OffsetFlag  = 0;  
	/*20200704 更新*/
	mcCurOffset.OffsetCount 		= 0;
	/**************/
	BEMFDetect.BEMFBrakeFlag       = 0;
	BEMFDetect.BEMFBrakeStepFlag = 0;
	BEMFDetect.BEMFStartDelayStatus = 0;
	BEMFDetect.BEMF_Function_Count = 0;
	BEMFDetect.BEMF_Function_Flag = 0;
	BEMFDetect.BEMF_Function_Time_Out_Count = 0;
	BEMFDetect.BEMF_Function_Time_Out_Flag = 0;
  }
}


/* -------------------------------------------------------------------------------------------------
    Function Name  : Motor_Init
    Description    : 对电机相关变量、PI进行初始化设置
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void Motor_Init(void)
{
//	ClrBit(ADC_MASK , CH4EN | CH1EN | CH0EN);	  // 关闭软件电流采样的ADC
	ClrBit(ADC_MASK , CH4EN | CH0EN);	  // 关闭软件电流采样的ADC
  VariablesPreInit();                           // 电机相关变量初始化
  PI_Init();                                    // PI初始化   
  SetBit(DRV_CR , DRVEN);
}
