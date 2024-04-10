/**
  ******************************************************************************
  * @file           : MotorProtect.c
  * @copyright      : (c) 2022, Fortior Tech
  * @brief          : 
  * @date           : 2023-01-31
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
  uint16 segment;                                                             /*!< 保護偵測輪詢計數器 */
  
  uint16 VoltDetecCnt;                                                        /*!< 過欠電壓檢測計數器 */
  uint16 VoltRecoverCnt;                                                      /*!< 過欠電壓恢復計數器 */
  uint16 CurrentRecoverCnt;                                                   /*!< 過電流恢復計數器 */

  uint16 StallDelayCnt;                                                       /*!< 堵轉延遲計數器 */
  uint16 StallDectSpeed;                                                      /*!< 堵轉失速計數器 */
  uint16 StallSpeedAndEsCnt;                                                  /*!< 堵轉轉速與反電動勢失衡計數器 */
  uint16 FOCUQ_Stall_Count;                                                   /*!< 堵轉 FOC_UQ溢位計數器 */
  uint16 StartFocmode;                                                        /*!< 堵轉 長時間在mode0計數器 */
  uint16 StallReCount;                                                        /*!< 堵轉保護恢復計數器 */

  uint16 Lphasecnt;                                                           /*!< 欠相保護計數器 */
  uint16 AOpencnt ;                                                           /*!< A相欠相計數器 */
  uint16 BOpencnt ;                                                           /*!< B相欠相計數器 */ 
  uint16 COpencnt ;                                                           /*!< C相欠相計數器 */
  uint16 mcLossPHRecCount;                                                    /*!< 欠相恢復計數器 */

}FaultVarible;


/* Private define ------------------------------------------------------------*/
#define Stall_ATOEndTime                      (uint16)((1400) + 10)           /*!< 堵轉檢測用 ATO 逾時時間 */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//FaultStateType       mcFaultSource;
motorFault_FlagDef xdata mcFaultSource;
FaultVarible       idata mcFaultDect;
ProtectVarible     idata mcProtectTime;

uint16 Stall_Diff_RecoverTime = 0;
uint16 Stall_CCW_State_Count  = 0;

#if (Stall_retryStart_Enable == 1)
 uint8 Stall_retryCounter   = 0;
 uint8 STall_retryIdle_Flag = 0;
#endif

/* Private function prototypes -----------------------------------------------*/
void Fault_phaseLoss(void);
void Fault_OverUnderVoltage(void);
void Fault_Detection(void);
void Fault_Stall(void);
void FaultProcess(void);
void Fault_softOverCurrent(CurrentVarible *);
void Fault_OverCurrentRecover(void);

/* Private user code ---------------------------------------------------------*/
/**
  * @brief         保護功能相關變數初始化
  * @detail        保護相關變數初始化 @note 本函數調用時務必確認相關變數已完成堆疊或不再使用。
  * @retval        無
  */
void faultVarible_Initial(void)
{
  // 函數執行開始
  memset(&mcFaultDect,   0, sizeof(FaultVarible));
  memset(&mcProtectTime, 0, sizeof(ProtectVarible)); 
  // 函數執行結束
}


/**
  * @brief         異常檢測輪詢函數
  * @detail        異常檢測使用輪詢方式進行檢測，僅能在 Systick中斷內調用。
  * @retval        無
  */
