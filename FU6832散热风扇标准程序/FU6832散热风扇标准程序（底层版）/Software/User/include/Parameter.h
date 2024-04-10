/* --------------------------- (C) COPYRIGHT 2020 Fortiortech ShenZhen -----------------------------
    File Name      : Parameter.h
    Author         : Fortiortech  Appliction Team
    Version        : V1.0
    Date           : 2020-04-11
    Description    : This file contains all FOC debug  parameter used for Motor Control.
----------------------------------------------------------------------------------------------------
                                       All Rights Reserved
------------------------------------------------------------------------------------------------- */
#include "Customer.h"
//#include "Protect.h"
#include "Develop.h"
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __Parameter_H_
#define __Parameter_H_

/* Define to prevent recursive inclusion -------------------------------------*/
/* Q format define ---------------------------------------------------------------------------------*/
#define _Q7(A)                          (int8)((A) * 0x7f)
#define _Q8(A)                          (int16)((A) * 0xff)
#define _Q10(A)                         (int16)((A) * 0x3ff)                                          // Q15 format
#define _Q11(A)                         (int16)((A) * 0x07ff)                                         // Q12 format
#define _Q12(A)                         (int16)((A) * 0x0fff)                                         // Q12 format
#define _Q13(A)                         (int16)((A) * 0x1fff)                                         // Q12 format
#define _Q14(A)                         (int16)((A) * 0x3fff)                                         // Q14 format
#define _Q15(A)                         (int16)((A) * 0x7fff)                                         // Q15 format
#define _2PI                            (3.1415926 * 2)                                               // 2 PI value
#define _Q16                            (65535.0)                                                     // Q16 format value

#define RPM2SpeedFlt(_RPM_)             _Q15((_RPM_) / (MOTOR_SPEED_BASE))
#define SpeedFlt2RPM(_Val_)             (uint16)((float)(_Val_) / 32767.0f * (float)MOTOR_SPEED_BASE)
  
#define Volt2DcBus(_VOLT_)              _Q15((_VOLT_) / HW_BOARD_VOLT_MAX)
#define DcBus2Volt(_Val_)               (uint16)(((float)(_Val_) / 32767.0f * (float)HW_BOARD_VOLT_MAX) * 100)

#define Open_Start_Mode                 (Open_Omega_Start)                      // 電機啟動模式選擇 [設定: Open_Start(強拖啟動) Omega_Start(Omega 啟動) Open_Omega_Start(混合啟動)]
#define MOTOR_OPEN_ACC_CYCLE            (1)                                     // 強拖啟動循環次數

 #define SVPMW_Mode                      (SVPWM_7_Segment) 
 #define DouRes_Sample_Mode              (DouRes_1_Cycle)

/*矯正時間*/
 #define Calib_Time                     (1000)                                  // 矯正時間 單位: mS
  
/*SMO 濾波*/
 #define OBS_KSLIDE                     (0.85)                              		// SMO 帶寬濾波增益

/*估算器 KP KI 參數*/ 
 #define ATT_COEF                       (0.85)                                  // 估算器 Kp Ki 參數
 
 #define currentCalib_Enable						(1)																			// 取樣電流矯正致能 [建議值: 1] [0:禁能 1:致能] 
 #define OUTloop_trans_time					    (10)                                    //  OUTloop PWM命令響應延遲時間 單位: mS

#define Angle2RawTHETA(_Angle_)         _Q15((_Angle_) / 180.0)


#define CurrentLimit_Val                ((uint16)(CURRENT_LIMITER_THRESHOLD / ((float)(HW_ADC_REF / 2.0 / HW_AMPGAIN / HW_RSHUNT) / 16383.0)) + 16383)

#define HW_ADC_REF_Volt                       (REF_5V0)                              // [6832N只能使用REF_5V0 ADC] 參考電壓 [設定:REF_5V REF_4V5 REF_4V REF_3V]  

