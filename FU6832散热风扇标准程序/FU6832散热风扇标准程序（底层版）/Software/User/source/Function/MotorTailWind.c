/**
  ******************************************************************************
  * @file           : MotorTailWind.h
  * @copyright      : (c) 2022, Fortior Tech
  * @brief          : 
  * @date           : 2023-02-01
  * @version        : 1.0.1
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

/* Includes ------------------------------------------------------------------*/
#include <FU68xx_4.h>

/* Private includes ----------------------------------------------------------*/
#include "MyProject.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct   
{
  int8   BemfSetFR;
  int8   BemfValue;
  uint8  BemfNum; 
  int8   BemfTabA, BemfTabB, RefNumZ, RefNumY, RefNumX;    
  uint8  BemfFR;
  uint8  Calcnum;
  uint16 Calcnms;
} Stk_TypeDef;

/* Private define ------------------------------------------------------------*/
//定义使用BEMF启动时ATO_BW值
#define ATO_BW_BEMF_START               (400.0)

#define OBSW_KP_GAIN_BEMF_START         _Q12(2 * _2PI * ATT_COEF * ATO_BW_BEMF_START / BASE_FREQ)
#define OBSW_KI_GAIN_BEMF_START         _Q12(_2PI * ATO_BW_BEMF_START * ATO_BW_BEMF_START * TPWM_VALUE / BASE_FREQ)

//定义使用BEMF启动时DKI QKI值
#define DKI_BEMF_START                  _Q12(1.0)
#define QKI_BEMF_START                  _Q12(1.0)

#define BEMFMotorStartSpeed              _Q15(TailWind_StartSpeed / MOTOR_SPEED_BASE)
#define BEMFHeadWind_EndSpeed            _Q15(2300.0 / MOTOR_SPEED_BASE)
#define BEMFMaxTailWindSpeed             _Q15((MOTOR_MAXSPEED * 1.0) / MOTOR_SPEED_BASE)


#define TIM2_Fre                        (187500.0) // TIM2计数频率187.5KHz
#define BEMFMotorStartSpeed             _Q15(TailWind_StartSpeed / MOTOR_SPEED_BASE)
#define BEMFMotorStartSpeedHigh         _Q15(windHighSpeed_Threshold / MOTOR_SPEED_BASE)
#define TempBEMFSpeedBase               (int32)(32767.0 / 8.0 * (TIM2_Fre * 60 / Pole_Pairs / MOTOR_SPEED_BASE))
#define TempBEMFSpeedBase1              (int32)(32767.0 / 6.0 / 8.0 * (TIM2_Fre * 60 / Pole_Pairs / MOTOR_SPEED_BASE))

#define NormalTailWind                  (0)

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
BEMFDetect_TypeDef   xdata BEMFDetect;
MotorTailWindTypeDef xdata TailWindDetect;
Stk_TypeDef          xdata Stk;
int16 FOC__THECOMP_Temp = 0;
uint8 BEMF2SECTION[8] = {0,1,3,2,5,6,4,7};
uint8 buf[6] = {0};

/* Private function prototypes -----------------------------------------------*/
void CMP_BMEF_Init(void);
void Time2_BMEF_Init(void);
void BEMFDealwith(void);
void TailWindDetectInit(void);
  
/* Private user code ---------------------------------------------------------*/
void tailWindVarible_Initial(void)
{
  memset(&TailWindDetect, 0, sizeof(MotorTailWindTypeDef));
  memset(&Stk,            0, sizeof(Stk_TypeDef));
  Stk.BemfFR  =0xFF;
  Stk.Calcnum =10;
}


void BEMFDetectInit(void)
{
  MOE = 0;
  BEMFDetect.BEMFSpeed           = 0;
  BEMFDetect.BEMFSpeedBase       = 0;
  BEMFDetect.BEMFStatus          = 0;
  BEMFDetect.FRStatus            = 0xFF;
  BEMFDetect.BEMFTimeCount       = BEMF_START_DETECT_TIME;

  BEMFDetect.BEMFSpeedInitStatus = 0;
  BEMFDetect.FlagSpeedCal        = 0;
  BEMFDetect.BEMFStartStatus     = 0;

  BEMFDetect.BEMFBrakeFlag       = 0;
  BEMFDetect.BEMFCCWFlag         = 0;
  BEMFDetect.BEMF_Function_Flag  = 0;

  Time2_BMEF_Init();

  CMP_BMEF_Init();
}


