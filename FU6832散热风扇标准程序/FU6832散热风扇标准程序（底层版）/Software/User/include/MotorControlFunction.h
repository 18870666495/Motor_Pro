/* --------------------------- (C) COPYRIGHT 2020 Fortiortech ShenZhen -----------------------------
    File Name      : MotorControlFunction.h
    Author         : Fortiortech  Appliction Team
    Version        : V1.0
    Date           : 2020-04-11
    Description    : This file contains motor contorl parameter used for Motor Control.
----------------------------------------------------------------------------------------------------
                                       All Rights Reserved
------------------------------------------------------------------------------------------------- */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MOTORCONTROLFUNCTION_H_
#define __MOTORCONTROLFUNCTION_H_

/* Define to prevent recursive inclusion -------------------------------------*/
typedef struct
{
  int16   IuOffset;       //Iu的偏置电压
  int32   IuOffsetSum;    //Iu的偏置电压总和
  int16   IvOffset;       //Iv的偏置电压
  int32   IvOffsetSum;    //Iv的偏置电压总和
  int16   Iw_busOffset;   //Iw或Ibus的偏置电压
  int32   Iw_busOffsetSum;//Iw或Ibus的偏置电压总和
  int16   OffsetCount;    //偏置电压采集计数
  int8    OffsetFlag;     //偏置电压结束标志位
}CurrentOffset;


 typedef struct
 {
   int16  Align_IA;                // 預定位 IA 採集
   int16  Align_IB;                // 預定位 IB 採集
   int16  Align_IC;                // 預定位 IC 採集
   uint8  Update_CNT;              // 更新計數器
   uint16 Counter;                 // 預定位計數器
 }AlignCalib_TypeDef;
 
extern uint16 Motor_DelayStart_CNT;

 
extern CurrentOffset xdata mcCurOffset;
extern AlignCalib_TypeDef  Align_Calib;

extern void VariablesPreInit(void);
extern void GetCurrentOffset(void);
extern void Motor_Ready (void);
extern void Motor_Init (void);

extern void FOC_Init(void);
extern void Motor_Charge(void);
extern void MC_Stop(void);
extern void Motor_Open(void);
extern void Motor_Align(void);
extern void MotorcontrolInit(void);
extern void Motor_TailWind(void);


#endif