//((arg1 / (SW_MAX_IValue / 16383)) + SW_OFFset_Val)

#define I_ValueX(Curr_Value)            (Curr_Value * HW_RSHUNT * HW_AMPGAIN / (HW_ADC_REF))
#define I_Value(Curr_Value)             _Q15(I_ValueX(Curr_Value))

/* Private define ------------------------------------------------------------*/
#define SystemPowerUpTime               (15000)                                                       // 上电等待时间，cpu计数时钟

/*芯片参数值------------------------------------------------------------------*/
/*CPU and PWM Parameter*/
#define PWM_CYCLE                       (1000.0 / PWM_FREQUENCY)                                      // 周期us
#define SAMP_FREQ                       (PWM_FREQUENCY * 1000)                                        // 采样频率(HZ)
#define TPWM_VALUE                      (1.0 / SAMP_FREQ)                                             // 载波周期(S)
#define PWM_VALUE_LOAD                  (uint16)((FREC / 1000000.0) * 1000 *2 / PWM_FREQUENCY)                 // PWM 定时器重载值 20190514修改

#define _48MHz 													(1)
#define _24MHz 													(0)
 
/*double resistor sample Parameter*/
#define DLL_TIME                        (1.3)                                                         // 双电阻最小脉宽设置(us),建议值为死区时间值+0.2us以上
/*three resistor overmodule Parameter*/
#define OVERMOD_TIME                    (2.0)                                                         // 三电阻过调制时间(us)，建议值2.0
/*deadtime compensation*/
#define DT_TIME                         (0.4)                                                         // 死区补偿时间(us)，适用于双电阻和三电阻，建议值是1/2死区时间

/*min pulse*/
#define GLI_TIME                        (0.0)                                                         // 桥臂窄脉宽消除(us),建议值0.5

/*deadtime Parameter*/
#define PWM_LOAD_DEADTIME               (PWM_DEADTIME * (FREC / 1000000.0))                                    // 死区设置值
#define PWM_OVERMODULE_TIME             (OVERMOD_TIME * (FREC / 1000000.0) / 2)                                // 过调制时间
#define PWM_DLOWL_TIME                  (DLL_TIME * (FREC / 1000000.0) / 2)                                    //下桥臂最小时间
/*single resistor sample Parameter*/
#define PWM_TS_LOAD                     (uint16)(_Q16 / PWM_CYCLE * MIN_WIND_TIME / 16)               // 单电阻采样设置值
#define PWM_DT_LOAD                     (uint16)(_Q16 / PWM_CYCLE * DT_TIME / 16)                     // 死区补偿值
#define PWM_TGLI_LOAD                   (uint16)(0)  																									// 最小脉冲

/*硬件板子参数设置值------------------------------------------------------------------*/
/*hardware current sample Parameter*/
/*电流基准的电路参数*/
#define HW_BOARD_CURR_MAX               (HW_ADC_REF / 2 / HW_AMPGAIN / HW_RSHUNT)                     // 最大采样电流,2.702A
#define HW_BOARD_CURR_MIN               (-HW_BOARD_CURR_MAX)                                          // 最小采样电流,-2.702A
#define HW_BOARD_CURR_BASE              (HW_BOARD_CURR_MAX * 2)                                       // 电流基准//5.4A

/*hardware voltage sample Parameter*/
/*母线电压采样分压电路参数*/
#define HW_BOARD_VOLT_MAX               (HW_ADC_REF * RV)                                             // (V)  ADC可测得的最大母线电压
//#define HW_BOARD_VOLTAGE_BASE           (HW_BOARD_VOLT_MAX / 1.732)                                   // 电压基准
#define HW_BOARD_VOLTAGE_BASE           (HW_BOARD_VOLT_MAX)                                   // 电压基准
#define HW_BOARD_VOLTAGE_VC             (RV)
#define HW_BOARD_VOLTAGE_BASE_Start     (HW_ADC_REF * HW_BOARD_VOLTAGE_VC / 1.732)                    // 电压基准