/* -------------------------------------------------------------------------------------------------
    Function Name  : CMP_BMEF_Init
    Description    : BMF对应Time2的初始化
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void CMP_BMEF_Init(void)
{
    /*-------------------------------------------------------------------------------------------------
    CMP Input Pin Mode
    0: GPIO Mode, P1.4--CMP0_IN+, P1.6--CMP1_IN+, P2.1--CMP2_IN+
                  P1.5--CMP0_IN-, P1.7--CMP1_IN-, P2.2--CMP2_IN-
    1: BEMF Mode, 比较器正端连接到内部星型连接电阻U、V、W的BMEF采样点，
                  比较器负端连接到内部星型连接电阻的虚拟中性点
                  比较器负端与P1.5/P1.7/P2.2断开，这三个GPIO可做其他用途
    -------------------------------------------------------------------------------------------------*/
    SetBit(P1_AN , P14 | P16);  //CMP0/1 Pin设置为模拟模式  +
    SetBit(P2_AN , P21);        //CMP2 Pin设置为模拟模式  +
    ClrBit(P1_PU , P14);        //P14上拉关闭
    /*-------------------------------------------------------------------------------------------------
       CMP0_MOD：
       00：  无内置虚拟中心点电阻的BEMF模式
       01：  内置虚拟中心点电阻的BEMF模式
       10：  3差分比较器模式
       11：  2比较器模式
    -------------------------------------------------------------------------------------------------*/
    SetReg(CMP_CR2 , CMP0MOD0 | CMP0MOD1 , CMP0MOD0);
    /*-------------------------------------------------------------------------------------------------
      比较器输出选择配置，与CMP0_MOD配合使用
      CMP0_SEL[1:0]=00，比较器0工作在3比较器轮询模式，正端在CMP0P、CMP1P、CMP2P之间自动轮流选择，
                      负端固定接内置BEMF电阻的中心点，其输出结果分别送至CMP0_OUT、CMP1_OUT、CMP2_OUT
      CMP0_SEL[1:0]=01，比较器0选择CMP0对应的端口组合，正端接CMP0P，负端接内置BEMF电阻的中心点，输出接CMP0_OUT
      CMP0_SEL[1:0]=10，比较器0选择CMP1对应的端口组合，正端接CMP1P，负端接内置BEMF电阻的中心点，输出接CMP1_OUT
      CMP0_SEL[1:0]=11，比较器0选择CMP2对应的端口组合，正端接CMP2P，负端接内置BEMF电阻的中心点，输出接CMP2_OUT
    -----------------------------------------------------------------------------*/
    SetReg(CMP_CR2 , CMP0SEL0 | CMP0SEL1 , 0x00);

    /*-------------------------------------------------------------------------------------------------
        比较器迟滞电压选择
        000: 无迟滞   001: ±2.5mV   010: -5mV   011: +5mV
        100: +-5mV   101: -10mV   110: +10mV   111: +-10mV
    -------------------------------------------------------------------------------------------------*/
    SetReg(CMP_CR1 , CMP0HYS0 | CMP0HYS1 | CMP0HYS2 , CMP0HYS0 | CMP0HYS1 | CMP0HYS2);
    /*-------------------------------------------------------------------------------------------------
        CMP0的轮询时间设置
    -------------------------------------------------------------------------------------------------*/
    SetReg(CMP_CR2 , CMP0CSEL1 | CMP0CSEL0 , 0x00);

    /*-------------------------------------------------------------------------------------------------
        比较器中断模式配置
        00: 不产生中断  01: 上升沿产生中断  10: 下降沿产生中断  11: 上升/下降沿产生中断
    -------------------------------------------------------------------------------------------------*/
    SetReg(CMP_CR0 , CMP2IM0 | CMP2IM1 , CMP2IM0 | CMP2IM1);
    SetReg(CMP_CR0 , CMP1IM0 | CMP1IM1 , CMP1IM0 | CMP1IM1);
    SetReg(CMP_CR0 , CMP0IM0 | CMP0IM1 , CMP0IM0 | CMP0IM1);

    ClrBit(CMP_CR4 , CMP0_FS);//CMP1/2功能转移	仅CMP0_MOD=01时有效
    
    SetBit(CMP_CR2 , CMP0EN); //开三个比较器
}



