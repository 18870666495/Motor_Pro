/**
  ******************************************************************************
  * @file           : Develop.h
  * @copyright      : (c) 2022, Fortior Tech
  * @brief          : This file contains customer parameter used for Motor Control.
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
#ifndef __DEVELOP_H
#define __DEVELOP_H

/* 標頭檔 (Includes) ---------------------------------------------------------*/

/* 外部變數 (Exported types) -------------------------------------------------*/

/* 外部常數 (Exported constants) ---------------------------------------------*/
/* ADC參考電壓 */
#define REF_5V0                               (0)                                     /*!< ADC 5.0V 參考電壓 */
#define REF_4V5                               (1)                                     /*!< ADC 4.5V 參考電壓 */
#define REF_4V0                               (2)                                     /*!< ADC 4.0V 參考電壓 */
#define REF_3V0                               (3)                                     /*!< ADC 3.0V 參考電壓 */

/* SVPWM 模式*/
#define SVPWM_5_Segment                       (0)                                     /*!< 五段式 SVPWM @note 單電阻採樣不得設置該項目 */
#define SVPWM_7_Segment                       (1)                                     /*!< 七段式 SVPWM */

/* 採樣週期 */
#define DouRes_1_Cycle                        (0)                                     /*!< 1 週期採樣 (ia、ib) @note 雙、三電阻採樣有效 */
#define DouRes_2_Cycle                        (1)                                     /*!< 2 週期採樣 (交替採樣ia、ib) @note 雙、三電阻採樣有效 */

/* 電壓採集模式 */
#define RVEXT                                 (0)                                     /*!< 外部分壓 */
#define RV6P5                                 (1)                                     /*!< 內部分壓 (1 / 6.5) */ 
#define RV12                                  (2)                                     /*!< 內部分壓 (1 / 12) */

#define GainExt                               (0)                                     /*!< 使用外部放大 @note 使用本設定時，外部AMP0不可連接回授電阻*/
#define Gain_2                                (2)                                     /*!< 使用內部 2倍放大*/
#define Gain_4                                (4)                                     /*!< 使用內部 4倍放大*/
#define Gain_8                                (8)                                     /*!< 使用內部 8倍放大*/
#define Gain_16                               (16)                                    /*!< 使用內部 16倍放大*/
#define Gain_32                               (32)                                    /*!< 使用內部 32倍放大*/


/* 旋向 */
#define CW                                    (0)                                     /*!< 正轉 */
#define CCW                                   (1)                                     /*!< 反轉 */

/* 預定位模式 */
#define UD_Align                              (0)                                     /*!< UD 預定位 */
#define UQ_Align                              (1)                                     /*!< UQ 預定位 */

/* 啟動模式 */
#define Open_Start                            (0)                                     /*!< 強拖啟動 */
#define Omega_Start                           (1)                                     /*!< Omega 啟動 */
#define Open_Omega_Start                      (2)                                     /*!< 混合啟動 [先強拖後 Omega 啟動] */

/* 電流採樣模式 */
#define Single_Resistor                       (0)                                     /*!< 單電阻電流採樣模式 */
#define Double_Resistor                       (1)                                     /*!< 雙電阻電流採樣模式 */
#define Three_Resistor                        (2)                                     /*!< 三電阻電流採樣模式 */

/* 調速模式 */
#define PWMMODE                               (0)                                     /*!< PWM 調速模式 */
#define NONEMODE                              (1)                                     /*!< 無調速模式 */
#define VSPMODE                               (2)                                     /*!< VSP 調速模式*/

/* PWM 調速表來源 */
#define PMW_Curve_Function                    (0)                                     /*!< 陣列模式 */
#define PMW_Calculation_Function              (1)                                     /*!< 線性內插模式 */
#define PWM_ADJPOINT_MODE                     (2)                                     /*!< 自定義可調節點模式 */

/* ROTATESIGNAL_TYPE 來源*/
#define NO_TYPE                               (0)                                     /*!< 無輸出類型 */
#define FG_TYPE                               (1)                                     /*!< FG 類型 */
#define RD_TYPE                               (2)                                     /*!< RD 類型 */

/* 過電流檢測來源 */
#define Compare_DAC                           (0)                                     /*!< 內部 DAC 來源 */
#define Compare_Hardware                      (1)                                     /*!< ADC 來源檢測 */

/* 控制環路模式 */
#define POWER_LOOP_CONTROL                    (0)                                     /*!< 功率環 @warning 暫無功能，不可調用 */
#define SPEED_LOOP_CONTROL                    (1)                                     /*!< 速度環 */

/* 電機制動功能 */
#define NONEBRAKE                             (0)                                     /*!< 無制動 */
#define ALWAYSBRAKE                           (1)                                     /*!< 總是制動 */
#define ONLYSTARTBRAKE                        (2)                                     /*!< 僅上電時制動 */
#define ONLYSTOPBRAKE                         (3)                                     /*!< 僅停止時制動 */

#define USED_AD1                              (1)                                     /*!< 使用 ADC1 */
#define USED_AD3                              (3)                                     /*!< 使用 ADC3 */
#define USED_AD13                             (13)                                    /*!< 使用 ADC13 */


#define MASTER                                (0)                                     /*!< 主機項目定義 */
#define SLAVE                                 (1)                                     /*!< 從機項目定義 */

#define LOGIC_RDA                             (1)                                     /*!< 邏輯準位 RDA */
#define HREQUENCY_RDA                         (2)                                     /*!< 頻率信號 RDA */



/* 外部變數 (Exported macro) -------------------------------------------------*/

/* 私有巨集 (Private macro) --------------------------------------------------*/

/* 外部函數 (Exported functions) ---------------------------------------------*/


#endif /* __DEVELOP_H */