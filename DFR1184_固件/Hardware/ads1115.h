#ifndef __ads1115_H
#define __ads1115_H

#include "cs32l010_hal.h"
#include "stdbool.h"	
#include "string.h"	

typedef enum{
    NACK = 0,
    ACK  = 1
}ACK_STATUS;

#define IIC_DELAY_TIME 10

#define IIC_SCL_PIN GPIO_PIN_5
#define IIC_SCL_PORT GPIOD
#define IIC_SDA_PIN GPIO_PIN_6
#define IIC_SDA_PORT GPIOD

#define IIC_SCL_H()     HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_SET)
#define IIC_SCL_L()     HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_RESET)
#define IIC_SDA_H()     HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_SET)
#define IIC_SDA_L()     HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_RESET)



//#define IIC_SCL_PIN GPIO_PIN_4
//#define IIC_SCL_PORT GPIOC

//#define IIC_SDA_PIN GPIO_PIN_3
//#define IIC_SDA_PORT GPIOC

//#define IIC_SCL_H()     HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_SET)
//#define IIC_SCL_L()     HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_RESET)
//#define IIC_SDA_H()     HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_SET)
//#define IIC_SDA_L()     HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_RESET)



void Soft_IIC_Init(void);
void Soft_IIC_Start(void);
void Soft_IIC_Stop(void);
void Soft_IIC_ACK(void);
void Soft_IIC_NACK(void);
uint8_t Soft_IIC_Wait_ACK(void);
uint8_t Soft_IIC_Write_Byte(uint8_t Byte);
uint8_t Soft_IIC_Recv_Byte(ACK_STATUS ack_sta);











#define ADS1115_WRITE_ADDRESS        0x90
#define ADS1115_READ_ADDRESS         0x91

#define DEVICE_ADDRESS	    0X90		// 从机设备地址

#define CONFIG_REG_H     ADS1115_REG_CONFIG_OS_START|\
                         ADS1115_REG_CONFIG_PGA_2|\
                         ADS1115_REG_CONFIG_MODE_CONTIN
#define CONFIG_REG_L     ADS1115_REG_CONFIG_DR_128|\
                         ADS1115_REG_CONFIG_COMP_MODE_TRADITIONAL|\
						   ADS1115_REG_CONFIG_COMP_POL_LOW|\
                         ADS1115_REG_CONFIG_COMP_LAT_NONLATCH|\
                         ADS1115_REG_CONFIG_COMP_QUE_DIS
//地址指针寄存器
 
#define ADS1015_REG_POINTER_CONVERT     (0x00)
#define ADS1015_REG_POINTER_CONFIG      (0x01)
#define ADS1015_REG_POINTER_LOWTHRESH   (0x02)
#define ADS1015_REG_POINTER_HITHRESH    (0x03)
// 单次转换开始
#define ADS1115_REG_CONFIG_OS_START                     (0x1U << 7)//设备单次转换开启 高字节的最高位
#define ADS1115_REG_CONFIG_OS_NULL                      (0x0U << 7)
//输入引脚选择和输入方式选择
#define ADS1115_REG_CONFIG_MUX_Diff_01                  (0x0U << 4)  // 差分输入0引脚和1引脚
#define ADS1115_REG_CONFIG_MUX_Diff_03                  (0x1U << 4)  // 差分输入0引脚和3引脚
#define ADS1115_REG_CONFIG_MUX_Diff_13                  (0x2U << 4)  // 差分输入1引脚和3引脚
#define ADS1115_REG_CONFIG_MUX_Diff_23                  (0x3U << 4)  // 差分输入2引脚和3引脚
#define ADS1115_REG_CONFIG_MUX_SINGLE_0                 (0x4U << 4)  //单端输入 0
#define ADS1115_REG_CONFIG_MUX_SINGLE_1                 (0x5U << 4)  //单端输入 1
#define ADS1115_REG_CONFIG_MUX_SINGLE_2                 (0x6U << 4)  //单端输入 2
#define ADS1115_REG_CONFIG_MUX_SINGLE_3                 (0x7U << 4)  //单端输入 3
 
//量程选择
#define ADS1115_REG_CONFIG_PGA_6                        (0x0U << 1) // +/- 6.144
#define ADS1115_REG_CONFIG_PGA_4                        (0x1U << 1) // +/- 4.096
#define ADS1115_REG_CONFIG_PGA_2                        (0x2U << 1) // +/- 2.048
#define ADS1115_REG_CONFIG_PGA_1                        (0x3U << 1) // +/- 1.024
#define ADS1115_REG_CONFIG_PGA_05                       (0x4U << 1) // +/- 0.512
#define ADS1115_REG_CONFIG_PGA_02                       (0x5U << 1) // +/- 0.256
 
//运行方式
 
#define ADS1115_REG_CONFIG_MODE_SINGLE              (0x1U << 0)  //  单次
#define ADS1115_REG_CONFIG_MODE_CONTIN              (0x0U << 0)  //连续转换
 
//转换速率
 
#define ADS1115_REG_CONFIG_DR_8                     (0x0U << 5)
#define ADS1115_REG_CONFIG_DR_16                    (0x1U << 5)
#define ADS1115_REG_CONFIG_DR_32                    (0x2U << 5)
#define ADS1115_REG_CONFIG_DR_64                    (0x3U << 5)
#define ADS1115_REG_CONFIG_DR_128                   (0x4U << 5)
#define ADS1115_REG_CONFIG_DR_250                   (0x5U << 5)
#define ADS1115_REG_CONFIG_DR_475                   (0x6U << 5)
#define ADS1115_REG_CONFIG_DR_860                   (0x7U << 5)
 
//比较器模式
#define ADS1115_REG_CONFIG_COMP_MODE_TRADITIONAL    (0x0U << 4)//默认
#define ADS1115_REG_CONFIG_COMP_MODE_WINDOW         (0x1U << 4)
 
 
#define ADS1115_REG_CONFIG_COMP_POL_LOW         (0x0U << 3)//默认
#define ADS1115_REG_CONFIG_COMP_POL_HIG         (0x1U << 3)
 
 
#define ADS1115_REG_CONFIG_COMP_LAT_NONLATCH        (0x0U << 2)
#define ADS1115_REG_CONFIG_COMP_LAT_LATCH           (0x1U << 2)
 
 
#define ADS1115_REG_CONFIG_COMP_QUE_ONE         (0x0U << 0)
#define ADS1115_REG_CONFIG_COMP_QUE_TWO         (0x1U << 0)
#define ADS1115_REG_CONFIG_COMP_QUE_THR         (0x2U << 0)
#define ADS1115_REG_CONFIG_COMP_QUE_DIS         (0x3U << 0)


void Ads1115_I2C_Init(void);
void I2C_Master_Clear_Error(void);
HAL_StatusTypeDef I2C_Read(uint8_t dev_address,uint8_t reg_address,uint8_t *rdata,uint8_t size);
HAL_StatusTypeDef I2C_Write(uint8_t dev_address,uint8_t reg_address,uint8_t *wdata,uint8_t size);

extern I2C_HandleTypeDef i2c_ads1115 ;
extern uint8_t mode_flag;//1为uart 0为iic 2为初始值
void ads1115_config_register(I2C_HandleTypeDef ads1115_I2cHandle,uint8_t pointADD,uint8_t configH,uint8_t configL);
int16_t ads1115_read_data(I2C_HandleTypeDef ads1115_I2cHandle);
double ads1115_get_voltage_val(I2C_HandleTypeDef ads1115_I2cHandle,uint8_t pointADD,uint8_t configH,uint8_t configL);

#endif