void Fault_Detection(void)
{
  // 函數執行開始
  mcFaultDect.segment++;
  if(mcFaultDect.segment >= 3)
  {
    mcFaultDect.segment = 0;
  }
 
  if(mcFaultDect.segment == 0)
  {
    #if (VoltageProtect_Enable == 1)
    {
      Fault_OverUnderVoltage();
    }
    #endif
  }
  else if(mcFaultDect.segment == 1)
  {
    #if (StallProtect_Enable == 1)
    {
      Fault_Stall();
    }
    #endif
  }
  else if(mcFaultDect.segment == 2)
  {
    #if (PhaseLossProtect_Enable == 1)
    {
      Fault_phaseLoss();
    }
    #endif
  }
  
  if(mcState == mcFault)
  {
    switch(mcFaultSource.Source)
    {
      case Hard_OverCurrent:
      case Soft_OverCurrent:
      {
        mcFaultDect.CurrentRecoverCnt++;
        if(mcFaultDect.CurrentRecoverCnt >= OverCurrentRecoverTime)
        {
          BEMFDetect.BEMFBrakeFlag = 0;
          Open_TailWind_Flag       = 0;                      // 未知處理階段，該變數已在 FaultProcess() 函數中清除。
          Open_TailWind_Count      = 0;                      // 未知處理階段，該變數已在 FaultProcess() 函數中清除。
          mcFaultSource.Source = NoFault;
          mcFaultDect.CurrentRecoverCnt  = 0;
        }
        break;
      }
      case SPD_Es_compMethod:
      case StartTimOUT_Method:
      case OUTOfRange_Method:
      {
        if (mcFaultDect.StallReCount != 0)
          mcFaultDect.StallReCount--;
        
        #if (Stall_retryStart_Enable == 1)
        {
          if (Stall_retryCounter < Stall_retryStart_Count && STall_retryIdle_Flag == 0)
          {
            Stall_retryCounter++;
            mcFaultDect.StallReCount = 0;
          }
          else
          {
            STall_retryIdle_Flag = 1;
          }
        }
        #endif
        
        if(mcFaultDect.StallReCount == 0)
        {
          #if (Stall_retryStart_Enable == 1)
          {
            STall_retryIdle_Flag = 0;
          }
          #endif
          
          mcFaultSource.Source = NoFault;
          Open_TailWind_Flag       = 0;                      // 未知處理階段，該變數已在 FaultProcess() 函數中清除。
          Open_TailWind_Count      = 0;                      // 未知處理階段，該變數已在 FaultProcess() 函數中清除。
        }
        break;
      }
      case PhaseLoss:
      {
        if((mcProtectTime.LossPHTimes < 5) && PhaseLossRecoverTime > 0)
        {
          mcFaultDect.mcLossPHRecCount++;
          if(mcFaultDect.mcLossPHRecCount >= PhaseLossRecoverTime)
          {
            mcFaultDect.AOpencnt = 0;
            mcFaultDect.BOpencnt = 0;
            mcFaultDect.COpencnt = 0;
            mcFaultSource.Source = NoFault;
          }
        }
        else
        {
          mcFaultDect.mcLossPHRecCount = 0;
        }
        break;
      }
      case UnderVoltage:
      case OverVoltage: 
      {
        if((ADCSampling.DcBus_Flt < Volt2DcBus(Over_Recover_Voltage)) && (ADCSampling.DcBus_Flt > Volt2DcBus(Under_Recover_Voltage)))
        {
          mcFaultDect.VoltRecoverCnt++;
          if(mcFaultDect.VoltRecoverCnt > OverVlotageRecoverTime)
          {
            mcFaultDect.StallSpeedAndEsCnt = 0;              // 未知處理階段，該變數清除與否，不足以影響電機運行。
            mcFocCtrl.State_Count          = 100;
            Open_TailWind_Flag             = 0;              // 未知處理階段，該變數已在 FaultProcess() 函數中清除。
            Open_TailWind_Count            = 0;              // 未知處理階段，該變數已在 FaultProcess() 函數中清除。
            mcFaultSource.Source = NoFault;
            mcFaultDect.VoltRecoverCnt     = 0;
          }
        }
        break;
      }
    } 
  }
  
  #if (Stall_retryStart_Enable == 1)
  {
    if (mcFocCtrl.Millis > 1000)
    {
      STall_retryIdle_Flag = 1;
    }
    
    if (STall_retryIdle_Flag == 1 && Stall_retryCounter != 0)
    {
      Stall_retryCounter = 0;
    }
  }
  #endif
  // 函數執行結束
}


/**
  * @brief         堵轉檢測函數
  * @detail        堵轉保護檢測相關。 @note 本函數僅能在 Fault_Detection() 函數中被調用。
  * @retval        無
  */