void Time2_BMEF_Init(void)
{
    /*-------------------------------------------------------------------------------------------------
    先停止计数，配置完寄存器后，最后启动计数
    -------------------------------------------------------------------------------------------------*/
    ClrBit(TIM2_CR1 , T2CEN);// 0，停止计数；1,使能计数

    /*-------------------------------------------------------------------------------------------------
    时钟分频设置(T2PSC)
    000:cpuclk(24MHz)         001:cpuclk/2^1(12MHz)   010:cpuclk/2^2(6MHz)    011:cpuclk/2^3(3MHz)
    100:cpuclk/2^4(1.5MHz)    101:cpuclk/2^5(750KHz)  110:cpuclk/2^6(375KHz)  111:cpuclk/2^7(187.5KHz)
    -------------------------------------------------------------------------------------------------*/
    SetReg(TIM2_CR0 , T2PSC0 | T2PSC1 | T2PSC2 , T2PSC0 | T2PSC1 | T2PSC2);
    /*-------------------------------------------------------------------------------------------------
    /模式选择
    T2MODE1，T2MODE0
    00--输入Timer模式；01--输出模式
    10--输入Count模式；11--QEP或者ISD模式
    -------------------------------------------------------------------------------------------------*/
    SetReg(TIM2_CR0 , T2MOD0 | T2MOD1, T2MOD0);
    /*-------------------------------------------------------------------------------------------------
    清除中断标志位
    禁止PWM周期检测中断使能
    -------------------------------------------------------------------------------------------------*/
    ClrBit(TIM2_CR0 , T2CES);                               // 清零脉冲计数器不使能
    ClrBit(TIM2_CR1 , T2IR | T2IF | T2IP);                  // 清零中断标志位

    /*-------------------------------------------------------------------------------------------------
    配置周期值、比较值、计数值
    禁止PWM周期检测中断使能
    使能计数器上溢中断使能
    -------------------------------------------------------------------------------------------------*/
    TIM2__ARR  = 60000;                                    // TIM2 Period = 0.32s
    TIM2__DR   = TIM2__ARR;
    TIM2__CNTR = 0;
    /*-----------启动计数------------------------------------------------*/
    SetBit(TIM2_CR1 , T2CEN);                                //启动计数
}


