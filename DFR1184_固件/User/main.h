/**
  ******************************************************************************
  * @file    main.h
  * @author  Application Team
	* @Version V1.0.0
  * @Date    1-April-2019	
  * @brief   Header for main.c file.
  *          This file contains the common defines of the application.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cs32l010_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "io_mapping.h"
#include "log.h"
#include "util.h"
#include <stdbool.h>

#include "ads1115.h"
//#include "myiic_slave.h"	
//#include "myiic_slave.h"	
#include "DFRobot_I2C_Slave.h"
/* USER CODE END Includes */


/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);


/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

