/**
  ******************************************************************************
  * @file           : Customer.h
  * @copyright      : (c) 2022, Fortior Tech
  * @brief          : 
  * @date           : 2023-03-16
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
  ******************************************************************************
  * @brief         FU6832_CONFIGURATION FU6832 參數配置檔
  * @details  *** 使電機運行前務必確認列表內容是否配置正確 ***
  * 1. 電機參數相關配置
  * 2. 電流採樣相關配置
  * 3. 母線電壓量測相關配置
  * 4. 確認各項可選功能是否配置錯誤
  ******************************************************************************
*/  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CUSTOMER_H
#define __CUSTOMER_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/** @brief   電機參數相關配置 */

/*motor Parameter*/
#define Pole_Pairs    2
#define L_Adj         1.0
#define RS            (0.09500 * 1.0)
#define LD            (0.000077 * L_Adj)
#define LQ            (0.000077 * L_Adj)
#define Ke            (0.392302 * 1.0)



/** @brief PWM 驅動信配置 */
#define PWM_FREQUENCY                         (25.0)                                  /*!< (單位: Khz) 載波頻率 */
#define PWM_DEADTIME                          (0.6)                                   /*!< (單位: uS) 載波死區時間 */
#define MIN_WIND_TIME                         (PWM_DEADTIME + 0.6)                    /*!< (單位: uS) 最小取樣時間 */


/** @brief SMO 估算器角度補償相關配置 */
#define SMO_OffSET_Enable                     (0)                                     /*!< 啟用 SMO 估算器角度補償  @param (0) 禁用 @param (1) 啟用 */
#define SMO_OffSET_DEGREE                     (8.0)                                   /*!< (單位: 度) SMO 估算器角度補償量 [範圍: -30.0 ~ 30.0] */


/** @brief 電流採樣相關配置 */
#define Shunt_Resistor_Mode                   (Single_Resistor)                       /*!< 取樣電阻模式配置  @see Single_Resistor 使用單電阻採樣 @see Double_Resistor 使用雙電阻採樣 @see Three_Resistor 使用三電阻採樣 */
#define HW_RSHUNT                             (0.01)                                  /*!< (單位: Ω) 取樣電阻配置 [建議值: 0.02 ~ 0x005] */
#define HW_AMPGAIN                            (Gain_8)                               /*!< 放大倍率配置  @see Gain_2 使用內部 2倍放大 @see Gain_4 使用內部 4倍放大 @see Gain_8 使用內部8倍放大 @see Gain_16 使用內部 16倍放大 @see Gain_32 使用內部 32倍放大*/


/** @brief 母線電壓量測相關配置 */
#define HW_RV_MODE                            (RVEXT)                                 /*!< 母線電壓量測來源配置  @see RVEXT 使用外部分壓配置 @see RV6P5 使用內部分壓配置 (1 / 6.5) @see RV12 使用內部分壓配置 (1 / 12) */
#define RV1                                   (100.0)                                  /*!< (單位: kΩ) 母線分壓電阻1 (當 @ref HW_RV_MODE 為 @ref RVEXT 時有效) */ 
#define RV2                                   (30.0)                                   /*!< (單位: kΩ) 母線分壓電阻2 ( @ref HW_RV_MODE 為 @ref RVEXT 時有效) */


/** @brief 電機旋向配置 */
#define MOTOR_DIRECTION                       (0)                                     /*!< 電機旋向配置  @see CW 電機正轉 @see CCW 電機反轉 */


/** @brief 電機延遲啟動配置 */
#define MOTOR_STARTDELAY_Enable               (0)                                     /*!< 啟用 電機延遲啟動  @param (0) 禁用 @param (1) 啟用 */
#define MOTOR_STARTDELAY_TIME                 (2000)                                  /*!< (單位: mS) 電機啟動延遲時間  [範圍: 1000 ~ 30000] */


/** @brief 電機預定位相關配置 */
#define MOTOR_ALIGN_Enable                    (1)                                     /*!< 電機預定位致能設置  @param (0) 禁用 @param (1) 啟用 */
#define ALIGN_TIME                            (600)                                   /*!< (單位: mS)   電機預定位時間  [範圍:200 ~ 3000]*/
#define ALIGN_ANGLE                           (15.0)                                  /*!< (單位: deg)  電機預定位角度  [範圍:(整數)-180 ~ 180]*/
#define ALIGN_DQ_VALUE                        _Q15(0.06)                              /*!< 電機預定位 UD 數值 [範圍: _Q15(0.01) ~ _Q15(0.2)] */
#define ALIGN_DQ_MAXVALUE                     _Q15(0.25)                              /*!< 電機預定位分量最大值 [範圍: ALIGN_DQ_VALUE ~ _Q15(0.6)] */


