/**
  ******************************************************************************
  * @file           : ACLostFunc.h
  * @copyright      : (c) 2023, Fortior Tech
  * @brief          : ACLostFunc.c �Ę��^�ļ�
  * @date           : 2023-06-14
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
#ifndef __ACLOSTFUNC_H
#define __ACLOSTFUNC_H

/* ���^�n      Includes ------------------------------------------------------*/
/* �������̈́e  Exported types ------------------------------------------------*/
/* �����ĳ���  Exported constants --------------------------------------------*/
/* �����ľ޼�  Exported macro ------------------------------------------------*/
/* �����ĺ�ʽ  Exported functions ------------------------------------------- */
extern void DELL_coastMode_Initial(void);
extern void DELL_coastMode_Process(void);
extern bool DELL_coastMode_State(void);
extern void DELL_coastMode_IF_Event(void);
extern void DELL_coastMode_IC_Event(uint16);
extern bool DELL_coastMode_slowWolkState(void);


#endif /* __ACLOSTFUNC_H */