#define motorStartOffset_Mod0   				(0.0524)                                // 電壓浮動補償係數 [線性補償插值係數 角度:3.0]
#define motorStartOffset_Mod1   				(0.0699)                                // 電壓浮動補償係數 [線性補償插值係數 角度:4.0]
#define motorStartOffset_Mod2   				(0.0874)                                // 電壓浮動補償係數 [線性補償插值係數 角度:5.0]

/*硬件过流保护DAC值*/
#define OverHardcurrentValue         (5.0/(HW_RSHUNT*HW_AMPGAIN)/2.0)      	//此數值不可更動
#define DAC_OvercurrentValue         (_Q8(I_ValueX(OverHardcurrentValue * 2.0)) + 0xFF)						// 過流設置與 FU6831 不同

#define Align_Theta                     _Q15((float)ALIGN_ANGLE / 180.0)

#define BASE_FREQ                       ((MOTOR_SPEED_BASE / 60.0) * Pole_Pairs)                        // 基准频率

/*保护参数值------------------------------------------------------------------*/
/* protect value */
//#define VOLT2RAWVALUE(_VOLT_)            _Q15((_VOLT_) / HW_BOARD_VOLT_MAX)



/* motor speed set value */
#define Motor_Open_Ramp_ACC             _Q15(MOTOR_OPEN_ACC     / MOTOR_SPEED_BASE)
#define Motor_Open_Ramp_Min             _Q15(MOTOR_OPEN_ACC_MIN / MOTOR_SPEED_BASE)

#define Motor_Omega_Ramp_Min            _Q15(MOTOR_OMEGA_ACC_MIN / MOTOR_SPEED_BASE)
#define Motor_Omega_Ramp_End            _Q15(MOTOR_OMEGA_ACC_END / MOTOR_SPEED_BASE)

#define Motor_Loop_Speed                _Q15(MOTOR_LOOP_RPM / MOTOR_SPEED_BASE)

#define Motor_OpenLoop_Max_Speed				_Q15(OpenLoop_Max_Speed   / MOTOR_SPEED_BASE)
#define Motor_Max_Speed                 _Q15(MOTOR_MAXSPEED   / MOTOR_SPEED_BASE)
#define Motor_Min_Speed                 _Q15(MOTOR_MINSPEED   / MOTOR_SPEED_BASE)

#define VSP_Slope                       (float)(Motor_Max_Speed - Motor_Min_Speed) / (float)((Max_VSP_Voltage) - (Min_VSP_Voltage))

/*速度穩態判斷參數值*/
#define SpeedFloating_FallRange_Value   _Q15(SpeedFloating_FallRange / MOTOR_SPEED_BASE)
#define SpeedFloating_RiseRange_Value   _Q15(SpeedFloating_RiseRange / MOTOR_SPEED_BASE)
/*電壓穩態判斷參數值*/
#define VoltFloating_Stable_TransValue  _Q15(VoltFloating_Stable_Value / HW_BOARD_VOLT_MAX)
#define VoltFloating_FailRange_Value   _Q15(VoltFloating_FailRange / HW_BOARD_VOLT_MAX)

#define Motor_Stall_Min_Speed           _Q15(MOTOR_SPEED_STAL_MIN_RPM / MOTOR_SPEED_BASE)
#define Motor_Stall_Max_Speed           _Q15(MOTOR_SPEED_STAL_MAX_RPM / MOTOR_SPEED_BASE)

#define SPEED_K                         ((float)(Motor_Max_Speed-Motor_Min_Speed)/(float)(PWM_MAXDUTY-PWM_MINDUTY))
#define SPEED_K_PWM                     ((float)(MAXPWMDutySpeed-Motor_Min_Speed)/(float)(PWM_MAXDUTY-PWM_MINDUTY))
#define SPEED_FG_01                     (float)((float)(fake_Curve_Speed02 - fake_Curve_Speed01) / (float)(fake_Curve_PWMDuty02 - fake_Curve_PWMDuty01))
#define SPEED_FG_02                     (float)((float)(fake_Curve_Speed03 - fake_Curve_Speed02) / (float)(PWM_MAXDUTY - fake_Curve_PWMDuty02))

