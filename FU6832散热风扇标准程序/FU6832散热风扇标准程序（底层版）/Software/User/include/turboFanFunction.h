/**
  ******************************************************************************
  * @file           : turboFanFunction.h
  * @copyright      : (c) 2022, Fortior Tech
  * @brief          : 
  * @date           : 2022-12-27
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
#ifndef __TURBOFANFUNCTION_H
#define __TURBOFANFUNCTION_H
/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
#define AVC_TURBOFAN_TxLEN                    (8)

/**
  * @brief TURBOFAN_HandleTypeDef 結構定義
  */
typedef struct
{
  uint8   Tx_DATA[AVC_TURBOFAN_TxLEN];  /*!< TxD 暫存 */
  uint16  TimeCount;                    /*!< 檢測時間計數器 */ 
  uint16  Refresh_TIM;                  /*!< 更新時間 */ 
  uint16  Receive_TIMOUT;               /*!< 接收逾時計數器 */ 
  uint8   isTirg;                       /*!< 自檢測警報旗標 */ 
  uint8   isActive;                     /*!< 運行旗標 */ 
  uint8   isError;                      /*!< 警報接收旗標 */ 
  uint8   isRespond;                    /*!< 響應旗標 */ 
}TURBOFAN_HandleTypeDef;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Exported Varibles ---------------------------------------------------------*/
extern TURBOFAN_HandleTypeDef c_TurboFan;

/* Exported functions --------------------------------------------------------*/ 
extern void TurboFan_Initial(void);
extern void TurboFan_CountEvent(void);
extern void TurboFan_ProcessEvent(void);

#endif /* __TURBOFANFUNCTION_H */
