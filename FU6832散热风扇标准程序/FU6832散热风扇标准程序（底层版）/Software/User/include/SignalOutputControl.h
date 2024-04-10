/**
  ******************************************************************************
  * @file           : SignalOutputControl.h
  * @copyright      : (c) 2023, Fortior Tech
  * @brief          : SignalOutputControl.c 的標頭文件
  * @date           : 2023-12-18
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

/* 定義以防止遞迴引入  Define to prevent recursive inclusion -----------------*/
#ifndef __SIGNALOUTPUTCONTROL_H
#define __SIGNALOUTPUTCONTROL_H

/* 標頭檔      Includes ------------------------------------------------------*/
/* 導出的型別  Exported types ------------------------------------------------*/
/* 導出的常數  Exported constants --------------------------------------------*/
/* 導出的巨集  Exported macro ------------------------------------------------*/
/* 導出的函式  Exported functions ------------------------------------------- */
extern void  SignalOutput_initial(void);
extern void  SignalOutput_Counter(void);
extern uint8 SignalOutput_Process(void);
extern void  FG_setFakeSpped(uint16);

extern void FG_Update_Event(void);

extern void SignalOutput_Pause(void);
extern void SignalOutput_Continue(void);



//extern void SignalOutput_Initital(void);
//extern void FGUpdate_Process(void);
//extern void FGCounter_Event(void);
//extern void FGUpdate_Event(void);
//extern void SignalOutput_Pause(void);
//extern void SignalOutput_Continue(void);

//extern uint8 RDSignal_Process(void);
//extern void RDFault_Detection(void);

#endif /* __SIGNALOUTPUTCONTROL_H */