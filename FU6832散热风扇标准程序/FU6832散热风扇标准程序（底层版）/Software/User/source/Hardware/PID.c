/* --------------------------- (C) COPYRIGHT 2020 Fortiortech ShenZhen -----------------------------
    File Name      : PID.c
    Author         : Fortiortech  Appliction Team
    Version        : V1.0
    Date           : 2020-04-11
    Description    : This file contains PI initial function used for Motor Control.
----------------------------------------------------------------------------------------------------  
                                       All Rights Reserved
------------------------------------------------------------------------------------------------- */
/* Includes -------------------------------------------------------------------------------------*/
#include "FU68xx_4.h"
#include "MyProject.h"
/*
void PI_Init(void)
{
    PI0_KP  = SKP;
    PI0_KI  = SKI;
    PI0_UKH = 0;
    PI0_UKL = 0;
    PI0_EK  = 0;
    PI0_EK1 = 0;
    PI0_UKMAX = SOUTMAX;
    PI0_UKMIN = SOUTMIN;
    SetBit(PI_CR , PI0STA);          // Start PI
    while (ReadBit(PI_CR , PIBSY));
}
*/
/*
void PI_Init(void)
{
    PI1_KP  = SKP;
    PI1_KI  = SKI;
    PI1_UKH = 0;
    PI1_UKL = 0;
    PI1_EK  = 0;
    PI1_EK1 = 0;
    PI1_UKMAX = SOUTMAX;
    PI1_UKMIN = SOUTMIN;
    SetBit(PI_CR , PI1STA);          // Start PI
    while (ReadBit(PI_CR , PIBSY));
}
*/
/*
void PI_Init(void)
{
    PI2_KP  = SKP;
    PI2_KI  = SKI;
    PI2_UKH = 0;
    PI2_UKL = 0;
    PI2_EK  = 0;
    PI2_EK1 = 0;
    PI2_KD = 0x05;
    PI2_EK2 = 0;
    PI2_UKMAX = SOUTMAX;
    PI2_UKMIN = SOUTMIN;
    SetBit(PI_CR , PI2STA);          // Start PI
    while (ReadBit(PI_CR , PIBSY));
}
*/
void PI_Init(void)
{
  #if (OUTLOOP_Enable == 1)
  {
    
    PI3_KP  = SPEED_KP;		//SKP;
    PI3_KI  = SPEED_KI;		//SKI;
    PI3_UKH = 0;
    PI3_UKL = 0;
    PI3_EK  = 0;
    PI3_EK1 = 0;
//    PI3_KD = SPEED_KD;
    PI3_EK2 = 0;
    PI3_UKMAX = SOUTMAX;
    PI3_UKMIN = SOUTMIN;
  }
  #elif (OUTLOOP_Enable == 0)
  {
    PI3_KP  = DQKP;
    PI3_KI  = DQKI;
  }
  #endif  
  SetBit(PI_CR , PI3STA);
  while (ReadBit(PI_CR , PIBSY));
}