/********************************************************************************

 **** Copyright (C), 2019, Fortior Technology Co., Ltd.                      ****

 ********************************************************************************
 * File Name     : FU68xx_4_Type.h
 * Author        : Bruce HW&RD
 * Date          : 2019-03-27
 * Description   : .C file function description
 * Version       : 1.0
 * Function List :
 * 
 * Record        :
 * 1.Date        : 2019-03-27
 *   Author      : Bruce HW&RD
 *   Modification: Created file

********************************************************************************/

#ifndef __FU68XX_4_TYPE_H__
#define __FU68XX_4_TYPE_H__

#define _I                              volatile const
#define _O                              volatile
#define _IO                             volatile

//#define bool                            bit
//#define false                           (0)
//#define true                            (!false)

typedef unsigned char                   uint8;
typedef unsigned short                  uint16;
typedef unsigned long                   uint32;
typedef long                            int32;
typedef short                           int16;
typedef char                            int8;

typedef enum{false = 0, true}           bool;
typedef enum{DISABLE = 0, ENABLE}       ebool;

#endif
