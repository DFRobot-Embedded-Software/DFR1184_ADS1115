#include "myiic_slave.h"	

//FlagStatus i2c_int = RESET;
//volatile uint8_t test_falg = 0;
//uint8_t pData[DATA_LEN_MAX]={0};
//uint16_t RV_Size = 0;
//uint16_t i2c_RX_receive_data;
//uint16_t i2c_TX_Send_data;
//uint16_t TR_Size = 2;
//uint32_t printf_i2c_flag;
//volatile static uint8_t cnt= 0;
//volatile static uint8_t i2c_status= 0;//0-idle 1-receive 2-trastmate
//volatile uint8_t receive_size = 0,transmit_size = 0;
//I2C_HandleTypeDef i2c_test = {0};



I2C_HandleTypeDef i2c_test = {0};
I2C_HandleTypeDef i2cSlave = {0};
volatile uint8_t r_address_offset = 0,transmit_size = 0;
uint8_t  rx_count = 0;

/* i2c 的发送 接收 buffer */
uint8_t pData[DATA_LEN_MAX]={0};
uint8_t i2c_buffer[DATA_LEN_MAX]={0};

bool begin_trans = false;
uint8_t r_address = 0;
bool regaddr_flag = false;

#if 0
void iic_slave_init(void)
{
//	__HAL_RCC_I2C_CLK_ENABLE();
//	GPIO_InitTypeDef  gpioi2c={0};
//	gpioi2c.Pin    = GPIO_PIN_3;
//	gpioi2c.Mode = GPIO_MODE_AF;	// GPIO端口复用功能 
//	gpioi2c.Alternate = GPIO_AF4_I2C_SDA; // 配置为I2C_SDA
//	gpioi2c.OpenDrain = GPIO_OPENDRAIN; // 开漏输出
//	gpioi2c.Debounce.Enable = GPIO_DEBOUNCE_DISABLE; // 禁止输入去抖动
//	gpioi2c.SlewRate = GPIO_SLEW_RATE_LOW; // 电压转换速率
//	gpioi2c.DrvStrength = GPIO_DRV_STRENGTH_LOW; // 驱动强度
//	gpioi2c.Pull = GPIO_NOPULL; // 无上下拉
//	HAL_GPIO_Init(GPIOC, &gpioi2c);
//	
//	
//	gpioi2c.Pin = GPIO_PIN_4;
//	gpioi2c.Mode = GPIO_MODE_AF; 
//	gpioi2c.Alternate = GPIO_AF4_I2C_SCL;
//	gpioi2c.OpenDrain = GPIO_OPENDRAIN; 
//	gpioi2c.Debounce.Enable = GPIO_DEBOUNCE_DISABLE;
//	gpioi2c.SlewRate = GPIO_SLEW_RATE_LOW;
//	gpioi2c.DrvStrength = GPIO_DRV_STRENGTH_LOW;
//	gpioi2c.Pull = GPIO_NOPULL;	
//	HAL_GPIO_Init(GPIOC, &gpioi2c);
//	

//	i2c_test.Instance = I2C;
//	i2c_test.Init.master = I2C_MASTER_MODE_DISABLE;// 主机模式禁止
//	i2c_test.Init.slave = I2C_SLAVE_MODE_ENABLE; // 从机模式使能
//	i2c_test.Mode = HAL_I2C_MODE_SLAVE; // 从机模式
//	i2c_test.Init.slaveAddr = (I2C_DEVICE_ADDRESS>>1); // 从机地址

//	
//	i2c_test.Init.broadack = I2C_BROAD_ACK_DISABLE; // 广播地址应答禁止
//	i2c_test.Init.speedclock = I2C_SPEED_RATE; // I2C传输速率  
//	i2c_test.State = HAL_I2C_STATE_RESET; //I2C状态重置
//	
//	HAL_I2C_Init(&i2c_test);
//	HAL_NVIC_SetPriority(I2C_IRQn,I2C_TEST_IRQ_LEVEL);
//	HAL_NVIC_EnableIRQ(I2C_IRQn);
	

	GPIO_InitTypeDef  gpioi2c={0};
	gpioi2c.Pin    = I2C_SDA_PIN;
	gpioi2c.Mode = GPIO_MODE_AF; 
	gpioi2c.Alternate = GPIO_AF4_I2C_SDA;
	gpioi2c.OpenDrain = GPIO_OPENDRAIN; // 开漏
	gpioi2c.Debounce.Enable = GPIO_DEBOUNCE_DISABLE; 
	gpioi2c.SlewRate = GPIO_SLEW_RATE_HIGH; 
	gpioi2c.DrvStrength = GPIO_DRV_STRENGTH_HIGH; 
	gpioi2c.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(I2C_SDA_PORT, &gpioi2c);

	gpioi2c.Pin = I2C_SCL_PIN;
	gpioi2c.Mode = GPIO_MODE_AF;
	gpioi2c.Alternate = GPIO_AF4_I2C_SCL;
	gpioi2c.OpenDrain = GPIO_OPENDRAIN; 
	gpioi2c.Debounce.Enable = GPIO_DEBOUNCE_DISABLE;
	gpioi2c.SlewRate = GPIO_SLEW_RATE_HIGH;
	gpioi2c.DrvStrength = GPIO_DRV_STRENGTH_HIGH;
	gpioi2c.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(I2C_SCL_PORT, &gpioi2c);

	i2cSlave.Instance = I2C;
	i2cSlave.Init.master = I2C_MASTER_MODE_DISABLE;
	i2cSlave.Init.slave = I2C_SLAVE_MODE_ENABLE; 
	i2cSlave.Mode = HAL_I2C_MODE_SLAVE; 
	i2cSlave.Init.slaveAddr = I2C_DEVICE_ADDRESS; 

	i2cSlave.Init.broadack = I2C_BROAD_ACK_DISABLE; 
	i2cSlave.Init.speedclock = 4000000; 
	i2cSlave.State = HAL_I2C_STATE_RESET; 
	HAL_I2C_Init(&i2cSlave);
	
	
	HAL_NVIC_SetPriority(I2C_IRQn, 1);
    HAL_NVIC_EnableIRQ(I2C_IRQn);
	
}
#endif



