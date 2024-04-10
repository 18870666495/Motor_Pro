/* --------------------------- (C) COPYRIGHT 2020 Fortiortech ShenZhen -----------------------------
    File Name      : DMA.c
    Author         : Fortiortech  Appliction Team
    Version        : V1.0
    Date           : 2020-04-11
    Description    : This file contains .C file function used for Motor Control.
----------------------------------------------------------------------------------------------------  
                                       All Rights Reserved
------------------------------------------------------------------------------------------------- */
#include "FU68xx_4.h"
#include "MyProject.h"

#define Switch_DMA(a)     SetBit(*(&DMA0_CR0 + a), DMAEN | DMABSY)

void SetPipe_DMA0(eType_DMA_PIPE ePipe)
{
    bool bTmp = false;

    if (ReadBit(DMA0_CR0 , DMAEN))
    {
        bTmp = true;
        while (ReadBit(DMA0_CR0 , DMABSY));
        ClrBit(DMA0_CR0 , DMAEN);
    }

    SetReg(DMA0_CR0 , DMACFG2 | DMACFG1 | DMACFG0 , ePipe);

    if (bTmp) SetBit(DMA0_CR0 , DMAEN);
}

void SetPipe_DMA1(eType_DMA_PIPE ePipe)
{
    bool bTmp = false;

    if (ReadBit(DMA1_CR0 , DMAEN))
    {
        bTmp = true;
        while (ReadBit(DMA1_CR0 , DMABSY));
        ClrBit(DMA1_CR0 , DMAEN);
    }

    SetReg(DMA1_CR0 , DMACFG2 | DMACFG1 | DMACFG0 , ePipe);

    if (bTmp) SetBit(DMA1_CR0 , DMAEN);
}

void SetDataPackage_DMA0(unsigned short ulAddr , char cLen)
{
    bool bTmp = false;

    if (ReadBit(DMA0_CR0 , DMAEN))
    {
        bTmp = true;

        while (ReadBit(DMA0_CR0 , DMABSY));
        ClrBit(DMA0_CR0 , DMAEN);
    }

    DMA0_LEN =  cLen;
    DMA0_BA  =  ulAddr;

    if (bTmp) SetBit(DMA0_CR0, DMAEN);
}

void SetDataPackage_DMA1(unsigned short ulAddr, char cLen)
{
    bool bTmp = false;

    if (ReadBit(DMA1_CR0 , DMAEN))
    {
        bTmp = true;

        while (ReadBit(DMA1_CR0 , DMABSY));
        ClrBit(DMA1_CR0 , DMAEN);
    }

    DMA1_LEN = cLen;
    DMA1_BA  = ulAddr;

    if (bTmp) SetBit(DMA1_CR0 , DMAEN);
}

void SetEndian_DMA(eType_DMA_Endian eEndian)
{
    bool bTmp = false, bTmp1;

    if (ReadBit(DMA0_CR0 , DMAEN))
    {
        bTmp = true;

        while (ReadBit(DMA0_CR0 , DMABSY));
        ClrBit(DMA0_CR0 , DMAEN);
    }

    if (ReadBit(DMA1_CR0 , DMAEN))
    {
        bTmp1 = true;

        while (ReadBit(DMA1_CR0 , DMABSY));
        ClrBit(DMA1_CR0 , DMAEN);
    }

    SetReg(DMA0_CR0, ENDIAN, eEndian);

    if (bTmp)  SetBit(DMA0_CR0 , DMAEN);
    if (bTmp1) SetBit(DMA1_CR0 , DMAEN);
}

void SetIRQ_DMA(ebool eIRQ, eType_DMA_IRQ eIP)
{
    SetReg(DMA0_CR0, DMAIE, eIRQ ? DMAIE : 0);

    if (!EA) EA = 1;

    if (eIRQ) SetReg(IP1 , PDMA1 | PDMA0 , eIP);
}

void SetDbgMod_DMA(void)
{
    bool bTmp = false;

    if (ReadBit(DMA1_CR0 , DMAEN))
    {
        bTmp = true;

        while (ReadBit(DMA1_CR0 , DMABSY));
        ClrBit(DMA1_CR0 , DMAEN);
    }

    SetBit(DMA1_CR0 , DBGEN);
    SetBit(DMA1_CR0 , ((eType_DMA_PIPE)DRAM_SPI));
    DMA1_LEN = 4;

    if (bTmp) SetBit(DMA1_CR0, DMAEN);
}

/**
 * 设置DMA1的DBG模式
 *
 * @param Addr   要DBG输出的地址
 */
void Set_DBG_DMA(uint16 DMAAddr)
{
    ClrBit(DMA1_CR0, DMAEN);                                // 禁止DMA1传输
    SetBit(DMA1_CR0, DBGEN | DRAM_SPI);                     // 设置DMA1使用DBG模式，XDATA发送数据到SPI
    if (!ReadBit(DMAAddr, 0x4000)) SetBit(DMA1_CR0, DBGSW); //根据DMAAddr判断是否在XRAM区，如果是则开启DBGSW

    DMA1_LEN = 7;                                           // 设置DMA1发送数量为8
    DMA1_BA = DMAAddr & 0x07ff;                             // 设置DMA1发送首地址

    Switch_DMA(1);                                       // 启动DMA1
}


void SetDbgData_DMA(unsigned short ulAddr)
{
    bool bTmp = false;

    if (ReadBit(DMA1_CR0, DMAEN))
    {
        bTmp = true;

        while (ReadBit(DMA1_CR0, DMABSY));
        ClrBit(DMA1_CR0, DMAEN);
    }

    if (ulAddr <= 0xfff) ulAddr += 0x70000000;
    DMA1_BA = ulAddr;

    if (bTmp) SetBit(DMA1_CR0, DMAEN);
}

void EnableRun_DMA0(void)
{
    SetBit(DMA0_CR0 , DMAEN | DMABSY);
}

void EnableRun_DMA1(void)
{
    SetBit(DMA1_CR0 , DMAEN | DMABSY);
}

bool GetStatus_DMA0(eType_DMA_Statu eStatu)
{
    return ReadBit(DMA0_CR0 , eStatu);
}

bool GetStatus_DMA1(eType_DMA_Statu eStatu)
{
    return ReadBit(DMA1_CR0 , eStatu);
}

