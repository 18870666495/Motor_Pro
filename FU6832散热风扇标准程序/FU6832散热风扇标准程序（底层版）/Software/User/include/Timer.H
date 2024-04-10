/**
  ******************************************************************************
  * @file           : TIMER.h
  * @copyright      : (c) 2022, Fortior Tech
  * @brief          : 
  * @date           : 2023-02-18
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
#ifndef __TIMER_H
#define __TIMER_H
/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/** @defgroup SYSTICK_IP_selection Systick 中斷優先級
  * @{
  */
#define SYSTICK_IP_00                         (0x00)                                  /*!< Systick 中斷優先級為 00 (最低) */
#define SYSTICK_IP_01                         (PSYSTICK0)                             /*!< Systick 中斷優先級為 01        */
#define SYSTICK_IP_10                         (PSYSTICK1)                             /*!< Systick 中斷優先級為 10        */
#define SYSTICK_IP_11                         (PSYSTICK1 | PSYSTICK0)                 /*!< Systick 中斷優先級為 11 (最高) */
/**
  * @}
  */
  

/** @defgroup TIM3_IP_selection TIM3 中斷優先級
  * @{
  */
#define TIM3_IP_00                            (0x00)                                  /*!< TIM3 中斷優先級為 00 (最低) */
#define TIM3_IP_01                            (PTIM30)                                /*!< TIM3 中斷優先級為 01        */
#define TIM3_IP_10                            (PTIM31)                                /*!< TIM3 中斷優先級為 10        */
#define TIM3_IP_11                            (PTIM31 | PTIM30)                       /*!< TIM3 中斷優先級為 11 (最高) */
/**
  * @}
  */
  
/** @defgroup TIM4_IP_selection TIM4 中斷優先級
  * @{
  */  
#define TIM4_IP_00                            (0x00)                                  /*!< TIM4 中斷優先級為 00 (最低) */
#define TIM4_IP_01                            (PTIM40)                                /*!< TIM4 中斷優先級為 01        */
#define TIM4_IP_10                            (PTIM41)                                /*!< TIM4 中斷優先級為 10        */
#define TIM4_IP_11                            (PTIM41 | PTIM40)                       /*!< TIM4 中斷優先級為 11 (最高) */
/**
  * @}
  */
  
 /** @defgroup SYSTICK_TS_Enable Systick 中斷
  * @{
  */ 
#define SYSTICK_TS_NONE                       (0x00)                                  /*!< 不使用 Systick 中斷   */
#define SYSTICK_TS_IF1F                       (SYSTIE)                                /*!< 使用Systick 中斷      */
/**
  * @}
  */
  

/** @defgroup TIM_ClockDivision TIM 時脈除頻
  * @{
  */
#define TIM_CLOCKDIVISION_DIV1                (0x00)                                  /*!< 不除頻 (24MHz)          */
#define TIM_CLOCKDIVISION_DIV2                (T4PSC0)                                /*!< 除以 2 (12MHz)          */ 
#define TIM_CLOCKDIVISION_DIV4                (T4PSC1)                                /*!< 除以 4 (6MHz)           */
#define TIM_CLOCKDIVISION_DIV8                (T4PSC1 | T4PSC0)                       /*!< 除以 8 (3MHz)           */ 
#define TIM_CLOCKDIVISION_DIV16               (T4PSC2)                                /*!< 除以 16 (1.5MHz)        */
#define TIM_CLOCKDIVISION_DIV32               (T4PSC2 | T4PSC0)                       /*!< 除以 32 (750KHz)        */ 
#define TIM_CLOCKDIVISION_DIV64               (T4PSC2 | T4PSC1)                       /*!< 除以 64 (375KHz)        */ 
#define TIM_CLOCKDIVISION_DIV128              (T4PSC2 | T4PSC1 | T4PSC0)              /*!< 除以 128 (187.5KHz)     */
#define TIM3_CLOCKMULTIPLICATION_MUL2         (T4PSC2 | T4PSC1 | T4PSC0)              /*!< TIM3 專用倍頻 2 (48MHz) */
 /**
  * @}
  */

/** @defgroup TIM_Output_Compare_N_Polarity TIM 比較輸出互補
  * @{
  */
#define TIM_OCNPOLARITY_HIGH                  (T4OCM)                                 /*!< 比較模式互補輸出極性 */
#define TIM_OCNPOLARITY_LOW                   (0x00)                                  /*!< 比較模式互補輸出極性 */
/**
  * @}
  */


/** @defgroup TIM_Trigger_selection TIM 中斷觸發
  * @{
  */
