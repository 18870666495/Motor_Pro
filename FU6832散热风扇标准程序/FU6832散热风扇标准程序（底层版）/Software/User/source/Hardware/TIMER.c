/**
  ******************************************************************************
  * @file           : TIMER.c
  * @copyright      : (c) 2022, Fortior Tech
  * @brief          : 
  * @date           : 2023-03-06
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

/* Private define ------------------------------------------------------------*/
#define PWM_DefPin                       (TIM3_CHANNEL_1)                     /*!< PWM 捕獲來源 ( @ref speedCtrl_Mode 為 PWM_Mode 時有效)  @see TIM3_CHANNEL_1 通道0 @see TIM3_CHANNEL_1 通道1*/



/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/
/**
  * @brief  TIM1 初始化函數
  * @bug    由於該函數還未完成，如果調用將不會有任何功能上的變化
  * @retval 無
  */
void TIM1_Initial(void)
{
  // do something.
}


/**
  * @brief    TIM2 初始化函數
  * @bug      由於該函數還未完成，如果調用將不會有任何功能上的變化
  * @retval   無
  */
void TIM2_Initial(void)
{
  // do something.
}


/**
  * @brief  TIM3 初始化函數
  * @note   TIM3 被作為 PWM 信號捕獲專用計數器，不可在 PWM 信號捕獲以外的應用中使用。
  * @retval 無
  */
void TIM3_Initial(void)
{
  SetReg(TIM3_CR0, (T3PSC2 | T3PSC1 | T3PSC0), TIM3_CLOCKMULTIPLICATION_MUL2);
  SetReg(TIM3_CR0, (T3OPM),                    TIM_AUTORELOAD_ENABLE);
  SetReg(TIM3_CR0, (T3OCM),                    TIM_INPUT_RISING);
  SetReg(TIM3_CR0, (T3MOD),                    TIM_BASE_Mode);
  SetReg(TIM3_CR1, (T3IPE | T3IFE),            TIM_TS_TI1F1P);
  SetReg(TIM3_CR1, (T3NM1 | T3NM0),            TIM_TriggerFilter_NONE);
  SetReg(PH_SEL1,  (T3CT),                     PWM_DefPin);
  
  TIM3__ARR = 0;
  TIM3__DR  = 0;
  
  SetReg(IP2, (PTIM31 | PTIM30), TIM3_IP_10); 
  
  SetBit(TIM3_CR1, T3EN);                                   // 致能 TIM3 功能
  SetBit(PH_SEL, T3SEL);                                    // 啟動 TIM3
}


/**
  * @brief    system tick 初始化函數
  * @retval   無
  */
void sysTick_Initial(void)
{
  SetReg(IP2, (PSYSTICK1 | PSYSTICK0), SYSTICK_IP_01);      // 設定 SYSTick 中斷優先權為 1
  SetReg(DRV_SR, SYSTIE, SYSTICK_TS_IF1F);
}


/**
  * @brief    TIM4 初始化函數
  * @note     TIM4 被作為 FG 信號輸出專用計數器，當在其他應用中使用，
              必須以不影響 FG 輸出為首要條件進行開發。
  * @retval   無
  */
void TIM4_Initial(void)
{
  ClrBit(PH_SEL, T4SEL);                                     // 停止 TIM4
  ClrBit(TIM4_CR0, T4IRE);
  
  SetReg(TIM4_CR0, (T4PSC2 | T4PSC1 | T4PSC0), FG_ClockBase);
  SetReg(TIM4_CR0, (T4OPM),                    TIM_AUTORELOAD_ENABLE);
  SetReg(TIM4_CR0, (T4OCM),                    TIM_OCNPOLARITY_LOW);
  SetReg(TIM4_CR0, (T4MOD),                    TIM_PWM_Mode);
  SetReg(TIM4_CR1, (T4IPE  | T4IFE),           TIM_TS_TI1F);
  SetReg(TIM4_CR0, (T4IRE),                    TIS_TS_NOIR);
  SetReg(TIM4_CR1, (T4NM1 | T4NM0),            TIM_TriggerFilter_NONE);
  SetReg(PH_SEL1,  (T4CT),                     TIM4_CHANNEL_1);
  
  TIM4__ARR = 468;
  TIM4__DR  = 540;
  FOC_KFG = 0;
  
  SetReg(IP2, (PTIM41 | PTIM40), TIM4_IP_11);

  //SetBit(PH_SEL, T4SEL); 
  SetBit(TIM4_CR1, T4EN);                                   // 致能 TIM4 功能
}