void Fault_Stall(void)
{
  // 函數執行開始
  if(BEMFDetect.BEMF_Inverse_Current_Flag == 1)
  {
    Stall_CCW_State_Count = 400;
  }
  else
  {
    Stall_CCW_State_Count = 180;
  }
  
  if((mcState == mcRun) && (mcFocCtrl.Open_DCBus_RetryFlag == 0))
  {
    if(mcFaultDect.StallDelayCnt <= Stall_CCW_State_Count)
    {
      mcFaultDect.StallDelayCnt++;
    }
    else
    {
      /** 條件1 MODE0 時間逾時 **/
      if (mcFocCtrl.CtrlMode == 0)
      {
        mcFaultDect.StartFocmode++;
        if (mcFaultDect.StartFocmode > Stall_ATOEndTime)
        {
          #if (Stall_Diff_RecoverTime_En == 0)
          {
            mcFaultDect.StallReCount = StallRecoverTime;
          }
          #elif (Stall_Diff_RecoverTime_En == 1)
          {
            mcFaultDect.StallReCount = (StallRecoverTime + 500);
          }
          #endif
          
          mcFaultDect.StartFocmode = 0;
          mcFaultSource.Source = StartTimOUT_Method;
		  mcFocCtrl.FaultStopFlag = 0;
        }
      }
      else
      {
        mcFaultDect.StartFocmode = 0;
      }
      
      /** 條件2 FOC__UQ 溢位 **/
      if(FOC__UQ < 500)
      {
        mcFaultDect.FOCUQ_Stall_Count++;
        if(mcFaultDect.FOCUQ_Stall_Count > 20)
        {
          
          #if (Stall_Diff_RecoverTime_En == 0)
          {
            mcFaultDect.StallReCount = StallRecoverTime;
          }
          #elif (Stall_Diff_RecoverTime_En == 1)
          {
            mcFaultDect.StallReCount = (StallRecoverTime + 750);
          }
          #endif
          
          mcFaultDect.FOCUQ_Stall_Count = 0;
          mcFaultSource.Source = UQOverFlow_Method;
		  mcFocCtrl.FaultStopFlag = 0;
        }
      }
      else
      {
        mcFaultDect.FOCUQ_Stall_Count = 0;
      }
      
      /** 條件3 電機估算失速 **/
      if((mcFocCtrl.SpeedFlt < Motor_Stall_Min_Speed) || (mcFocCtrl.SpeedFlt > Motor_Stall_Max_Speed))
      {
        mcFaultDect.StallDectSpeed++;
        if(mcFaultDect.StallDectSpeed >=5)
        {
          #if (Stall_Diff_RecoverTime_En == 0)
          {
            mcFaultDect.StallReCount = StallRecoverTime;
          }
          #elif (Stall_Diff_RecoverTime_En == 1)
          {
            mcFaultDect.StallReCount = (StallRecoverTime + 248);
          }
          #endif
          
          mcFaultDect.StallDectSpeed = 0;
          mcFaultSource.Source = SPD_Es_compMethod;
		  mcFocCtrl.FaultStopFlag = 0;
        }
      }
      else if (mcFaultDect.StallDectSpeed > 0)
      {
        mcFaultDect.StallDectSpeed--;
      }
      
      /** 條件4 電機估算轉速與反電動勢異常 **/
      if(mcFocCtrl.SpeedFlt > (((uint32)mcFocCtrl.EsValue) * 3))
      {
        mcFaultDect.StallSpeedAndEsCnt++;
        if(mcFaultDect.StallSpeedAndEsCnt >= StallSensitivity)
        {
          #if (Stall_Diff_RecoverTime_En == 0)
          {
            mcFaultDect.StallReCount = StallRecoverTime;
          }
          #elif (Stall_Diff_RecoverTime_En == 1)
          {
            mcFaultDect.StallReCount = (StallRecoverTime + 0);
          }
          #endif
          
          mcFaultDect.StallSpeedAndEsCnt = 0;
          mcFaultSource.Source = OUTOfRange_Method;
		  mcFocCtrl.FaultStopFlag = 0;
        }
      }
      else if(mcFaultDect.StallSpeedAndEsCnt > 0)
      {
        mcFaultDect.StallSpeedAndEsCnt--;
      }
    }
  }
  // 函數執行結束
}


/**
  * @brief         過欠電壓檢測函數
  * @detail        過欠電壓檢測相關。 @note 本函數僅能在 Fault_Detection() 函數中被調用。
  * @retval 無
  */
void Fault_OverUnderVoltage(void)
{
  static uint8 UV_CNT = 5, OV_CNT = 5;
  // 函數執行開始
  if((mcFaultSource.Source == NoFault))
  {
    if (ADCSampling.DcBus_Flt > Volt2DcBus(Over_Protect_Voltage) || (mcState == mcReady && (ADCSampling.DcBus_Flt > Volt2DcBus(Over_Recover_Voltage))))
    {
      UV_CNT = 5;
      if (OV_CNT != 0)
        OV_CNT--;
      else
      {
        mcFaultSource.Source = OverVoltage;
		mcFocCtrl.FaultStopFlag = 0;
      }
    }
    else if (ADCSampling.DcBus_Flt < Volt2DcBus(Under_Protect_Voltage) || (mcState == mcReady && (ADCSampling.DcBus_Flt < Volt2DcBus(Under_Recover_Voltage))))
    {
      OV_CNT = 5;
      if (UV_CNT != 0)
        UV_CNT--;
      else
      {
        mcFaultSource.Source = UnderVoltage;
		mcFocCtrl.FaultStopFlag = 0;
      }
    }
    else
    {
      UV_CNT = 5;
      OV_CNT = 5;
    }
  }  
  // 函數執行結束
}


/**
  * @brief         欠相保護檢測函數
  * @detail        欠相檢測相關。 @note 本函數僅能在 Fault_Detection() 函數中被調用。
  * @retval        無
  */