#define TIM_TS_NONE                           (0x00)                                  /*!< TIM 無中斷               */
#define TIS_TS_NOIR                           (TIM4_CR0 & 0xF7)                       /*!< TIM 匹配中斷致能         */
#define TIM_TS_TI1F                           (T4IFE)                                 /*!< TIM 溢位中斷致能         */
#define TIM_TS_TI1P                           (T4IPE)                                 /*!< TIM 比較中斷致能         */
#define TIS_TS_TI1R                           (T4IRE)                                 /*!< TIM 匹配中斷致能         */
#define TIM_TS_TI1F1P                         (T4IPE | T4IFE)                         /*!< TIM 溢位 & 比較中斷致能  */
/**
  * @}
  */
  
/** @defgroup TIM_Mode_selection TIM 模式
  * @{
  */
#define TIM_BASE_Mode                         (0x00)                                  /*!< Timer 模式           */
#define TIM_PWM_Mode                          (T4MOD)                                 /*!< PWM 輸出模式         */
#define TIM_ICN_Mode                          (T2MOD1)                                /*!< TIM2 專用計數器模式  */
#define TIM_RSD_Mode                          (T2MOD1 | T2MOD0)                       /*!< TIM2 專用 RSD 模式   */
/** 
  * @} 
  */
  
/** @defgroup TIM_Trigger_Filter_selection TIM 輸入濾波器
  * @{
  */
#define TIM_TriggerFilter_NONE                (0x00)                                  /*!< 不使用輸入濾波器     */
#define TIM_TriggerFilter_4CLK                (T4NM0)                                 /*!< 輸入濾波 4 週期      */
#define TIM_TriggerFilter_8CLK                (T4NM1)                                 /*!< 輸入濾波 8 週期      */
#define TIM_TriggerFilter_16CLK               (T4NM1 | T4NM0)                         /*!< 輸入濾波 16 週期     */
/**
  * @} 
  */

/** @defgroup TIM_CHANNEL_selection TIM 功能轉移
  * @{
  */
#define TIM3_CHANNEL_0                        (0x00)                                  /*!< TIM3 不轉移(P1.1)     */
#define TIM3_CHANNEL_1                        (T3CT)                                  /*!< TIM3 功能轉移(P0.1)   */
#define TIM4_CHANNEL_0                        (0x00)                                  /*!< TIM4 不轉移(P0.1)     */
#define TIM4_CHANNEL_1                        (T4CT)                                  /*!< TIM4 功能轉移(P0.0)   */
/** 
  * @} 
  */

/** @defgroup TIM_AUTORELOAD_selection TIM 自動重載
  * @{
  */
#define TIM_AUTORELOAD_DISABLE                (T4OPM)                                 /*!< 不自動重載(單次模式)   */
#define TIM_AUTORELOAD_ENABLE                 (0x00)                                  /*!< 使用自動重載(連續模式) */
/**
  * @} 
  */

/** @defgroup TIM_Input_Capture_Polarity TIM 輸入捕獲有效邊緣
  * @{
  */
#define TIM_INPUT_RISING                      (0x00)                                  /*!< 正邊緣觸發(高電平脈寬)   */
#define TIM_INPUT_FALLING                     (T4OCM)                                 /*!< 負邊緣觸發(低電平脈寬)   */
/**
  * @}
  */


/** @defgroup DRV_Trigger_selection Driver 中斷觸發
  * @{
  */
#define DRV_Trigger_NONE                      (0x00)                                  /*!< DRV 無中斷             */
#define DRV_Trigger_Falling                   (DCIM1)                                 /*!< DRV 正邊緣觸發         */
#define DRV_Trigger_Rising                    (DCIM0)                                 /*!< DRV 負邊緣觸發         */
#define DRV_Trigger_Rising_Falling            (DCIM1 | DCIM0)                         /*!< DRV 正負邊緣觸發       */
/** 
 * @}
 */

/** @defgroup DRV_MODE_selection DRV 運行模式
  * @{
  */
#define DRV_BLDC_MODE                         (DRVEN)                                 /*!< DRV DLDC方波模式       */
#define DRV_FOC_MODE                          (MESEL | DRVEN)                         /*!< DRV FOC模式(推薦)      */
/** @} */

/** @defgroup DRV_PRELOAD_Enable DRV 預裝載
  * @{
  */
#define DRV_EnableARRPreload                  (DRPE)                                  /*!< DRV 啟用預裝載       */
#define DRV_DisableARRPreload                 (0x00)                                  /*!< DRV 禁用預裝載       */
/** @} */