/** @brief 電機運轉相關配置 */
#define SPD_BW                                (20.0)                                  /*!< 速度帶寬濾波 [範圍:5 ~ 40] */
#define TailWind_SPD_BW                       (150.0)                                 /*!< 估算器 Kp Ki 參數 */



/** @defgroup MOTOR_CONTROL_BASIC_ITEM 電機基礎運行配置
  * @brief   電機基礎運行相關配置
  * @details 本配置包含：啟動、FOC-D\Q 軸限幅，以及 FOC-PID 調整等功能設置。
  * @{
  */
#define MOTOR_OPEN_ACC                        _Q15(0.011)                             /*!< 強拖啟動增量 */
#define MOTOR_OPEN_ACC_MIN                    (0.0)                                   /*!< 強拖啟動初始數值 */
#define MOTOR_OPEN_ACC_CNT                    (65.0)                                  /*!< 強拖啟動執行次數 */
#define Motor_Omega_Ramp_ACC                  (2)                                     /*!< Omega 啟動增量 */
#define MOTOR_OMEGA_ACC_MIN                   _Q15(0.015)                             /*!< Omega 啟動最低數值 */
#define MOTOR_OMEGA_ACC_END                   _Q15(0.035)                             /*!< Omega 啟動限制數值 */
#define MOTOR_LOOP_RPM                        (930.0)                                 /*!< (單位: RPM) Omega 啟動進入閉環轉速 */


#define DQKP_START                            _Q12(1.0)                               /*!< 電機啟動階段 DQ軸 PID Kp */
#define DQKI_START                            _Q15(0.01)                              /*!< 電機啟動階段 DQ軸 PID Ki */
#define DQKP                                  _Q12(0.8)                               /*!< 電機運行階段 DQ軸 PID Kp */
#define DQKI                                  _Q15(0.01)                              /*!< 電機運行階段 DQ軸 PID Ki */


#define ID_Start_CURRENT                      I_Value(0.0)                            /*!< (單位: A) 電機啟動階段 D軸電流 [預設為 0] */
#define IQ_Start_CURRENT                      I_Value(3.0)                            /*!< (單位: A) 電機啟動階段 Q軸電流 */
#define ID_RUN_CURRENT                        I_Value(0.0)                            /*!< (單位: A) 電機運行階段 D軸初始電流 */
#define IQ_RUN_CURRENT                        I_Value(1.5)                            /*!< (單位: A) 電機運行階段 Q軸初始電流 */


#define ATO_BW                                (20.0)                                  /*!< 電機初始化階段 ATO帶寬 */
#define ATO_BW_RUN                            (40.0)                                  /*!< 電機啟動階段 1 ATO帶寬 */
#define ATO_BW_RUN1                           (50.0)                                  /*!< 電機啟動階段 2 ATO帶寬 */
#define ATO_BW_RUN2                           (60.0)                                  /*!< 電機啟動階段 3 ATO帶寬 */
#define ATO_BW_RUN3                           (70.0)                                  /*!< 電機啟動階段 4 ATO帶寬 */
#define ATO_BW_RUN4                           (90.0)                                  /*!< 電機啟動階段 5 ATO帶寬 */


#define DOUTMAX                               _Q15(0.8)                               /*!< 電機運行 D軸最大幅值 [範圍: _Q15(0.001) ~ _Q(0.999)] */
#define QOUTMAX                               _Q15(0.98)                              /*!< 電機運行 Q軸最大幅值 [範圍: _Q15(0.001) ~ _Q(0.999)] */

/**
  * @}
  */



/** @defgroup MOTOR_CONTROL_LOOP_ITEM 電機運行控制迴路配置項目
  * @brief   電機運行控制迴路相關配置
  * @details 本配置包含：外環控制(電流\速度環)、PID，以及 PWM 調速等功能設置。
  * @{
  */
  