#define SPEED_FG_01_01                  (float)((float)(fake_SPEEDCurve_Speed02 - fake_SPEEDCurve_Speed01) / (float)(_Q15(3380/MOTOR_SPEED_BASE) - _Q15(2290/MOTOR_SPEED_BASE)))
#define SPEED_FG_02_02                  (float)((float)(fake_SPEEDCurve_Speed03 - fake_SPEEDCurve_Speed02) / (float)(_Q15(23000/MOTOR_SPEED_BASE) - _Q15(3380/MOTOR_SPEED_BASE)))

///*曲線假FG功能設置*/
//#define fake_Curve_PWMDuty_Slope01      (float)(PWMDuty01_Speed - PWMDutyLS_Speed) / (float)(PWMDuty01 - PWMDutyLS)
//#define fake_Curve_PWMDuty_Slope02      (float)(PWMDuty01_Speed - PWMDutyLS_Speed) / (float)(PWMDuty01 - PWMDutyLS)

#define POWER_K                         ((float)(Motor_Max_Power-Motor_Min_Power)/(float)(PWM_MAXDUTY-PWM_MINDUTY))
#define CURRENT_K                       ((float)(QOUTMAXVALUE-QOUTMINVALUE)/(float)(PWM_MAXDUTY-PWM_MINDUTY))

#define AC_Lose_Tailwind_Value          _Q15(AC_Lose_Tailwind_Speed / MOTOR_SPEED_BASE)

/* obsever parameter set value */
#define MAX_BEMF_VOLTAGE                ((MOTOR_SPEED_BASE*Ke)/(1000.0))
#define MAX_OMEG_RAD_SEC                ((float)(_2PI*BASE_FREQ))
#define OBS_K1T                         _Q15(LD/(LD+RS*TPWM_VALUE))
#define OBS_K2T                         _Q13((TPWM_VALUE/(LD+RS*TPWM_VALUE))*(HW_BOARD_VOLTAGE_BASE_Start/HW_BOARD_CURR_BASE))
#define OBS_K2T_SMO                     _Q13((TPWM_VALUE/(LD+RS*TPWM_VALUE))*1.4*(HW_BOARD_VOLTAGE_BASE_Start/HW_BOARD_CURR_BASE))
#define OBS_K2T_Actual                  _Q13(1.732*(TPWM_VALUE/(LD+RS*TPWM_VALUE))*(HW_BOARD_VOLTAGE_BASE/HW_BOARD_CURR_BASE))
#define OBS_K3T                         _Q15((TPWM_VALUE/(LD+RS*TPWM_VALUE))*(MAX_BEMF_VOLTAGE/HW_BOARD_CURR_BASE))
#define OBS_K4T                         _Q15(((LD-LQ)*TPWM_VALUE*MAX_OMEG_RAD_SEC)/(LD+RS*TPWM_VALUE))

#define OBSW_KP_GAIN                    _Q12(2*_2PI*ATT_COEF*ATO_BW/BASE_FREQ)
#define OBSW_KI_GAIN                    _Q15(_2PI*ATO_BW*ATO_BW*TPWM_VALUE/BASE_FREQ)

#define OBSW_KP_GAIN_RUN                _Q12(2*_2PI*ATT_COEF*ATO_BW_RUN/BASE_FREQ)
#define OBSW_KI_GAIN_RUN                _Q15(_2PI*ATO_BW_RUN*ATO_BW_RUN*TPWM_VALUE/BASE_FREQ)

#define OBSW_KP_GAIN_RUN1               _Q12(2*_2PI*ATT_COEF*ATO_BW_RUN1/BASE_FREQ)
#define OBSW_KI_GAIN_RUN1               _Q15(_2PI*ATO_BW_RUN1*ATO_BW_RUN1*TPWM_VALUE/BASE_FREQ)