/** @defgroup MOE_MODE_selection MOE 模式
  * @{
  */
#define MOE_NOT_CLEAR                         (0x00)                                  /*!< MOE 不自動關閉         */
#define MOE_AUTO_Clear                        (MOEMD0)                                /*!< MOE 自動關閉           */
#define MOE_AUTO_Clear_Reload_10uS            (MOEMD1)                                /*!< MOE 自動關閉10uS後啟動 */
#define MOE_AUTO_Clear_Reload_5uS             (MOEMD1 | MOEMD0)                       /*!< MOE 自動關閉5uS後啟動  */
/**
 * @}
 */


/* Exported macro ------------------------------------------------------------*/
/**
  * @brief FG TIM除頻器配置
  * @param (TIM_CLOCKDIVISION_DIV1)   不除頻   (24MHz)
  * @param (TIM_CLOCKDIVISION_DIV2)   除以 2   (12MHz)
  * @param (TIM_CLOCKDIVISION_DIV4)   除以 4   (6MHz)
  * @param (TIM_CLOCKDIVISION_DIV8)   除以 8   (3MHz)
  * @param (TIM_CLOCKDIVISION_DIV16)  除以 16  (1,5MHz)
  * @param (TIM_CLOCKDIVISION_DIV32)  除以 32  (750kHz)
  * @param (TIM_CLOCKDIVISION_DIV64)  除以 64  (375kHz)
  * @param (TIM_CLOCKDIVISION_DIV128) 除以 128 (87.5kHz)
  */
#define FG_ClockBase                     (TIM_CLOCKDIVISION_DIV32)              /*!<( @ref SignalOutput_Type 為 FG_Signal 時有效)*/

#if (FG_ClockBase == TIM_CLOCKDIVISION_DIV1)
  #define FG_TIMCLOCK     (24000000.0)            // 24MHz FG time base freq.
#elif (FG_ClockBase == TIM_CLOCKDIVISION_DIV2)
  #define FG_TIMCLOCK     (12000000.0)            // 12MHz FG time base freq.
#elif (FG_ClockBase == TIM_CLOCKDIVISION_DIV4)
  #define FG_TIMCLOCK     (6000000.0)             // 6MHz FG time base freq.
#elif (FG_ClockBase == TIM_CLOCKDIVISION_DIV8)
  #define FG_TIMCLOCK     (3000000.0)             // 3MHz FG time base freq.
#elif (FG_ClockBase == TIM_CLOCKDIVISION_DIV16)
  #define FG_TIMCLOCK     (1500000.0)             // 1.5MHz FG time base freq.
#elif (FG_ClockBase == TIM_CLOCKDIVISION_DIV32)
  #define FG_TIMCLOCK     (750000.0)              // 750kKz FG time base freq.
#elif (FG_ClockBase == TIM_CLOCKDIVISION_DIV64)
  #define FG_TIMCLOCK     (375000.0)              // 375kHz FG time base freq.
#elif (FG_ClockBase == TIM_CLOCKDIVISION_DIV128)
  #define FG_TIMCLOCK     (187500.0)              // 187.5kHz FG time base freq.
#else
  #error "FG_TIM error"
#endif

/* Private macro -------------------------------------------------------------*/

/** @brief  設定 DEV 輸出
  * @param  __FLAG__ 輸出設定 0: 禁用 1: 啟用
  * @retval 無
  */
 #define __DRV_OUT_En(__FLAG__)               SetReg(DRV_CR, DRVOE, __FLAG__);
 
/** @brief 設定 KFG 計數時間
  * @note <pre>公式: FOC_KFG = 24 / (2^T4PSC * FBASE * FG_Pulse * Pole_Pairs)
  *  T4PSC : TIM4 除頻類型，參考: @ref TIM_ClockDivision
  *  FBASE : 速度基準值頻率
  *  FG_Pulse : FG 脈波數量
  *  Pole_Pairs : 電機極數(單位: 對)
  *</pre>
  * @param  __PSC__ TIM4 除頻類型，參考: @ref TIM_ClockDivision
  * @retval 無
  */ 
 #define KFG_Value(__PSC__)            ((FREC/1000000) / ((2 << ((__PSC__  >> 5) - 1)) * (BASE_FREQ / 1000000) * ((float)FG_cyclePulseNumber / (float)Pole_Pairs)))


 
/* Exported Varibles ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/ 
extern void TIM2_Initial(void);
extern void TIM3_Initial(void);
extern void TIM4_Initial(void);
extern void sysTick_Initial(void);

#endif /* __TIMER_H */