#define OUTLOOP_Enable                        (1)                                     /*!< 啟用外環控制  @param (0) 禁用 @param (1) 啟用 */


#define SPEED_REFRESH_TIME                    (12)                                    /*!< (單位: mS) 轉速偵測刷新時間 [範圍: 5 ~ 200] */


#define QOUTINC                               (8)                                     /*!< 電流環遞增量  ( @ref OUTLOOP_Enable 禁用時時有效) */
#define QOUTMINVALUE                          I_Value(0.04)                           /*!< 電流環輸出下限  ( @ref OUTLOOP_Enable 禁用時時有效) */
#define QOUTMAXVALUE                          I_Value(10.0)                           /*!< 電流環輸出上限  ( @ref OUTLOOP_Enable 禁用時時有效) */

/** @brief 速度環 PID 配置 */
#define SPEED_KP                              _Q12(0.8)                               /*!< 速度環 PID Kp [範圍: 0 ~ 6.999]  ( @ref OUTLOOP_Enable 啟用時時有效) */
#define SPEED_KI                              _Q15(0.015)                             /*!< 速度環 PID Ki [範圍: 0 ~ 0.999]  ( @ref OUTLOOP_Enable 啟用時時有效) */
#define SPEED_KD                              _Q15(0.004)                             /*!< 速度環 PID Kd [範圍: 0 ~ 0.999]  ( @ref OUTLOOP_Enable 啟用時時有效) */

/** @brief 速度環 PID 幅值配置 */
#define SOUTMIN                               I_Value(0.1)                            /*!< 速度環 PID 幅值下限  ( @ref OUTLOOP_Enable 啟用時時有效) */
#define SOUTMAX                               I_Value(10.0)                           /*!< 速度環 PID 幅值上限  ( @ref OUTLOOP_Enable 啟用時時有效) */

/** @brief 調速控制模式配置 */
#define SPEED_MODE                            (VSPMODE)                               /*!< 調速控制模式配置  @see NONEMODE 無調速模式 @see PWMMODE PWM 調速模式 @see VSPMODE VSP 調速模式 */


#define NONEMODE_SPEED                        (2000)                                  /*!< (單位: RPM) 無調速轉速 (範圍: 1000 ~ 40000)  ( @ref SPEED_MODE 為 NONEMODE 時有效) */


#define WIDEBAND_PWM_Enable                   (0)                                     /*!< 啟用 PWM 寬頻率支援  @param (0) 禁用 @param (1) 啟用  ( @ref SPEED_MODE 為 PWMMODE 時有效) */
#define PWM_INVERT_Enable                     (1)                                     /*!< 啟用 PWM 信號倒相  @param (0) 禁用 @param (1) 啟用 */

/** @brief PWM 開關機配置 */
#define MOTOR_SHUTDOWN_Enable                 (0)                                     /*!< 啟用 PWM 開關機控制  @param (0) 禁用 @param (1) 啟用  ( @ref SPEED_MODE 為 PWMMODE 時有效) */
#define PWMDUTY_TURNOFF                       _Q15(0.025)                             /*!< (單位: %) PWM 開機站空比 範圍: ( @ref 0 ~ 0.999]  ( @ref SPEED_MODE 為 PWMMODE 時有效) */
#define PWMDUTY_TURNON                        _Q15(0.045)                             /*!< (單位: %) PWM 開機站空比 範圍: ( @ref 0 ~ 0.999]  ( @ref SPEED_MODE 為 PWMMODE 時有效) */

/*VSPinput OPENLOOP*/
#define Max_VSP_Voltage                (32600) //5V
#define Min_VSP_Voltage                (0)  //1V
#define Stop_VSP_Voltage               (0)
#define Start_VSP_Voltage              (0)
#define MotorOnFilterTime              (20)
#define MotorOffFilterTime             (20)

/** 
  * @brief   PWM 調速節點數量
  * @details 節點數量範圍: 2 ~ 24點。 ( @ref SPEED_MODE 為 PWMMODE 時有效)
  * @note    過多的節點會造成容量緊迫，須注意應用場景是否需要如此多的節點。
  */
#define MOTOR_PWMMODE_NODENUM                 (13)


