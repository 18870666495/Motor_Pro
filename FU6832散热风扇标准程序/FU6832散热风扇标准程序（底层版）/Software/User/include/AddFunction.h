/* --------------------------- (C) COPYRIGHT 2020 Fortiortech ShenZhen -----------------------------
    File Name      : AddFunction.h
    Author         : Fortiortech  Appliction Team
    Version        : V1.0
    Date           : 2020-04-11
    Description    : This file contains all the common data types used for Motor Control.
----------------------------------------------------------------------------------------------------
                                       All Rights Reserved
------------------------------------------------------------------------------------------------- */
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __AddFunction_H_
#define __AddFunction_H_

/******************************************************************************/
#include <FU68xx_4_Type.h>
/******************************************************************************/

#define Speed_Curve_Table_Pint   501 
#define Open_Curve_Table_Pint   501 

/* Exported types ------------------------------------------------------------*/
typedef struct
{
    //Current protect
    uint8  OverCurCnt;                                                          // 软件过流计数

    uint16 Abs_ia;                                                              // IA的绝对值
    uint16 Abs_ib;                                                              // IB的绝对值
    uint16 Abs_ic;                                                              // IC的绝对值

    uint16 Max_ia;                                                              // IA的最大值
    uint16 Max_ib;                                                              // IB的最大值
    uint16 Max_ic;                                                              // IC的最大值
		uint16 FOC_IQREF_Value;	
		uint16 ADC_IBUS;
		uint16 ADC_IBUS_LSB;
		uint16 ADC_IBUS_Offset;
		uint16 ADC_IBUS_Value;
}CurrentVarible;


typedef struct
{
  uint16 DcBus_SamplingValue;                                                  // ADC 電壓採集
  uint16 DcBus_Flt;                                                            // 電壓量測值 (原始)
  uint16 DcBus_Volt;                                                           // 電壓量測值 (轉換)
  int16  DcBusFlt_Buff;                                                        // 濾波器暫存
}ADCSample_TypeDef;

typedef struct
{
  uint16 ADC7_Value;
  uint16 ADC7_LPF_Value;
  uint16 ADC7_LPF_LSB;
  uint16 TargetValue_Open_Value;
  uint16 VSP_Off_Filter_Count;
  uint16 VSP_On_Filter_Count;
  uint8  OnOff_Flag;
  
}VSPINPUTCAL;

typedef struct
{
    uint16 LedCount;                                                            // 闪灯周期计数
    uint16 Counttime;                                                           // 闪灯周期
    uint16 Count;                                                               // 电平翻转计数
    uint8  LedTimCot;                                                           // 闪灯次数计数
}MCLedDisplay;

typedef struct
{
    
    uint16 CtrlMode;                                                            // 控制模式
    uint16 CurrentPower;                                                        // 当前功率
    int16  Powerlpf;                                                            // 功率滤波后的值
    int16  Powerlpf_LSB;                                                        // 功率滤波后的值
    int16  mcIqref;                                                             // Q轴给定电流
    int16  QoutValue;
    int16  DoutValue;
  
    int16  McuRef;
    uint16 mcPosCheckAngle;                                                     // 位置检测的角度
    int32  SQUSysSpeed;                                                         // 速度的平方和
    int16  SpeedFlt;                                                            // 当前速度滤波后的值
    int16  SpeedFlt_LSB;                                                        // 当前速度滤波后的值
    int16  SpeedFlag;
    uint16 EsValue;                                                             // 当前FOC_ESQU滤波后的值
    uint16 SQUSpeedDIVEs;                                                       // 速度平方和除以ESQU的值
    uint16 Smo_EK2;                                                             // EK2的调试值
    uint16 EsValue_LSB;                                                         // 当前FOC_ESQU滤波后的值
    uint8  Uqdelaycnt;
    uint8  ESDIVFlag;                                                           // 除法执行标志位
    uint16 SpeedLoopTime;                                                       // 速度环时间
    uint16 RunStateCnt;                                                         // 运行状态计数
    uint16 PowerLoopTime;                                                       // 功率环时间
    uint16 TorqueLoopTime;                                                  		// 转矩环时间
    uint16 RunCurrent;                                                          // 运行电流
    uint16 ChargeStep;                                                          // 预充电的步骤
    uint16 State_Count;                                                         // 电机各个状态的时间计数
		int16  SpeedFlt_Abs;
    uint16 Millis;
		uint16 Open_DCBus_RetryFlag;
		uint8 TailWindStatus;
		uint8 StartStaFlag;
		uint8 StopStaFlag;
		uint8 FaultStopFlag;
    float Voltage_Value;
    float Current_Value;
    float Watt_Limit_Value;
    uint16 mcDcbusFlt; 
    uint16 Watt_Limit_Current;    
}FOCCTRL;

