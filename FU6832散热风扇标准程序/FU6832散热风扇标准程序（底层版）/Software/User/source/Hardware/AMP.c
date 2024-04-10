/* --------------------------- (C) COPYRIGHT 2020 Fortiortech ShenZhen -----------------------------
    File Name      : AMP.c
    Author         : Fortiortech  Appliction Team
    Version        : V1.0
    Date           : 2020-04-11
    Description    : This file contains .C file function used for Motor Control.
----------------------------------------------------------------------------------------------------  
                                       All Rights Reserved
------------------------------------------------------------------------------------------------- */
#include "FU68xx_4.h"
#include "MyProject.h"

void AMP_Init(void)
{
	#if (Shunt_Resistor_Mode == Single_Resistor) 
  {
		/*****AMP 端口模拟功能设置******/
    SetBit(P3_AN , P31);            //AMP0 Pin设置为模拟模式  +
    SetBit(P3_AN , P30);            //AMP0 Pin设置为模拟模式  -
    SetBit(P2_AN , P27);            //AMP0 Pin设置为模拟模式  O

    //000-->Genaral AMP 001-->2X 010-->4X 011-->8X 100-->16X 101-->32X Others-->32X
    /**********AMP0 PGA SET************/
		#if (HW_AMPGAIN == 0)		 //Genaral AMP
			ClrBit(AMP0_GAIN , AMP0_GAIN2);	//0x80
			ClrBit(AMP0_GAIN , AMP0_GAIN1);	//0x40
			ClrBit(AMP0_GAIN , AMP0_GAIN0);	//0x20
		#elif (HW_AMPGAIN == Gain_2)	 //2X
			ClrBit(AMP0_GAIN , AMP0_GAIN2);	//0x80
			ClrBit(AMP0_GAIN , AMP0_GAIN1);	//0x40
			SetBit(AMP0_GAIN , AMP0_GAIN0);	//0x20
		#elif (HW_AMPGAIN == Gain_4)  //4X
			ClrBit(AMP0_GAIN , AMP0_GAIN2);	//0x80
			SetBit(AMP0_GAIN , AMP0_GAIN1);	//0x40
			ClrBit(AMP0_GAIN , AMP0_GAIN0);	//0x20
		#elif (HW_AMPGAIN == Gain_8)  //8X
			ClrBit(AMP0_GAIN , AMP0_GAIN2);	//0x80
			SetBit(AMP0_GAIN , AMP0_GAIN1);	//0x40
			SetBit(AMP0_GAIN , AMP0_GAIN0);	//0x20
		#elif (HW_AMPGAIN == Gain_16) //16X
			SetBit(AMP0_GAIN , AMP0_GAIN2);	//0x80
			ClrBit(AMP0_GAIN , AMP0_GAIN1);	//0x40
			ClrBit(AMP0_GAIN , AMP0_GAIN0);	//0x20
		#elif (HW_AMPGAIN == Gain_32) //32X
			SetBit(AMP0_GAIN , AMP0_GAIN2);	//0x80
			ClrBit(AMP0_GAIN , AMP0_GAIN1);	//0x40
			SetBit(AMP0_GAIN , AMP0_GAIN0);	//0x20
		#else
			#error "HW_AMPGAIN setting error!"
		#endif
        
    SetBit(AMP_CR , AMP0EN);        //AMP0 Enable
  }
  #elif (Shunt_Resistor_Mode == Double_Resistor)
  {
    SetBit(P1_AN , P16);            //AMP1 Pin设置为模拟模式  +
    SetBit(P1_AN , P17);            //AMP1 Pin设置为模拟模式  -
    SetBit(P2_AN , P20);            //AMP1 Pin设置为模拟模式  O

    SetBit(P2_AN , P21);            //AMP2 Pin设置为模拟模式  +
    SetBit(P2_AN , P22);            //AMP2 Pin设置为模拟模式  -
    SetBit(P2_AN , P23);            //AMP2 Pin设置为模拟模式  O
    ClrBit(P2_OE , P23);            //P23_OE需要强制为0，禁止DA1输出至PAD  
		
		SetBit(P3_AN , P31);            //AMP0 Pin设置为模拟模式  +
    SetBit(P3_AN , P30);            //AMP0 Pin设置为模拟模式  -
    SetBit(P2_AN , P27);            //AMP0 Pin设置为模拟模式  O
    
    ClrBit(AMP0_GAIN , AMP0_GAIN2);
    ClrBit(AMP0_GAIN , AMP0_GAIN1);
    ClrBit(AMP0_GAIN , AMP0_GAIN0);
    
    SetBit(AMP_CR , AMP0EN);
    SetBit(AMP_CR , AMP1EN);        //AMP1 Enable
    SetBit(AMP_CR , AMP2EN);        //AMP2 Enable
  }
  #elif (Shunt_Resistor_Mode == Three_Resistor)
  {
    SetBit(P1_AN , P16);            //AMP1 Pin设置为模拟模式  +
    SetBit(P1_AN , P17);            //AMP1 Pin设置为模拟模式  -
    SetBit(P2_AN , P20);            //AMP1 Pin设置为模拟模式  O

    SetBit(P2_AN , P21);            //AMP2 Pin设置为模拟模式  +
    SetBit(P2_AN , P22);            //AMP2 Pin设置为模拟模式  -
    SetBit(P2_AN , P23);            //AMP2 Pin设置为模拟模式  O
    ClrBit(P2_OE , P23);            //P23_OE需要强制为0，禁止DA1输出至PAD  
		
		SetBit(P3_AN , P31);            //AMP0 Pin设置为模拟模式  +
    SetBit(P3_AN , P30);            //AMP0 Pin设置为模拟模式  -
    SetBit(P2_AN , P27);            //AMP0 Pin设置为模拟模式  O
    
    ClrBit(AMP0_GAIN , AMP0_GAIN2);
    ClrBit(AMP0_GAIN , AMP0_GAIN1);
    ClrBit(AMP0_GAIN , AMP0_GAIN0);
    
    SetBit(AMP_CR , AMP0EN);
    SetBit(AMP_CR , AMP1EN);        //AMP1 Enable
    SetBit(AMP_CR , AMP2EN);        //AMP2 Enable       
  }
  #endif
}