#define PWM_MINDUTY                           _Q15(0.0)                             /*!< (單位: %) PWM 最低站空比 範圍: ( @ref 0 ~ 0.999]  ( @ref SPEED_MODE 為 PWMMODE 時有效) */
#define PWM_MAXDUTY                           _Q15(1.0)                             /*!< (單位: %) PWM 最高站空比 範圍: ( @ref 0 ~ 0.999]  ( @ref SPEED_MODE 為 PWMMODE 時有效) */
#define MOTOR_MINSPEED                        (1900.0)                                /*!< (單位: RPM) 電機最低轉速 ( @ref SPEED_MODE 為 PWMMODE 時有效) */
#define MOTOR_MAXSPEED                        (20650.0)                               /*!< (單位: RPM) 電機最高轉速 ( @ref SPEED_MODE 為 PWMMODE 時有效) */


/** 
  * @brief   PWM 調速節點站空比
  * @details (單位: %) PWM 調速節點站空比數量必須與 @ref MOTOR_PWMMODE_NODENUM 點數相同  ( @ref SPEED_MODE 為 PWMMODE 時有效)
  */
#define MOTOR_PWMMODE_NODEDUTY                {PWM_MINDUTY,_Q15(0.1),_Q15(0.2),_Q15(0.3),_Q15(0.4),_Q15(0.5),_Q15(0.6),_Q15(0.7),_Q15(0.8),_Q15(0.85),_Q15(0.9),_Q15(0.95), PWM_MAXDUTY}


/** 
  * @brief   PWM 調速節點轉速
  * @details (單位: RPM) PWM 調速節點轉速數量必須與 @ref MOTOR_PWMMODE_NODENUM 點數相同  ( @ref SPEED_MODE 為 PWMMODE 時有效)
  */
#define MOTOR_PWMMODE_NODESPEED               {(MOTOR_MINSPEED),(3800),(5700),(7574),(9474),(11374),(13248),(15148),(17048),(17998),(18948),(19898),(MOTOR_MAXSPEED)}
/**
  * @}
  */


/** @defgroup MOTOR_TAILWIND_ITEM 電機順逆風配置項目
  * @brief   電機順逆風相關配置
  * @details 本配置包含：電機順逆風配置。
  * @{
  */
#define TailWindEnable                        (1)                                     // 順風功能致能 [0:禁能 1:致能]
#define MOTOR_SPEED_SMOMIN_RPM                (1300.0)                                // SMO 最低運行轉速 單位: RPM 2300
#define TailWind_StartSpeed                   (100.0)                                 // 順風最低運行轉速 單位: RPM
#define windHighSpeed_Threshold               (MOTOR_MAXSPEED * 0.8)                  // 順風速度高速判定閥值
#define TailWindAlign_Time                    (1800)                                  // 順逆風狀態下預定位時間 單位: mS
#define TailWindDQKP                          _Q12(5.5)                               // 電機運行 DQ軸 Kp(改小可壓低順風Peak)
#define TailWindDQKI                          _Q15(0.4)                               // 電機運行 DQ軸 Ki
#define TailWind_Speed_Kp                     _Q12(0.05)                              // 速度環 Kp 
#define TailWind_Speed_Ki                     _Q15(0.015)                             // 速度環 Ki
#define IQ_CloseLoopStart_CURRENT_H           I_Value(1.4)                            // 電機順風啟動 Q軸電流 單位: A，用於高速順風
#define IQ_CloseLoopStart_CURRENT_M           I_Value(1.4)                            // 電機順風啟動 Q軸電流 單位: A，用於中速順風
#define IQ_CloseLoopStart_CURRENT_L           I_Value(1.4)                            // 電機順風啟動 Q軸電流 單位: A，用於低速順風
#define CloseLoopStart_UQ_Value_H             (13000)                                 // 電機順風啟動UQ數值，用於高速順風
#define CloseLoopStart_UQ_Value_M             (8000)                                  // 電機順風啟動UQ數值，用於中速順風
#define CloseLoopStart_UQ_Value_L             (2000)                                  // 電機順風啟動UQ數值，用於低速順風
#define IQ_Tailwind_Start_CURRENT             I_Value(2.8)                            // 電機逆風啟動 Q軸電流 單位: A       
#define IQ_Tailwind_RUN_CURRENT_LOW           I_Value(1.5)                            // 電機逆風啟動 Q軸電流 單位: A
#define IQ_Tailwind_RUN_CURRENT_HIGH          I_Value(1.5)                            // 電機逆風啟動 Q軸電流 單位: A
#define Inverse_Current_Function              (0)
#define ATO_BW_Wind                           (100.0)                                 // 逆风判断观测器带宽的滤波值，经典值为8.0-100.0
#define SPD_BW_Wind                           (10.0)                                  // 逆风判断速度带宽的滤波值，经典值为5.0-40.0
#define DQKP_TailWind                         _Q12(1.5)                               
#define DQKI_TailWind                         _Q15(0.18)                              
#define ATO_CCW_BW                            (17.0)                                  // 估算器帶寬濾波數值
#define ID_TailWind_CCW_CURRENT               I_Value(0.0)                            // (A) D軸啟動電流
#define IQ_TailWind_CCW_CURRENT1              I_Value(1.0)                            // (A) Q軸啟動電流
#define IQ_TailWind_CCW_CURRENT2              I_Value(11.5)                           // (A) 逆風啟動Q軸電流，須留意SOUTMAX參數
#define IQ_TailWind_CCW_Run_CURRENT           I_Value(5.0)                            // (A) Q軸啟動電流