#define OBSW_KP_GAIN_RUN2               _Q12(2*_2PI*ATT_COEF*ATO_BW_RUN2/BASE_FREQ)
#define OBSW_KI_GAIN_RUN2               _Q15(_2PI*ATO_BW_RUN2*ATO_BW_RUN2*TPWM_VALUE/BASE_FREQ)

#define OBSW_KP_GAIN_RUN3               _Q12(2*_2PI*ATT_COEF*ATO_BW_RUN3/BASE_FREQ)
#define OBSW_KI_GAIN_RUN3               _Q15(_2PI*ATO_BW_RUN3*ATO_BW_RUN3*TPWM_VALUE/BASE_FREQ)

#define OBSW_KP_GAIN_RUN4               _Q12(2*_2PI*ATT_COEF*ATO_BW_RUN4/BASE_FREQ)
#define OBSW_KI_GAIN_RUN4               _Q15(_2PI*ATO_BW_RUN4*ATO_BW_RUN4*TPWM_VALUE/BASE_FREQ)

#define SPEED_KLPF_WIND                 _Q15(_2PI*SPD_BW_Wind*TPWM_VALUE)           // 2PI*SPD_BW_Wind*Tpwm
#define OBSW_KP_GAIN_WIND               _Q12(2*_2PI*ATT_COEF*ATO_BW_Wind/BASE_FREQ)
#define OBSW_KI_GAIN_WIND               _Q15(_2PI*ATO_BW_Wind*ATO_BW_Wind*TPWM_VALUE/BASE_FREQ)//---PLL

#define OBSW_KP_CCW_GAIN                _Q12(2*_2PI*ATT_COEF*ATO_CCW_BW/BASE_FREQ)
#define OBSW_KI_CCW_GAIN                _Q15(_2PI*ATO_CCW_BW*ATO_CCW_BW*TPWM_VALUE/BASE_FREQ)
#define OBS_EA_KS_Wind                  _Q15((2*MOTOR_SPEED_SMOMIN_RPM_Wind*_2PI*BASE_FREQ*TPWM_VALUE)/MOTOR_SPEED_BASE)     // SMO的最小速度

#define OBS_FBASE                       _Q15(BASE_FREQ*TPWM_VALUE)                                    // Fbase*Tpwm*32768
#define OBS_KLPF                        _Q15(_2PI*BASE_FREQ*TPWM_VALUE)                               // 2PI*Fbase*Tpwm
#define SPEED_KLPF                      _Q15(_2PI*SPD_BW*TPWM_VALUE)                                  // 2PI*SPD_BW*Tpwm
#define TailWind_SPEED_KLPF             _Q15(_2PI*TailWind_SPD_BW*TPWM_VALUE)                         // 2PI*TailWind_SPD_BW*Tpwm

#define OBS_EA_KS                       _Q15((2*MOTOR_SPEED_SMOMIN_RPM*_2PI*BASE_FREQ*TPWM_VALUE)/MOTOR_SPEED_BASE)     // SMO的最小速度


/**********************晶片設置選項*****************************
*1. 電機工作電壓設定
  *2. 晶片設置參數
	*3. 電機參數
	*4. 硬體電路參數
	*└ a. 母線電壓配阻
	**************************************************************/ 


 /*******************電機運行設置選項*****************************
	*1. 正反轉設置
  *2. 預定位設置
	*└ a. 預定位模式設置 
	*3. 電機啟動設置
	*├ a. 啟動模式設置
	*├ b. 啟動參數設置 
	*└ c. ATO 參數設置
	*4. 電機運行設置
	*├ a. 運行電流設置
	*└ b. 運行 DQ軸輸出設置
	*5. 電流環參數設置
	*6. 外環控制致能
	*├ a. 電流環控制設置
	*└ b. 外環控制模式設置
	*  ├ A. 速度環設置
	*	 | └ i. 可變 PID 控制致能選項
	*  └ B. 功率環設置
  ***************************************************************/

