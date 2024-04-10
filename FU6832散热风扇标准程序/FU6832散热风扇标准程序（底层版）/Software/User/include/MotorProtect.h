/**
  ******************************************************************************
  * @file           : MotorProtect.h
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MOTORPROTECT_H
#define __MOTORPROTECT_H

/* Includes ------------------------------------------------------------------*/
#include "AddFunction.h"

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  NoFault            = 0x0000,    
  Hard_OverCurrent   = 0x0001,     // 硬體過電流保護
  Soft_OverCurrent   = 0x0002,     // 軟體過電流保護
  UnderVoltage       = 0x0004,     // 欠電壓保護
  OverVoltage        = 0x0008,     // 過電壓保護
  PhaseLoss          = 0x0010,     // 缺相保護
  SPD_Es_compMethod  = 0x0020,     // 堵轉保護 --估算(轉速-反電動勢)比值演算法
  UQOverFlow_Method  = 0x0060,     // 堵轉保護 --演算法
  StartTimOUT_Method = 0x0040,     // 堵轉保護 --演算法
  OUTOfRange_Method  = 0x0080      // 堵轉保護 --演算法
  
}motorFault_Type;
 
typedef enum
{
    FaultNoSource      = 0,                                                    /*!< 無錯誤 */
    FaultHardOVCurrent = 1,                                                    /*!< 硬體過電流 */
    FaultSoftOVCurrent = 2,                                                    /*!< 軟體過電流 */
    FaultUnderVoltage  = 3,                                                    /*!< 欠電壓 */
    FaultOverVoltage   = 4,                                                    /*!< 過電壓 */
    FaultLossPhase     = 5,                                                    /*!< 欠相 */
    FaultStall         = 6,                                                    /*!< 堵轉 */
} FaultStateType;

typedef union
{
  uint16 Source;
  struct
  {
    uint16 Hard_OverCurrent :1;  // 硬體過電流保護旗標
    uint16 Soft_OverCurrent :1;  // 軟體過電流保護旗標
    uint16 UnderVoltage     :1;  // 欠電壓保護旗標
    uint16 OverVoltage      :1;  // 過電壓保護旗標
    uint16 PhaseLoss        :1;  // 缺相保護旗標
    uint16 Stall            :3;  // 堵轉保護旗標
    uint16 CurrentOffset    :1;  // 電流矯正異常旗標
    uint16 Reserve          :7;  // 預留
  }isTrigger;
}motorFault_FlagDef;


/**
  * @brief  電機保護變數  @bug 該結構尚未優化
  */
typedef struct
{
    uint16 LossPHTimes;                                                        /*!< 欠相計數器 */
}ProtectVarible;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
                                                 
/* Exported Varibles --------------------------------------------------------*/
//extern FaultStateType xdata mcFaultSource;
extern motorFault_FlagDef xdata mcFaultSource;
extern ProtectVarible idata mcProtectTime;

/* Exported functions --------------------------------------------------------*/ 
extern void faultVarible_Initial(void);
extern void FaultProcess(void);
extern void Fault_softOverCurrent(CurrentVarible *);

#endif /* __MOTORPROTECT_H */