void BEMFDealwith(void)
{
  if((BEMFDetect.BEMFTimeCount >= 0) && (BEMFDetect.BEMFTimeCount < (BEMF_START_DETECT_TIME - BEMF_START_DELAY_TIME)))
  {
    BEMFDetect.BEMF_Function_Time_Out_Flag = 1;
    
    if(BEMFDetect.FRStatus == mcFRState.TargetFR)
    {
      BEMFDetect.BEMF_Function_Flag = 1;
      if((BEMFDetect.BEMFSpeed > BEMFMotorStartSpeed) && (BEMFDetect.BEMFSpeed < BEMFMotorStartSpeedHigh))
      {
        #if (AC_Lose_Function_Enable == 1)
        {
          if (DELL_coastMode_slowWolkState() == 1)
          {
            if(BEMFDetect.BEMFSpeed <= mcSpeedRamp.TargetValue)
              BEMFDetect.BEMFStartStatus = 1;
          }
          else
            BEMFDetect.BEMFStartStatus = 1;
        }
        #else
        {
          BEMFDetect.BEMFStartStatus = 1;
        }
        #endif
        
        BEMFDetect.BEMFStartDelayStatus = 1;
      }
      else if(BEMFDetect.BEMFSpeed >= BEMFMotorStartSpeedHigh)
      {  
        BEMFDetect.BEMFTimeCount = BEMF_START_DETECT_TIME;
      }
      else
      {
        mcState             = mcAlign;
        Align_Calib.Counter = TailWindAlign_Time;
//						  ClrBit(CMP_CR0 , CMP2IM1 | CMP2IM0 | CMP1IM1 | CMP1IM0 | CMP0IM1 | CMP0IM0);
//							ClrBit(CMP_CR2 , CMP0EN);
      }
    }
    if((BEMFDetect.FRStatus != mcFRState.TargetFR) &&(BEMFDetect.FRStatus != 0x7F) && (BEMFDetect.FRStatus != 0xFF))//反转，刹车
    {
      #if (Inverse_Current_Function == 1)
      {
        if(BEMFDetect.BEMF_Function_Flag != 2)
        {
          BEMFDetect.BEMF_Function_Flag = 2;
          BEMFDetect.BEMFMotorStartLowSpeedFilter = 0;//判断2s后转速是否大于2500RPM
        }

        if(BEMFDetect.BEMFSpeed > BEMFMaxTailWindSpeed)//顺风转速过大则等待2S后再次判断      
        {
          McStaSet.SetFlag.TailWindSetFlag = 0;
          mcFocCtrl.State_Count = 2000;          
        }
        else
        {
          if(BEMFDetect.BEMFSpeed > _Q15(3500.0 / MOTOR_SPEED_BASE))
          {
            if(TailWindDetect.TailWindBrakeFlag == 0)
            {
              TailWindDetect.TailWindBrakeFlag = 1;
              ClrBit(CMP_CR0 , CMP2IM1 | CMP2IM0 | CMP1IM1 | CMP1IM0 | CMP0IM1 | CMP0IM0);
              ClrBit(CMP_CR2 , CMP0EN);
              McStaSet.SetFlag.TailWindSetFlag = 0;//刹车完后重新开始检测
              mcFocCtrl.State_Count = 600;//刹车，主要是为了防止冲电压。
              
              MOE = 0;
              DRV_DR   = DRV_ARR + 1;
              DRV_CMR &= 0xFFC0;
              DRV_CMR |= 0x015;
              ClrBit(DRV_CR, OCS);
              MOE = 1;
            }
          }
          else
          {
            if(BEMFDetect.BEMFMotorStartLowSpeedFilter >= 2000)//2s后转速依然小于2500RPM，说明是低速逆风，刹车预定位启动
            {
              BEMFDetect.BEMFMotorStartLowSpeedFilter = 0;
              
              mcFocCtrl.State_Count = 500;
              
              MOE = 0;
              DRV_DR   = DRV_ARR + 1;
              DRV_CMR &= 0xFFC0;
              DRV_CMR |= 0x015;
              ClrBit(DRV_CR, OCS);
              MOE = 1;    

              BEMFDetect.BEMFBrakeFlag = 1;//刹车预定位启动    
              BEMFDetect.BEMFCCWFlag = 1;
            }
          }
          
          if((TailWindDetect.TailWindBrakeFlag == 1) && (mcFocCtrl.State_Count == 0))
          {
            if(BEMFDetect.BEMFSpeed > _Q15(3500.0 / MOTOR_SPEED_BASE))
            {
              if(TailWindDetect.TailWindDetectInitFlag == 0)
              {
                TailWindDetect.TailWindDetectInitFlag = 1;
                ClrBit(CMP_CR0 , CMP2IM1 | CMP2IM0 | CMP1IM1 | CMP1IM0 | CMP0IM1 | CMP0IM0);
                ClrBit(CMP_CR2 , CMP0EN); 
                TailWindDetectInit();
              }
            }
            if(TailWindDetect.TailWindDetectInitFlag == 1)
            {
              SMO_TailWindDealwith();
            }
          }
        }
      }
      #elif (Inverse_Current_Function == 0)
      {
        BEMFDetect.BEMF_Function_Flag = 2;
        McStaSet.SetFlag.TailWindSetFlag = 0;

        MOE = 0;
        DRV_DR   = DRV_ARR + 1;
        DRV_CMR  = 0x00;
        DRV_CMR |= 0x015;                         // 三相下桥臂通，刹车

        ClrBit(DRV_CR , OCS);//OCS = 0, DRV_COMR;OCS = 1, FOC/SVPWM/SPWM
        MOE = 1;

        ClrBit(CMP_CR0 , CMP2IM1 | CMP2IM0 | CMP1IM1 | CMP1IM0 | CMP0IM1 | CMP0IM0);
        ClrBit(CMP_CR2 , CMP0EN);

        BEMFDetect.BEMFBrakeFlag =  1;
      
        if(BEMFDetect.BEMFSpeed > _Q15(10000.0 / MOTOR_SPEED_BASE))
        {
            mcFocCtrl.State_Count  = 3000;
            BEMFDetect.BEMFCCWFlag = 1;
        }
        else
        {
            mcFocCtrl.State_Count = 1500;
            if(BEMFDetect.BEMFCCWFlag == 0) 
              BEMFDetect.BEMFCCWFlag = 2;
        }
      }
      #endif
    }
    if((BEMFDetect.FRStatus == 0x7F || BEMFDetect.FRStatus == 0xFF) && (TailWindDetect.TailWindBrakeFlag == 0))
    {
      ClrBit(CMP_CR0 , CMP2IM1 | CMP2IM0 | CMP1IM1 | CMP1IM0 | CMP0IM1 | CMP0IM0);
      ClrBit(CMP_CR2 , CMP0EN);
      mcState             = mcAlign;
      Align_Calib.Counter = ALIGN_TIME;
    }
  }
  if((BEMFDetect.BEMF_Function_Count >= BEMF_START_DETECT_TIME+200)&&(BEMFDetect.BEMF_Function_Flag != 2))
  {
    BEMFDetect.BEMFStartDelayStatus = 0;
    BEMFDetect.BEMF_Function_Flag = 0;
    BEMFDetect.BEMF_Function_Count = 0;
    mcState = mcStop;
  }
  if(BEMFDetect.BEMF_Function_Time_Out_Count > 4500)
  {
    BEMFDetect.BEMF_Function_Time_Out_Count = 0;
    BEMFDetect.BEMFStartDelayStatus = 0;
    BEMFDetect.BEMF_Function_Flag = 0;
    BEMFDetect.BEMF_Function_Count = 0;
    mcState = mcStop;
  }

}