/**
  * @}
  */



/** @defgroup MOTOR_OPTIONAL_FUNCTION_ITEM 電機可選功能配置項目
  * @brief    電機可選功能相關配置
  * @details  本配置包含：信號輸出、電流限制器、功率限制器、COASTMODE、HP-FCOL、AVC_TURBOFAN 等功能設置。
  * @{
  */
  
/** @brief 電機轉速信號輸出配置 */
#define ROTATESIGNAL_TYPE                     (FG_TYPE)                               /*!< 轉速信號輸出類型  @see NO_TYPE 無信號 @see RD_TYPE RD信號 @see FG_TYPE FG信號 */


/** 
  * @brief   設定 電流限制器 相關項目
  * @details 啟用 電流限制器後，電流將被限制在 CURRENT_LIMITER_THRESHOLD 所設定的閾值正負 6% 以內，
  *          如果發現無法限流則須調整 CURRENT_LIMITER_SOURCE 設定，或者確認電路板是否受損。
  */
#define CURRENT_LIMITER_Enable                (0)                                     /*!< 啟用 電流限制器  @param (0) 禁用 @param (1) 啟用 */
#define CURRENT_LIMITER_THRESHOLD             (2.0)                                   /*!< (單位: A) 電流限制閾值  [範圍: 1.0 ~ HW_BOARD_CURR_MAX] */
#define CURRENT_LIMITER_SOURCE                (USED_AD13)                              /*!< 電流限制器檢測來源  @param USED_AD1 使用AD1 @param USED_AD3 使用AD3 @param USED_AD13 使用AD13 */

/*功耗限制功能*/
#define Watt_Limit_Enable                     (0)                                     /*!< 功率限制致能  @param (0) 禁用 @param (1) 啟用 */                  
#define LIMIT_MIN_CURRENT         		        I_Value(0.6)
#define Watt_Limit_Target          		        (50.0)                                   //單位：W，母線瓦數	
#define Watt_Limit_Modeify_Value   		        (0.0)  

/** 
  * @brief   設定 DELL-COASTMODE 相關項目
  * @warning DELL-COASTMODE 項目已毀損，暫時無法使用。
  */
#define AC_Lose_Function_Enable               (1)                                     //AC Lose Function開關功能
#define AC_Lose_Limit_Power_Value             I_Value(0.8)                            //AC Lose狀態下電流限制值
//#define AC_Lose_Max_Duty                      _Q15(0.999)                             //AC Lose狀態下最高PWM控速Duty   <已棄用>
#define AC_Lose_Tailwind_Speed                (10000.0)                               //AC Lose狀態下順風轉速
//#define AC_Lose_SVPWM_5_Step_Enable           (0)                                     //AC Lose五段功能開啟後Limit_Power_Value需要重新調整，建議不開啟     <已棄用>
//#define Elf_Swiching                          (4)                                     //AC Lose判斷參數，請勿調整                 <已棄用>

/** 
  * @brief   設定 HP-FCOL 相關項目
  * @details 啟用 HP-FCOL 功能時將會自動接管 ROTATESIGNAL_TYPE、MOTOR_STARTDELAY_Enable、MOTOR_STARTDELAY_TIME 
  *          以及 MOTOR_BRAKE_MODE 之設定項目，如果要修改相關設定須至 Parameter.h 檔案內搜尋 HPFCOL_Enable 項目進行設定。
  */
