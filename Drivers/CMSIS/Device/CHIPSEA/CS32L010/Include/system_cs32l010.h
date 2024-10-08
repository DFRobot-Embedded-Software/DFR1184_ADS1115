/**
  ******************************************************************************
  * @file    system_cs32l010.h
  * @author  Software Team
  * @version V1.0.0
  * @date    21-Oct-2019
  * @brief   CMSIS Cortex-M0+ Device Peripheral Access Layer Header File. 
  *          This file contains all the peripheral register's definitions, bits 
  *          definitions and memory mapping for CX32L0xx devices.            
  *            
  *          This file contains:
  *           - Data structures and the address mapping for all peripherals
  *           - Peripheral's registers declarations and bits definition
  *  
  ******************************************************************************
  */
	
#include "stdint.h"

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup cs32l010_system
  * @{
  */  
  
/**
  * @brief Define to prevent recursive inclusion
  */
#ifndef __SYSTEM_CS32L010_H
#define __SYSTEM_CS32L010_H

#ifdef __cplusplus
 extern "C" {
#endif 



/* Includes ------------------------------------------------------------------*/
/** @addtogroup CS32L010_System_Includes
  * @{
  */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @addtogroup CS32L010_System_Exported_Constants
  * @{
  */

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/** @addtogroup CS32L010_System_Exported_types
  * @{
  */
extern uint32_t SystemCoreClock;          /*!< System Clock Frequency (Core Clock) */	

/**
  * @}
  */
	
	
/* Exported macro ------------------------------------------------------------*/
/** @addtogroup CS32L010_System_Exported_Macros
  * @{
  */

/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/** @addtogroup CS32L010_System_Exported_Functions
  * @{
  */
#if !defined  (HIRC_VALUE_24M) 
  #define HIRC_VALUE_24M    		((uint32_t)24000000) 	/*!< Value of the Internal oscillator select 24MHz */	
#endif /* HIRC_VALUE_24M */

#if !defined  (HIRC_VALUE_22M) 
  #define HIRC_VALUE_22M    		((uint32_t)22120000) 	/*!< Value of the Internal oscillator select 22.12MHz */	
#endif /* HIRC_VALUE_22M */

#if !defined  (HIRC_VALUE_16M) 
  #define HIRC_VALUE_16M    		((uint32_t)16000000) 	/*!< Value of the Internal oscillator select 16MHz */	
#endif /* HIRC_VALUE_16M */

#if !defined  (HIRC_VALUE_8M) 
  #define HIRC_VALUE_8M    			((uint32_t)8000000) 	/*!< Value of the Internal oscillator select 8MHz */	
#endif /* HIRC_VALUE_8M */

#if !defined  (HIRC_VALUE_4M) 
  #define HIRC_VALUE_4M    			((uint32_t)4000000) 	/*!< Value of the Internal oscillator select 4MHz */	
#endif /* HIRC_VALUE_4M */
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_CS32L010_H */

/**
  * @}
  */ /* End of group cs32l010_system */
  
/**
  * @}
  */ /* End of group CMSIS */
	
/************************ (C) COPYRIGHT ChipSea*****END OF FILE****/