uint8 GetBEMFStatus(void)
{
  uint8 BEMFStatus = 0;
  if(ReadBit(CMP_SR , CMP2OUT)) 
    BEMFStatus += 4;
  if(ReadBit(CMP_SR , CMP1OUT)) 
    BEMFStatus += 2;
  if(ReadBit(CMP_SR , CMP0OUT)) 
    BEMFStatus += 1;
  return BEMFStatus;
}

/* -------------------------------------------------------------------------------------------------
    Function Name  : BEMFSpeedDetect
    Description    : 检测速度的计时
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void BEMFSpeedDetect(void)
{
    if(BEMFDetect.BEMFSpeedInitStatus == 0)
    {
        BEMFDetect.BEMFSpeedInitStatus =1;

        BEMFDetect.PeriodTime = 0;
        BEMFDetect.MC_StepTime[0] = 0;
        BEMFDetect.MC_StepTime[1] = 0;
        BEMFDetect.MC_StepTime[2] = 0;
        BEMFDetect.MC_StepTime[3] = 0;
        BEMFDetect.MC_StepTime[4] = 0;
        BEMFDetect.MC_StepTime[5] = 0;
        BEMFDetect.BEMFStep = 0;
        BEMFDetect.StepTime = 0;
        BEMFDetect.FirstCycle = 0;
    }
    else
    {
        BEMFDetect.StepTime = TIM2__CNTR;
        TIM2__CNTR = 0;

        BEMFDetect.MC_StepTime[BEMFDetect.BEMFStep] = BEMFDetect.StepTime;

        BEMFDetect.PeriodTime = (BEMFDetect.MC_StepTime[0] + BEMFDetect.MC_StepTime[1] + BEMFDetect.MC_StepTime[2]
                                +BEMFDetect.MC_StepTime[3] + BEMFDetect.MC_StepTime[4] + BEMFDetect.MC_StepTime[5]) >> 3;

        BEMFDetect.BEMFStep++;

        if(BEMFDetect.FirstCycle)//360度，第一圈是360计算一次速度，第二圈是60度计算一次速度
        {
            BEMFDetect.FlagSpeedCal  = 1;
            BEMFDetect.BEMFSpeedBase = TempBEMFSpeedBase;
        }
        else//60度
        {
            BEMFDetect.FlagSpeedCal  = 1;
            BEMFDetect.BEMFSpeedBase = TempBEMFSpeedBase1;
            BEMFDetect.PeriodTime    = BEMFDetect.StepTime;
        }

        if(BEMFDetect.BEMFStep == 6)
        {
            BEMFDetect.FirstCycle = 1;
            BEMFDetect.BEMFStep   = 0;
        }
    }
}

/* -------------------------------------------------------------------------------------------------
    Function Name  : BEMFSpeedCal
    Description    : 速度计算，得到的是Q格式的数据
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void BEMFSpeedCal(void)
{
  if(BEMFDetect.FlagSpeedCal)//此处注意XDATA和除数只能是16位的问题
  {
    DivQ_L_MDU(BEMFDetect.BEMFSpeedBase >> 16, BEMFDetect.BEMFSpeedBase, BEMFDetect.PeriodTime, BEMFDetect.BEMFSpeed);
    BEMFDetect.FlagSpeedCal = 0;
  }
}

/* -------------------------------------------------------------------------------------------------
    Function Name  : BEMFDetectFunc
    Description    : BEMF检测，判断方向，速度，以及顺风切闭环
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void BEMFDetectFunc(void)
{
  /* -----检测顺风或逆风----- */
  BEMFDetect.FRStatus = Drv_SectionCheak();
  /* -----速度检测----- */
  BEMFSpeedDetect();
  /* -----速度计算----- */
  BEMFSpeedCal();

  //强制启动标志使能时
  if(BEMFDetect.BEMFStartStatus)
  {
      //CW时U相BEMF上升沿启动，CCW时V相BEMF上升沿启动
      if(((mcFRState.TargetFR == CW) && (BEMFDetect.BEMFStatus == 4))
        ||((mcFRState.TargetFR == CCW) && (BEMFDetect.BEMFStatus == 4)))
      {
          /* -----执行直接闭环启动程序----- */
          BEMFFOCCloseLoopStart();
          ClrBit(CMP_CR0 , CMP2IM1 | CMP2IM0 | CMP1IM1 | CMP1IM0 | CMP0IM1 | CMP0IM0);
          ClrBit(CMP_CR2 , CMP0EN);

          BEMFDetect.BEMFStartStatus = 0;
      }
  }
}