#define HPFCOL_Enable                         (0)                                     /*!< 啟用 HP-FCOL 功能  @param (0) 禁用 @param (1) 啟用 */
#define HPFCOL_SWCRC_CHECK_Enable             (1)                                     /*!< 啟用 HP-FCOL 軟體驗證 CRC 功能  @param (0) 禁用 @param (1) 啟用 */
//#define HPFCOL_RDA_TYPE                       (LOGIC_RDA)                             /*!< HP-FCOL 樣式  @see LOGIC_RDA 邏輯準位 @see HREQUENCY_RDA 頻率 */     <已棄用>


/** @brief    AVC 專用 TurboFan 功能相關配置
  * @details  該項目為 AVC TurboFan 專用功能，不可用於其他案件中，擅自應用出現任何狀況時，需自行處理。
  *           以下為TurboFan 運行機制說明:\n
  *           1. TurboFan 運作僅適用於雙轉子電機中，當任一級轉速發生異常(低於 50% 目標轉速)且持續超過 10 秒，另一級將以 @ref AVC_TURBOFAN_RUN_SPEED 設置的轉速進行補償，且無法進行調速。
  *           2. 解除 TurboFan 則是當異常的風扇轉速恢(到達 70% 目標轉速)且超過 10 秒，運行TurboFan的電機將解除補償，恢復到正常運行。
  */
#define AVC_TURBOFAN_Enable                   (0)                                     /*!< 啟用 AVC 專用 TurboFan 功能  @param (0) 禁用 @param (1) 啟用 */
#define AVC_TURBOFAN_ACTIVE_Enable            (1)                                     /*!< 啟用 AVC 專用 TurboFan 功能響應  @param (0) 禁用 @param (1) 啟用 */
#define AVC_TURBOFAN_TYPE                     (MASTER)                                /*!< AVC 專用 TurboFan 功能模式  @see MASTER 主機 @see SLAVE 從機 */
#define AVC_TURBOFAN_RUN_SPEED                (34000)                                 /*!< (單位: RPM) AVC 專用 TurboFan 模式下電機轉速 */
#define AVC_TURBOFAN_DETECTION_TIME           (10000)                                 /*!< (單位: mS) AVC 專用 TurboFan 觸發時間 */

/** @brief 電機制動模式配置 */
#define MOTOR_BRAKE_MODE                      (NONEBRAKE)                             /*!< 電機制動模式  @see NONEBRAKE 無制動 @see ALWAYSBRAKE 啟動制動 @see ONLYSTARTBRAKE 僅上電時制動 @see ONLYSTOPBRAKE 僅停職時制動 */


#define Slowdown_Response_Enable              (1)


#define RUNSVPWM5_Enable                      (0)                                     /*!< 啟用 5段式運行功能  @param (0) 禁用 @param (1) 啟用  ( @ref Shunt_Resistor_Mode 為 Single_Resistor 時有效) */
#define SVPWM5_SWITCH_THRESHOLD               (5000.0)                                /*!< (單位: RPM) 5段式運行切換閾值  [範圍: MOTOR_SPEED_SMOMIN_RPM ~ MOTOR_MAXSPEED] */
#define SVPWM5_BUFFER_THRESHOLD               (300.0)                                 /*!< (單位: RPM) 5段式運行切換緩衝  [範圍: 100 ~ 500] */
#define SVPWM5_ATT_LEVEL                      (1)                                     /*!< 5段式運行切換衰減等級  @param (0) 無衰減 @param (1) 衰減1/2 @param (1) 衰減1/4 */


#define OverModulation_Enable                 (1)                                     // 0:禁能 1:致能


#define FOC_TSMIN_Close_Enable                (0)                                     //採樣窗口關閉功能

/**
  * @}
  */



/** @defgroup MOTOR_EXPERIMENTAL_ITEM 電機實驗性配置項目
  * @brief    電機實驗性功能相關選項
  * @details  本配置包含：預定位測試選項。
  * @{
  */
  
#define AlignTestMode                         (0)                                     // 預定位測試模式

/**
  * @}
  */



/* Exported macro ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Exported Varibles ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/ 


#endif /* __CUSTOMER_H */
