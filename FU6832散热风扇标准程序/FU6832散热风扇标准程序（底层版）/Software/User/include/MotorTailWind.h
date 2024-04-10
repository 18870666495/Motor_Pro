/**
  ******************************************************************************
  * @file           : MotorTailWind.h
  * @copyright      : (c) 2022, Fortior Tech
  * @brief          : 
  * @date           : 2023-01-31
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MOTORTAILWIND_H
#define __MOTORTAILWIND_H
/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  uint16 BEMFSpeed;                   //反电动势检测的速度
  uint32 BEMFSpeedBase;               //反电动势检测的速度基准
  uint8  BEMFStatus;                  //反电动势的状态，6拍的状态
  uint8  FRStatus;                    //正反转
  uint16 PeriodTime;                  //转一圈的周期计数值/8,因除数只能是16位的
  uint16 MC_StepTime[6];              //转一拍的计数值数组
  uint16 StepTime;                    //单拍的计数值
  uint16 BEMFTimeCount;               //反电动势检测时间
  uint8  FirstCycle;                  //反电动势检测第一个周期
  uint8  BEMFStep;                    //拍的计数
  uint8  BEMFSpeedInitStatus;         //速度初始化状态
  uint8  FlagSpeedCal;                //速度计算标志
  uint8  BEMFStartStatus;             //强制启动标志位
  uint8  BEMFStartDelayStatus;        //强制启动标志位  
  uint8  BEMFCCWFlag;                 //反转的强弱标志
  uint8  BEMFBrakeFlag;
  uint8  BEMFBrakeStepFlag;
  uint16 BEMF_Function_Count;
  uint8  BEMF_Function_Flag;
  uint16 BEMFMotorStartLowSpeedFilter;
  uint8  BEMF_Inverse_Current_Flag;
  
  uint16 BEMF_Function_Time_Out_Count;
  uint16 BEMF_Function_Time_Out_Flag;
  
  uint16 rotateTime_Buf[6];        // 旋轉扇區
  uint8  PhasePosition;            // 相位位置
  uint8  isSpeedCalc;
  
}BEMFDetect_TypeDef;


typedef struct
{
  uint16 TailWindSpeed;               // 顺逆风速度
  uint8  MotorDir;                    // 电机方向
  uint8  AngleState;                  // 角度状态
  uint8  MotorTailWindState;          // 电机顺逆风状态
  uint8  SpeedStoreNum;               // 速度计算的次数
  uint16 SpeedCountTimer;             // 速度检测计时
  uint8  SpeedTimerClearStatus;       // 速度检测计时的清零标志位
  uint8  SpeedOverFlowStatus;         // 转速溢出标志
  uint8  ShakeTimes;                  // 1和3状态来回抖动的次数
  uint8  AntiFlag;                    // 不同转速的反转标志
  uint8  MotorOmegaStartFlag;
  uint8  TailWindStartFlag;
  uint8  TailWindDetectInitFlag;
  int16  FocIqChangeDelay;  
  uint8  TailWindCCWFlag;
  uint8  SpeedLowFlag;
  uint8  TailWindBrakeFlag;
  int16 TailWindFocIqChangeDelay;
}MotorTailWindTypeDef;

/* Exported constants --------------------------------------------------------*/
#define BEMF_START_DETECT_TIME                (150)                                   /*!< example */
#define BEMF_START_DELAY_TIME                 (120)//速度快的电机，时间可以缩短到15

/* Exported macro ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Exported Varibles ---------------------------------------------------------*/
extern BEMFDetect_TypeDef   xdata BEMFDetect;
extern MotorTailWindTypeDef xdata TailWindDetect;

/* Exported functions --------------------------------------------------------*/ 
extern void BEMFSpeedDetect(void);
extern uint8 CWCCWDetect(uint8 HallStatus);
extern uint8 GetBEMFStatus(void);
extern void BEMFDetectInit(void);
extern void BEMFDetectFunc(void);
extern void BEMFSpeedCal(void);
extern void BEMFDealwith(void);
extern void BEMFFOCCloseLoopStart(void);
extern uint8 Drv_SectionCheak(void);
extern void tailWindVarible_Initial(void);
extern void TailWindSpeedDetect(void);
extern void SMO_TailWindDealwith(void);

#endif /* __MOTORTAILWIND_H */