void iic_slave_init(void)
{
	GPIO_InitTypeDef  gpioi2c={0};
	gpioi2c.Pin    = I2C_SDA_PIN;
	gpioi2c.Mode = GPIO_MODE_AF; 
	gpioi2c.Alternate = GPIO_AF4_I2C_SDA;
	gpioi2c.OpenDrain = GPIO_OPENDRAIN; // 开漏
	gpioi2c.Debounce.Enable = GPIO_DEBOUNCE_DISABLE; 
	gpioi2c.SlewRate = GPIO_SLEW_RATE_HIGH; 
	gpioi2c.DrvStrength = GPIO_DRV_STRENGTH_HIGH; 
	gpioi2c.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(I2C_SDA_PORT, &gpioi2c);

	gpioi2c.Pin = I2C_SCL_PIN;
	gpioi2c.Mode = GPIO_MODE_AF;
	gpioi2c.Alternate = GPIO_AF4_I2C_SCL;
	gpioi2c.OpenDrain = GPIO_OPENDRAIN; 
	gpioi2c.Debounce.Enable = GPIO_DEBOUNCE_DISABLE;
	gpioi2c.SlewRate = GPIO_SLEW_RATE_HIGH;
	gpioi2c.DrvStrength = GPIO_DRV_STRENGTH_HIGH;
	gpioi2c.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(I2C_SCL_PORT, &gpioi2c);

	i2cSlave.Instance = I2C;
	i2cSlave.Init.master = I2C_MASTER_MODE_DISABLE;
	i2cSlave.Init.slave = I2C_SLAVE_MODE_ENABLE; 
	i2cSlave.Mode = HAL_I2C_MODE_SLAVE; 
	i2cSlave.Init.slaveAddr = I2C_DEVICE_ADDRESS; 

	i2cSlave.Init.broadack = I2C_BROAD_ACK_DISABLE; 
	i2cSlave.Init.speedclock = 4000000; 
	i2cSlave.State = HAL_I2C_STATE_RESET; 
	HAL_I2C_Init(&i2cSlave);
	
	
	HAL_NVIC_SetPriority(I2C_IRQn, 1);
    HAL_NVIC_EnableIRQ(I2C_IRQn);
}