//uint16 TailWind_Speed = 0;
uint8 brakefinishflag = 0;
/* -------------------------------------------------------------------------------------------------
    Function Name  : BEMFDealwith
    Description    : BEMF处理方式
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------------------------------
    Function Name  : BEMFFOCCloseLoopStart
    Description    : 闭环启动
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
void BEMFFOCCloseLoopStart(void)
{
  #if (MOTOR_STARTDELAY_Enable == 1)
  {
    Motor_DelayStart_CNT = 0;
  }
  #endif
  
  
  FOC_Init();
//	mcFaultDect.StallSpeedAndEsCnt = 0;
  BEMFDetect.BEMFBrakeStepFlag = 1;
  
  /*启动电流、KP、KI*/
  FOC_IDREF = ID_Start_CURRENT;                // D轴启动电流 
  
  FOC_DQKP = TailWindDQKP;
  FOC_DQKI = TailWindDQKI;

  FOC_EFREQACC  = Motor_Omega_Ramp_ACC;
  FOC_EFREQMIN  = MOTOR_OMEGA_ACC_MIN;
  FOC_EFREQHOLD = MOTOR_OMEGA_ACC_END;

  SetBit(FOC_CR1,EFAE);                                 // 估算器强制输出
  ClrBit(FOC_CR1,RFAE);                                 // 禁止强拉
  SetBit(FOC_CR1,ANGM);                                 // 估算模式

  FOC__EOME = BEMFDetect.BEMFSpeed;	
  if( BEMFDetect.BEMFSpeed >= _Q15(0.25))
  {
    FOC_EKP           = OBSW_KP_GAIN_RUN4;
    FOC_EKI           = OBSW_KI_GAIN_RUN4;
    FOC__UQ =  CloseLoopStart_UQ_Value_H;
    FOC_IQREF = IQ_CloseLoopStart_CURRENT_H;
  }
  else if(( BEMFDetect.BEMFSpeed >= _Q15(0.1))&&( BEMFDetect.BEMFSpeed < _Q15(0.25)))
  {
    FOC_EKP           = OBSW_KP_GAIN_RUN2;
    FOC_EKI           = OBSW_KI_GAIN_RUN2;
    FOC__UQ =  CloseLoopStart_UQ_Value_M;
    FOC_IQREF = IQ_CloseLoopStart_CURRENT_M;
  }
  else
  {
    FOC_EKP           = OBSW_KP_GAIN_RUN2;
    FOC_EKI           = OBSW_KI_GAIN_RUN2;		
    FOC__UQ =  CloseLoopStart_UQ_Value_L;
    FOC_IQREF = IQ_CloseLoopStart_CURRENT_L; 
  }
  PI3_KP  = TailWind_Speed_Kp;		//SKP;
  PI3_KI  = TailWind_Speed_Ki;		//SKI;
  mcFocCtrl.State_Count = 200;	
  FOC_OMEKLPF = TailWind_SPEED_KLPF;
	mcFocCtrl.StartStaFlag = 1;
  mcFocCtrl.CtrlMode    = 0;				
                
  DRV_CMR |= 0x3F;
  MOE = 1;
}