void Fault_phaseLoss(void)
{
  // 函數執行開始
  if(mcState == mcRun)
  {
    mcFaultDect.Lphasecnt++;
    if(mcFaultDect.Lphasecnt > 20)
    {
      mcFaultDect.Lphasecnt = 0;
      if(((mcCurVarible.Max_ia > (mcCurVarible.Max_ib * 2)) || (mcCurVarible.Max_ia > (mcCurVarible.Max_ic * 2)))
          && (mcCurVarible.Max_ia > PhaseLossCurrentValue))
      {
        mcFaultDect.AOpencnt++;
      }
      else if(mcFaultDect.AOpencnt > 0)
      {
        mcFaultDect.AOpencnt--;
      }

      if(((mcCurVarible.Max_ib > (mcCurVarible.Max_ia * 2)) || (mcCurVarible.Max_ib > (mcCurVarible.Max_ic * 2)))
          && (mcCurVarible.Max_ib > PhaseLossCurrentValue))
      {
        mcFaultDect.BOpencnt++;
      }
      else if(mcFaultDect.BOpencnt > 0)
      {
         mcFaultDect.BOpencnt--;
      }

      if(((mcCurVarible.Max_ic > (mcCurVarible.Max_ia * 2)) || (mcCurVarible.Max_ic > (mcCurVarible.Max_ib * 2)))
          && (mcCurVarible.Max_ic > PhaseLossCurrentValue))
      {
        mcFaultDect.COpencnt++;
      }
      else if(mcFaultDect.COpencnt > 0) 
      {
        mcFaultDect.COpencnt--;
      }
      
      mcCurVarible.Max_ia = 0;
      mcCurVarible.Max_ib = 0;
      mcCurVarible.Max_ic = 0;
      SetBit(FOC_CR2, ICLR);
      if((mcFaultDect.AOpencnt > 9) || (mcFaultDect.BOpencnt > 9) || (mcFaultDect.COpencnt > 9))
      {
        mcProtectTime.LossPHTimes++;
        mcFaultSource.Source = PhaseLoss;
		mcFocCtrl.FaultStopFlag = 0;
      }
    }
  }
  // 函數執行結束
}


/**
  * @brief         軟體過電流檢測函數
  * @detail        軟體過電流檢測相關函數。 @note 本函數僅能在 DRV_ISR() 中斷函數中被調用。
  * @param[in,out] CurrentVarible *h_Cur  FOC電流採集變數
  * @retval        無
  */
void Fault_softOverCurrent(CurrentVarible *h_Cur)
{
  // 函數執行開始
  if((mcState == mcRun) || (mcState == mcStart))
  {
    h_Cur->Max_ia = FOC__IAMAX;
    h_Cur->Max_ib = FOC__IBMAX;
    h_Cur->Max_ic = FOC__ICMAX;

    mcCurVarible.Max_ia = FOC__IAMAX;
    mcCurVarible.Max_ib = FOC__IBMAX;
    mcCurVarible.Max_ic = FOC__ICMAX;

    if((FOC__IAMAX >= OverSoftCurrentValue) || (FOC__IBMAX >= OverSoftCurrentValue) || (FOC__ICMAX >= OverSoftCurrentValue))
    {
      h_Cur->OverCurCnt++;
      if(h_Cur->OverCurCnt >= 3)
      {
        mcFaultSource.Source = Soft_OverCurrent;
		mcFocCtrl.FaultStopFlag = 0;
        h_Cur->Max_ia     = 0;
        h_Cur->Max_ib     = 0;
        h_Cur->Max_ic     = 0;
        h_Cur->OverCurCnt = 0;
      }
    }
    else if (h_Cur->OverCurCnt > 0)
    {
      h_Cur->OverCurCnt--;
    }
  }
  // 函數執行結束
}


/**
  * @brief         異常處理函數
  * @detail        電機保護接段相關處理。
  * @retval        無
  */
void FaultProcess(void)
{
  // 函數執行開始
  /** 關閉電機輸出 --開始 **/
  ClrBit(DRV_CR, DRVEN);
  ClrBit(DRV_CR, FOCEN);
  MOE     = 0;
//  mcState = mcFault;
  /** 關閉電機輸出 --結束 **/

  /** 其他處理程序 --開始 **/
  #if (ROTATESIGNAL_TYPE == RD_TYPE)
  {
//    RDFault_Detection();
  }
  #endif
  
  mcFocCtrl.CtrlMode  = 0;
  Open_TailWind_Flag  = 0;
  Open_TailWind_Count = 0;
  /** 其他處理程序 --結束 **/
  // 函數執行結束
}