typedef struct
{
    uint32 SleepDelayCout;
    int32  SleepFlag;
    uint8  SleepEn;
}SLEEPMODE;

typedef struct
{
    uint16 Read;
    uint16 Sum;
    uint16 RealValue;
}VspInput;

typedef struct
{
    int16  TargetValue;
    int16  ActualValue;
    int16  IncValue;
    int16  DecValue;
    int16  DelayCount;
    int16  DelayPeriod;
    int8   FlagONOFF;
}MCRAMP;


typedef struct
{
  	int32 PWMCount;
		uint16 TargetValue_Open_Value;
		uint16 ActualValue_Open_Value;
		uint8  PWMDealFlag;
}PWMINPUTCAL;

typedef struct
{
    uint8 FR;                        // 当前给定的转子方向
    uint8 FlagFR;                    // 反向的标志位
    uint8 FRStatus;
    uint8 TargetFR;                  // 设置的目标方向
    uint16 OldTargetSpeed;
} MotorFRTypeDef;

typedef struct
{
  uint16 TargetFG;
  uint16 ActualFG;
	uint32 mcSpeedFltTemp;
	uint16 PWM_Countrol_Duty;
	uint8  fakeFG_Reoutput_Count;
	uint8  fakeFG_Reoutput_Flag;
	uint16 fakeFG_Reoutput_Times;	
	uint8  FG_Reoutput_Count;
	uint8  FG_Reoutput_Flag;
}fakeFGCurveTpyeDef;

/* Exported variables ---------------------------------------------------------------------------*/
extern PWMINPUTCAL    idata mcPwmInput;
extern CurrentVarible idata mcCurVarible;


extern ADCSample_TypeDef xdata ADCSampling;
extern FOCCTRL           xdata mcFocCtrl;
extern MCRAMP            xdata mcSpeedRamp;
extern int16             xdata mcSpeedFdb;
extern int16             xdata VSP;
extern MotorFRTypeDef   xdata mcFRState;
extern SLEEPMODE        xdata SleepSet;
extern VSPINPUTCAL      idata     mcVspInput;
/* Exported functions ---------------------------------------------------------------------------*/
extern void   VariablesPreInit(void);
extern void   Fault_Detection(void);
extern void   Speed_response(void);
extern void   mc_ramp(MCRAMP *hSpeedramp);
extern void   VSPSample(void);
extern int16  HW_One_PI(int16 Xn1);
extern void   SpeedRegulation(void);
extern uint32 Abs_F32(int32 value);
extern void   StarRampDealwith(void);
extern void   MULDIV_test(void);
extern void   APP_DIV(void);
extern void   PWMInputCapture(void);
extern void   PWM_Curve_Capture(void);
extern void   Weakening_Control(void);
extern void   runSVPWM5_Function(void);
extern void   FOC_TSMIN_Close_Function(void);
extern void   Open_Loop_Function(void);
extern void   FGOutput(void);
extern void   Slowdown_Response_Function(void);
extern uint8	AC_Lose_Flag;
extern uint16 AC_Lose_Power_Limit;
extern uint8	mc_ramp_Flag;
extern uint16 code Open_Curve_Table[Open_Curve_Table_Pint];
extern void   PWM_oscillation_Function(void);
extern uint16 FG_ARR_Value ;
extern void CurrentLimiter_Process(void);
extern void TickCycle_1ms(void);

extern uint16 Open_TailWind_Count;
extern uint16 Open_TailWind_Flag;
extern void Watt_Limit_Function(void);
extern void VSP_speedControl(void);
extern void VSPShutDown_Function(void);
#endif