void HAL_I2C_SlaveCallback(I2C_HandleTypeDef *hi2c)
{
    uint32_t i2c_flag = 0xFF;
    HAL_NVIC_DisableIRQ(I2C_IRQn);
//    HAL_I2C_Wait_Flag(hi2c, &i2c_flag);
    switch(i2c_flag){
        case I2C_FLAG_SLAVE_RX_SLAW_ACK://60H
            rx_count = 0;
            r_address_offset = 0;
            begin_trans = true;
            regaddr_flag = true;
            transmit_size = 0;
            break;
        case I2C_FLAG_SLAVE_RX_SDATA_ACK:
            if(begin_trans == true && regaddr_flag == true)
            {
                // 第一次写入的值是 寄存器地址
                r_address = hi2c->Instance->DATA;
                // 寄存器溢出
                if(r_address > DATA_LEN_MAX){
                    r_address = 0;
                }
                r_address_offset = r_address;
                regaddr_flag = false;
            }else{
                // 第二个数据
                // 寄存器溢出
                if((r_address_offset + 1) > DATA_LEN_MAX){
                    r_address_offset = 0;
                }
                // pData 存储写入的数据
                pData[r_address_offset++] = hi2c->Instance->DATA; // 80H
                rx_count++;  // 每次接收到的数据个数
            }
            break;
        case I2C_FLAG_SLAVE_STOP_RESTART:
            begin_trans = false;
//            endtranmissing(r_address, rx_count);
            break;
        case I2C_FLAG_SLAVE_TX_SLAW_ACK: // A8H
            // 发送 pData register 处的寄存器
            if((r_address+transmit_size+1) > DATA_LEN_MAX){
                HAL_I2C_Send_Byte(hi2c,0);
            }else{
                HAL_I2C_Send_Byte(hi2c, pData[r_address + transmit_size++]);
            }
            break;
        case I2C_FLAG_SLAVE_TX_DATA_ACK: // B8H 
            if((r_address+transmit_size+1) > DATA_LEN_MAX){
                HAL_I2C_Send_Byte(hi2c,0);
            }else{
                HAL_I2C_Send_Byte(hi2c, pData[r_address + transmit_size++]);
            }
            break;
        default:
            break;
    }
    i2c_flag = 0XFF;
    HAL_I2C_ACK_Config(hi2c, ENABLE);
    HAL_I2C_Clear_Interrupt_Flag(hi2c);
    NVIC_ClearPendingIRQ(I2C_IRQn);
    HAL_NVIC_EnableIRQ(I2C_IRQn);
}




#if 0
void HAL_I2C_SlaveCallback(I2C_HandleTypeDef *hi2c)
{
	uint32_t i2c_flag = 0XFF;

	HAL_NVIC_DisableIRQ(I2C_IRQn);
	HAL_I2C_Wait_Flag(hi2c, &i2c_flag);
	switch(i2c_flag)
	{
		case I2C_FLAG_SLAVE_RX_SLAW_ACK:
				 receive_size = 0;
				 break;
		case I2C_FLAG_SLAVE_RX_SDATA_ACK:
				 pData[receive_size] = hi2c->Instance->DATA; // 80H 接收数据字节
				 i2c_RX_receive_data = 	pData[receive_size];
				 test_falg = TEST_FLAG_TRANSM;
				 receive_size++;
				 break;
		case I2C_FLAG_SLAVE_TX_SLAW_ACK: // A8H
				 transmit_size = 0;
				 HAL_I2C_Send_Byte(hi2c,pData[transmit_size]);  // 发送寄存器地址 即上次接收的第一个数据
				 test_falg = TEST_FLAG_RECIVE;	
				 i2c_TX_Send_data = pData[transmit_size];
				 transmit_size++;
				 break;
		case I2C_FLAG_SLAVE_TX_DATA_ACK: // B8H 
				 if(pData[0] == EEPROM_REGISTER1_ADDRESS)	
				 {
					 transmit_size = 1;
				 }
				 if(pData[0] == EEPROM_REGISTER2_ADDRESS)	
				 {
					 transmit_size = 2;
				 }
				 HAL_I2C_Send_Byte(hi2c,pData[transmit_size]); // 发送一个数据字节
				 test_falg = TEST_FLAG_RECIVE;	
				 i2c_TX_Send_data = pData[transmit_size];
				 transmit_size++;
				 break;
		default:
			break;
	}

	printf_i2c_flag = i2c_flag;
	i2c_flag = 0XFF;
	HAL_I2C_ACK_Config(hi2c, ENABLE);
	HAL_I2C_Clear_Interrupt_Flag(hi2c);
	NVIC_ClearPendingIRQ(I2C_IRQn);
	HAL_NVIC_EnableIRQ(I2C_IRQn);
		
}
#endif
