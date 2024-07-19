#ifndef _DFROBOT_I2C_SLAVE_H_
#define _DFROBOT_I2C_SLAVE_H_
#include "cs32l010_hal.h"
#include "stdint.h"
#include "stdbool.h"
#define I2C_DEVICE_ADDRESS 0x48 //本机地址  0x48 0x49 0x4A 0x4B
/*
拨码开关
A0		A1		add
0		0		0+0x48
1		0		0x01+0x48
0		1		0x02+0x48
1		1		0x03+0x48

*/
#define CHANNEL_SELECT_ADDRESS  0x20 //选择ad通道寄存器地址
#define CHANNEL_DATA_ADDRESS  0x30 //ad数据地址
#define	DATA_LEN_MAX                    255


#define I2C_SCL_PORT GPIOC
#define I2C_SCL_PIN  GPIO_PIN_4

#define I2C_SDA_PORT GPIOC
#define I2C_SDA_PIN  GPIO_PIN_3


/******* i2c register    ********/
#define I2C_M1_DIRECTION    0
#define I2C_M1_SPEED        1
#define I2C_M2_DIRECTION    2
#define I2C_M2_SPEED        3




/**
 * @brief I2C从机初始化
 */
void i2cInitSlave(void);

/**
 *@brief 初始化I2C中断
 */
void i2cIRQConfig(void);

extern uint8_t i2c_interrupt;
extern uint8_t r_address;
extern uint8_t pData[DATA_LEN_MAX];
extern uint8_t  rx_count;

extern uint8_t i2c_buffer[DATA_LEN_MAX];
extern I2C_HandleTypeDef i2cSlave ;

#endif
