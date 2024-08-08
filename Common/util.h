/**
  ******************************************************************************
  * @file    util.h
  * @author  Application Team
	* @Version V1.0.0
  * @Date    1-April-2019
  * @brief   Header file of utility module.
  ******************************************************************************
  */

#ifndef __CS32L010_UTIL_H
#define __CS32L010_UTIL_H

/* Includes ------------------------------------------------------------------*/
 void SystemClock_Config(void);
 void debug_pin_init(void);
 void debug_pin_toggle(unsigned int num);
#endif /* __CS32L010_UTIL_H */
