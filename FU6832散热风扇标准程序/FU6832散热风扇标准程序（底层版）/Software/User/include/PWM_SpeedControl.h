/**
  ******************************************************************************
  * @file           : PWM_SpeedControl..h
  * @copyright      : (c) 2022, Fortior Tech
  * @brief          : 
  * @date           : 20223-02-14
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
#ifndef __PWM_SPEEDCONTROL_H
#define __PWM_SPEEDCONTROL_H
/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  uint16 Compare;
  uint16 Cycle;
  int16  Duty;
  uint8  isBusy;
  uint8  ShutDown_isActive;
  
  uint8  turnOn_CNT;
  uint8  turnOff_CNT;
  
  #if (WIDEBAND_PWM_Enable == 1)
   uint8  wideBand_Flag;
   uint8  wideBand_CNT;
  #endif 
  
}PWMCapture_TyperDef;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Exported Varibles ---------------------------------------------------------*/
extern PWMCapture_TyperDef mcPWM_Input;

/* Exported functions --------------------------------------------------------*/ 
extern void PWM_capture(void);
extern void PWM_overFlowDetection(void);
extern void PWM_speedControl(void);


#endif /* __FILENAME_H */