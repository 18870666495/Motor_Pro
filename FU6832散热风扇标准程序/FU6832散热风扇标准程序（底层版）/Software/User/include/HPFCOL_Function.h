/**
  ******************************************************************************
  * @file           : HPFCOL_Function..h
  * @copyright      : (c) 2022, Fortior Tech
  * @brief          : 
  * @date           : YYYY-MM-DD
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
#ifndef __HPFCOL_FUNCTION_H
#define __HPFCOL_FUNCTION_H
/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* example */
#define example                               (0)                                     /*!< example */

/* Exported macro ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Exported Varibles ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/ 
extern void HPFCOL_Initial(void);
extern void HPFCOL_CommDetection(void);
extern void HPFCOLCounter_Event(void);
extern void HPFCOLUpdate_Event(void);
extern uint16 HPFCOL_SpeedControl(uint16 VSP);

#endif /* __HPFCOL_FUNCTION_H */