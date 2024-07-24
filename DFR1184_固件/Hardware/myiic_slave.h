#ifndef __MYIIC_SLAVE_H
#define __MYIIC_SLAVE_H

#include "cs32l010_hal.h"
#include "cs32l010_starterkit.h"
#include "stdbool.h"	

#define I2C_SPEED_RATE	100   //uint khz

#define I2C_SCL_PORT GPIOC
#define I2C_SCL_PIN  GPIO_PIN_4

#define I2C_SDA_PORT GPIOC
#define I2C_SDA_PIN  GPIO_PIN_3

#define I2C_DEVICE_ADDRESS	 0X11 // 本机地址
#define I2C_TEST_MASTER						0x00U
#define I2C_TEST_SLAVE						0x01U
#define I2C_TEST_IRQ_LEVEL					0x00U
#define	DATA_LEN_MAX						 255

#define TEST_FLAG_RECIVE					0X01U
#define	TEST_FLAG_TRANSM					0X02U

#define EEPROM_REGISTER1_ADDRESS	0XC0   // 寄存器地址
#define EEPROM_REGISTER2_ADDRESS	0XC1
extern 	I2C_HandleTypeDef i2c_test ;

void  iic_slave_init(void);
#endif
