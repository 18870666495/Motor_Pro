/**
  ******************************************************************************
  * @file           : ACLostFunc.h
  * @copyright      : (c) 2023, Fortior Tech
  * @brief          : ACLostFunc.c 的標頭文件
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

/* 定義以防止遞迴引入  Define to prevent recursive inclusion -----------------*/
#ifndef __ACLOSTFUNC_H
#define __ACLOSTFUNC_H

/* 標頭檔      Includes ------------------------------------------------------*/
/* 導出的型別  Exported types ------------------------------------------------*/
/* 導出的常數  Exported constants --------------------------------------------*/
/* 導出的巨集  Exported macro ------------------------------------------------*/
/* 導出的函式  Exported functions ------------------------------------------- */
extern void DELL_coastMode_Initial(void);
extern void DELL_coastMode_Process(void);
extern bool DELL_coastMode_State(void);
extern void DELL_coastMode_IF_Event(void);
extern void DELL_coastMode_IC_Event(uint16);
extern bool DELL_coastMode_slowWolkState(void);


#endif /* __ACLOSTFUNC_H */















