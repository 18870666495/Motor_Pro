/**
  ******************************************************************************
  * @file           : SignalOutputControl.h
  * @copyright      : (c) 2023, Fortior Tech
  * @brief          : SignalOutputControl.c �Ę��^�ļ�
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

/* ���x�Է�ֹ�fޒ����  Define to prevent recursive inclusion -----------------*/
#ifndef __SIGNALOUTPUTCONTROL_H
#define __SIGNALOUTPUTCONTROL_H

/* ���^�n      Includes ------------------------------------------------------*/
/* �������̈́e  Exported types ------------------------------------------------*/
/* �����ĳ���  Exported constants --------------------------------------------*/
/* �����ľ޼�  Exported macro ------------------------------------------------*/
/* �����ĺ�ʽ  Exported functions ------------------------------------------- */
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