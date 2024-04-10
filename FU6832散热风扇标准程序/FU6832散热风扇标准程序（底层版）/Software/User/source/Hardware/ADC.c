/* --------------------------- (C) COPYRIGHT 2020 Fortiortech ShenZhen -----------------------------
    File Name      : ADC.c
    Author         : Fortiortech  Appliction Team
    Version        : V1.1
    Date           : 2020-07-06
    Description    : This file contains .C file function used for Motor Control.
----------------------------------------------------------------------------------------------------  
                                       All Rights Reserved
------------------------------------------------------------------------------------------------- */
#include "FU68xx_4.h"
#include "MyProject.h"

void ADC_Init(void)
{
    /********************ADC 端口模拟功能设置************************/
	SetBit(P2_AN , PIN0);	//AD0 P20 CH open--IU--固定
	SetBit(P2_AN , PIN3);	//AD1 P23 CH open--IV--固定 
  SetBit(P2_AN , PIN5);	//AD3 P25 CH open
  SetBit(P2_AN , PIN7);	//AD4 P27 CH open--IBUS/IW--固定
	SetBit(P1_AN , PIN5);	//AD13 P15 CH open
//	
	#if (SPEED_MODE == VSPMODE)
  {
    SetBit(P3_AN , PIN4);
    SetBit(ADC_MASK, CH7EN);
  }
  #endif
//	set_csr(P3_AN , PIN2);	//AD5 P32 CH open
//	set_csr(P3_AN , PIN3);	//AD6 P33 CH open
//	set_csr(P3_AN , PIN4);	//AD7 P34 CH open
//	set_csr(P2_AN , PIN1);	//AD8 P21 CH open
//	set_csr(P1_AN , PIN6);	//AD9 P16 CH open
//	set_csr(P1_AN , PIN4);	//AD10 P14 CH open
//	set_csr(P2_AN , PIN6);	//AD11 P26 CH open
//	set_csr(P1_AN , PIN3);	//AD12 P13 CH open
//	set_csr(P1_AN , PIN5);	//AD13 P15 CH open
	/****************************************************************/
  SetBit(ADC_MASK , CH0EN | CH1EN | CH2EN | CH3EN | CH4EN | CH13EN | CH14EN);   //通道使能

  //#20200706新增
  #if (HW_RV_MODE == RV6P5)
  {
    SetBit(ADC_CR , ADCRATIO); 								// 設定 ADC 分壓比 6.5倍
  }
  #elif (HW_RV_MODE == RV12)
  {
    ClrBit(ADC_CR , ADCRATIO); 								// 設定 ADC 分壓比 12倍 
  }
  #elif (HW_RV_MODE == RVEXT)
  {
    SetBit(P2_AN,  PIN4);
  }
  #endif
	//#
	
  SetBit(ADC_CR , ADCALIGN); 			//ADC数据次高位对齐使能0-->Disable	1-->Enable
  ClrBit(ADC_CR , ADCIE); 				//ADC中断使能
  SetBit(ADC_CR , ADCEN);         //Enable ADC0
}