/*預定位設置 --進階*/
 

/*電機啟動設置--進階*/
 
    
/*電機運行設置--進階*/	
#if (Pole_Pairs < 4)	
 #define MOTOR_SPEED_BASE               (MOTOR_MAXSPEED * 2.0)             // 速度環速度基準值 [典型值: 2倍最高轉速] 單位: RPM
#elif (Pole_Pairs >= 4)
 #define MOTOR_SPEED_BASE               (MOTOR_MAXSPEED * 1.8)             // 速度環速度基準值 [典型值: 2倍最高轉速] 單位: RPM
#endif 

 
 /************************功能選項********************************
  *1. 電流採樣模式設置
	*2. 調速設置
	*├ a. PMW調速模式設置
	*└ b. 無調速模式設置
  *2.	順逆風設置
	*3. 正反轉設置
	*4. FG 功能設置
  ***************************************************************/
	



#define noneMode_Speed            			_Q15(noneModeSpeed_RPM / MOTOR_SPEED_BASE)

#define MAXPWMDutySpeed									_Q15(MaxCurveSpeed/ MOTOR_SPEED_BASE)


 /************************測試用選項********************************
  *1. 預定位測試選項
  *2. 啟動測試選項
	*3. 使用煞車
  *4. 五段式運行選
	*5. SMO 估算補償角選項
	*6. 弱磁選項項
	*7. IPM 測試
	*8. PWM 非工作 DUTY 區間關機致能 
	*9. 過調適選項
  ***************************************************************/

#if (HPFCOL_Enable == 1)
 #undef  ROTATESIGNAL_TYPE
 #undef  MOTOR_BRAKE_MODE
 #undef  MOTOR_STARTDELAY_Enable
 #undef  MOTOR_STARTDELAY_TIME
 #define RDSignal_DelayTIM                     (13000)
 #define ROTATESIGNAL_TYPE                     (RD_TYPE)
 #define MOTOR_BRAKE_MODE                      (ALWAYSBRAKE)
 #define MOTOR_STARTDELAY_Enable               (0) 
 #define MOTOR_STARTDELAY_TIME                 (4000)
#endif
  
  
  
#define UART_BUFFER_SIZE                      (16)
  
  
  
/*使用煞車 --進階*/
 #define Motor_Stop_Speed									_Q15(MOTOR_SPEED_STOP_RPM / MOTOR_SPEED_BASE)								// 煞車轉速數值轉換
 
  
/*PWM Duty 轉速*/
 #define PWMDutyLS												_Q15(0.0)
 #define PWMDutyHS												_Q15(1.0)

 
#if (PWM_INVERT_Enable == 1)
  #define PWM_OFState             (1)
#else
  #define PWM_OFState             (0)
#endif
 
/*ADC 參考電壓選擇*/
 #if (HW_ADC_REF_Volt   == REF_5V0)
  #define HW_ADC_REF                     (5.0)
 #elif (HW_ADC_REF_Volt == REF_4V5)
  #define HW_ADC_REF                     (4.5)
 #elif (HW_ADC_REF_Volt == REF_4V0)
  #define HW_ADC_REF                     (4.0)
 #elif (HW_ADC_REF_Volt == REF_3V0)
  #define HW_ADC_REF                     (3.0)
 #endif
 
 /*母線分壓比率設定*/
 #if (HW_RV_MODE == RVEXT)
   #define RV                           ((RV1 + RV2) / RV2)       			  // 外部分壓比率
 #elif (HW_RV_MODE == RV6P5)
   #define RV                           (6.5)       													  // 內部 6.5 倍分壓比率  
 #elif (HW_RV_MODE == RV12)
   #define RV                           (12.0)       													  // 內部 12 倍分壓比率
 #else
   #error "RV mode setting serror."
 #endif
 
#endif