/* -------------------------------------------------------------------------------------------------
    Function Name  : Drv_SectionCheak
    Description    : 
    Date           : 2020-04-10
    Parameter      : None
------------------------------------------------------------------------------------------------- */
uint8 Drv_SectionCheak(void)
{
	static uint8 i=0;
	int8 Value;
	
	BEMFDetect.BEMFStatus=GetBEMFStatus(); 
	Stk.BemfValue = BEMFDetect.BEMFStatus;
	
    buf[i++]=Stk.BemfValue;
	if(i >= 6)
	{
		i=0;
	}
	
    if((Stk.BemfValue <= 6)&&(Stk.BemfValue > 0))
	{
		Stk.BemfTabA = BEMF2SECTION[Stk.BemfValue];

		Value = Stk.BemfTabA - Stk.BemfTabB;
		if(Value>=5) 
		{
			Value = Value-6;
		}
		else if(Value<=-5)
		{
			Value = Value+6;
		}
			
		Stk.BemfTabB = Stk.BemfTabA;
		
//----------------------------------------------------
		if(Value == 1)
		{
			Stk.RefNumY++;      
			Stk.RefNumX = 0; 

		}
		else if(Value == -1)
		{
			Stk.RefNumY--;      
			Stk.RefNumX = 0; 
		}
		else
		{
			Stk.RefNumX++;      
		}

		if(Stk.RefNumY > Stk.Calcnum)
		{
			Stk.RefNumY = Stk.Calcnum;
		}else if(Stk.RefNumY <- Stk.Calcnum)
		{
			Stk.RefNumY = -Stk.Calcnum;
		}
//--------------------------------------------------------
		if(Stk.RefNumY >= Stk.Calcnum)
		{ 
			Stk.BemfFR = CW;
			Stk.BemfTabA +=1; 
			if(Stk.BemfTabA > 6)
			{
				Stk.BemfTabA -=6; 
			}
		}else if(Stk.RefNumY <= -Stk.Calcnum)
		{
			Stk.BemfFR = CCW;
		}else  
		{
			Stk.BemfFR = 0xFF; 
		}

		if(Stk.RefNumX >= 50)
		{
			Stk.BemfFR = 0X7F;
		}
	}
    return  Stk.BemfFR;
}


void TailWindDetectInit(void)
{
    TailWindDetect.MotorOmegaStartFlag    = 0;
    TailWindDetect.TailWindStartFlag      = 0;
		TailWindDetect.TailWindFocIqChangeDelay = 0;
    TailWindDetect.MotorTailWindState     = NormalTailWind;     //  初始状态为正常的顺逆风状态
    FOC_Init();                            // FOC的初始化
    FOC_DQKP    = DQKP_TailWind;           // 顺逆风的电流环KP
    FOC_DQKI    = DQKI_TailWind;           // 顺逆风的电流环KI
    FOC_EKP     = OBSW_KP_GAIN_WIND;       // 顺逆风速度估算的KP
    FOC_EKI     = OBSW_KI_GAIN_WIND;       // 顺逆风速度估算的KI
    FOC_OMEKLPF = SPEED_KLPF_WIND;         // 顺逆风下的速度滤波系数
    SetBit(FOC_CR1 , ANGM);                // 估算模式
    DRV_CMR |= 0x3F;                       // U、V、W相输出
    MOE = 1;                               // 打开MOE
    FOC_IDREF = 0;                         // D轴给定电流
    FOC_IQREF = 0;
}


