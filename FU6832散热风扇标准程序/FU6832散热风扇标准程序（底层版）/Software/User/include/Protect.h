/**
  ******************************************************************************
  * @file           : Protect.h
  * @copyright      : (c) 2022, Fortior Tech
  * @brief          : This file contains customer parameter used for protection motor.
  * @date           : 2023-01-30
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PROTECT_H
#define __PROTECT_H

/* 標頭檔 (Includes) ---------------------------------------------------------*/

/* 外部變數 (Exported types) -------------------------------------------------*/

/* 外部常數 (Exported constants) ---------------------------------------------*/
/* 硬體過流來源 */
#define Compare_Mode                          (Compare_DAC)                           /*!< @see Compare_DAC 內部 DAC 比較來源 @see Compare_Hardware 外部硬體比較來源 */
 
/* 軟體過電流檢測 */
#define OverSoftCurrentValue                  I_Value(10.0)                           /*!< (單位: A) 軟體過流閾值 */
#define CurrentRecover_Enable                 (1)                                     /*!< 軟體過流回復致能  @param (0) 禁用 @param (1) 啟用 */
#define OverCurrentRecoverTime                (2500)                                  /*!< (單位: mS) 軟體過流回復時間*/

/* 過欠電壓保護 */
#define VoltageProtect_Enable                 (1)                                     /*!< 過欠壓保護致能  @param (0) 禁用 @param (1) 啟用 */
#define Over_Protect_Voltage                  (16.0)                                  /*!< (單位: V) 過電壓保護觸發閾值 [範圍: ( @ref Over_Recover_Voltage + 0.5 ) ~ 30.0] */
#define Over_Recover_Voltage                  (15.0)                                  /*!< (單位: V) 過電壓保護恢復閾值 [範圍: ( @ref Under_Recover_Voltage + 2.0 ) ~ ( @ref Over_Protect_Voltage - 0.5)] */
#define Under_Recover_Voltage                 (8.0)                                   /*!< (單位: V) 欠電壓保護恢復閾值 [範圍: ( @ref Under_Protect_Voltage + 0.5 ) ~ ( @ref Over_Recover_Voltage - 2.0)] */
#define Under_Protect_Voltage                 (7.0)                                   /*!< (單位: V) 欠電壓保護觸發閾值 [範圍: ( 6.0 ~ ( @ref Under_Recover_Voltage - 0.5 )] */
#define OverVlotageRecoverTime                (150)                                   /*!< (單位: mS) 過欠電壓回復時間 */

/* 欠相保護 */
#define PhaseLossProtect_Enable               (1)                                     /*!< 欠相保護致能  @param (0) 禁用 @param (1) 啟用 */
#define PhaseLossCurrentValue                 I_Value(0.2)                            /*!< (單位: A) 欠相電流觸發閾值 [範圍: 0.1 ~ HW_BOARD_CURR_MAX] */
#define PhaseLossRecoverTime                  (3000)                                  /*!< (單位:mS) 欠相保護恢復時間*/

/* 啟動保護 */
#define StartProtect_Enable                   (0)                                     /*!< 啟動保護致能  @param (0) 禁用 @param (1) 啟用 */
#define StartProtectRestartValue              (5)                                     /*!< (單位: 次) 啟動保護重啟次數[為0時永遠嘗試重啟] */
#define StartProtectRecoverTimes              (3000)                                  /*!< (單位:mS) 啟動保護恢復時間*/
 
/* 堵轉保護 */
#define StallProtect_Enable                   (1)                                     /*!< 堵轉保護致能  @param (0) 禁用 @param (1) 啟用 */
#define MOTOR_SPEED_STAL_MAX_RPM              (MOTOR_MAXSPEED * 1.5)                  /*!< (單位: RPM) 堵轉估算異常最高閥值 */
#define MOTOR_SPEED_STAL_MIN_RPM              (500)                                   /*!< (單位: RPM) 堵轉估算異常最低閥值 */
#define StallRecoverTime                      (6000)                                  /*!< (單位:mS) 堵轉恢復時間*/
#define Stall_Diff_RecoverTime_En             (0)                                     /*!< 保留功能 */
#define StallSensitivity                      (5)                                     /*!< 堵轉靈敏度 [建議參數:15~25] */
#define Stall_retryStart_Enable               (0)                                     /*!< 立即重啟致能  @param (0) 禁用 @param (1) 啟用 */
#define Stall_retryStart_Count                (2)                                     /*!< 立即重啟次數 [範圍: 1 ~ 255] */

/* 外部變數 (Exported macro) -------------------------------------------------*/

/* 私有巨集 (Private macro) --------------------------------------------------*/

/* 外部函數 (Exported functions) ---------------------------------------------*/


#endif /* __PROTECT_H */