void TailWindSpeedDetect(void)
{
    static int16 LatestTheta;
    if(mcState == mcTailWind)
    {
        //顺风检测过程由于FOC内部根据电流估算角度，故直接对FOC_ETHETA进行处理
        if(TailWindDetect.MotorTailWindState == NormalTailWind)
        {
            LatestTheta = FOC__ETHETA;

            //旋转方向判断在 <-170度   <10 >-10  >170度三个状态切换的时间
            if(LatestTheta < -30946)
            {
                //计数器未清零或从状态3跳到状态1时清零
                if((TailWindDetect.SpeedTimerClearStatus == 0) || (TailWindDetect.AngleState == 3))
                {
                    TailWindDetect.SpeedCountTimer       = 0;
                    TailWindDetect.SpeedTimerClearStatus = 1;
                    if(TailWindDetect.AngleState == 3) TailWindDetect.ShakeTimes ++;//来回1和3之间抖动，抖动次数加1
                }
                //<-170度  时设置状态为1，并清零SpeedCountTimer在TIM5中计时
                TailWindDetect.AngleState = 1;
            }
            else if((LatestTheta>-1820)&&(LatestTheta<1820)) //<10 >-10
            {
                //状态1或状态3切换到状态2时保存当前转速时间至TailWindDetect.SpeedCount[SpeedStoreNum]
                if((TailWindDetect.AngleState == 1) || (TailWindDetect.AngleState == 3))
                {
                    //计算当前转速，RPM
                    TailWindDetect.SpeedCountTimer += 1;//防止为0
                    TailWindDetect.SpeedStoreNum ++;

                    //用于SpeedCountTimer清零
                    if(TailWindDetect.SpeedTimerClearStatus == 1) TailWindDetect.SpeedTimerClearStatus = 0;

                    //有1状态切换到2状态说明角度递增旋转方向为CW，3->2则为CCW
                    if(TailWindDetect.AngleState == 1)      TailWindDetect.MotorDir = CW;
                    else if(TailWindDetect.AngleState == 3) TailWindDetect.MotorDir = CCW;
                    TailWindDetect.ShakeTimes = 0;//清除来回抖动的计数
                }
                TailWindDetect.AngleState = 2;

            }
            //>170度时
            else if(LatestTheta > 30946)
            {
                //计数器未清零或从状态1跳到状态3时清零
                if((TailWindDetect.SpeedTimerClearStatus == 0) || (TailWindDetect.AngleState == 1))
                {
                    TailWindDetect.SpeedCountTimer       = 0;
                    TailWindDetect.SpeedTimerClearStatus = 1;
                    if(TailWindDetect.AngleState == 1) TailWindDetect.ShakeTimes ++;//来回1和3之间抖动
                }
                TailWindDetect.AngleState = 3;
            }
        }
    }
}

void SMO_TailWindDealwith(void)
{
    if((TailWindDetect.MotorDir == CCW) && (TailWindDetect.SpeedStoreNum > 2))
    {        
        TailWindDetect.TailWindBrakeFlag = 0;
				BEMFDetect.BEMF_Inverse_Current_Flag = 1;

        
        FOC_DQKP              = DQKP;
        FOC_DQKI              = DQKI;

        FOC_EKP               = OBSW_KP_CCW_GAIN;
        FOC_EKI               = OBSW_KI_CCW_GAIN;
        mcFocCtrl.State_Count = 0;

        FOC_IDREF             = ID_TailWind_CCW_CURRENT;        
        mcFocCtrl.mcIqref     = IQ_TailWind_CCW_CURRENT1;       // Q轴启动电流
        FOC_IQREF             = mcFocCtrl.mcIqref;
			
//        FOC_EKLPFMIN = OBS_EA_KS_Wind;
        FOC__THECOMP 	      = _Q15(0.0/180.0);//在正电流反拉时FOC角度补偿值给90度
        FOC__THECOMP_Temp = _Q15(0.0/180.0);

        TailWindDetect.TailWindStartFlag     = 1;
        mcFocCtrl.StartStaFlag = 1;
        mcFocCtrl.CtrlMode    = 0;
    }
}
