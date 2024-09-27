#include "DFRobot_I2C_Slave.h"

I2C_HandleTypeDef i2cSlave = {0};

volatile uint8_t r_address_offset = 0,transmit_size = 0;
uint8_t  rx_count = 0;

uint8_t i2c_RX_receive_data;
uint8_t i2c_TX_Send_data;

extern uint8_t int_gpio_start_flag;
extern uint8_t Addr0,Addr1;
extern volatile uint8_t select_pin;

/* i2c 的发送 接收 buffer */
uint8_t pData[DATA_LEN_MAX]={0};
uint8_t i2c_buffer[DATA_LEN_MAX]={0};

void i2cInitSlave(void)
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
	i2cSlave.Init.slaveAddr = I2C_DEVICE_ADDRESS|Addr0|(Addr1<<1); 
	
	i2cSlave.Init.broadack = I2C_BROAD_ACK_DISABLE; 
	i2cSlave.Init.speedclock = 4000000; 
	i2cSlave.State = HAL_I2C_STATE_RESET; 
	HAL_I2C_Init(&i2cSlave);
}

void i2cIRQConfig(void)
{
    HAL_NVIC_SetPriority(I2C_IRQn, 1);
    HAL_NVIC_EnableIRQ(I2C_IRQn);
}

bool begin_trans = false;
uint8_t r_address = 0;
bool regaddr_flag = false;

uint8_t i2c_interrupt = 0;

void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c) {  
    if (hi2c->Instance == I2C) {  
        // 关闭 I2C 的时钟  
        __HAL_RCC_I2C_CLK_DISABLE();  
        // 将 I2C 引脚去初始化，这里假设使用的是 PB6 和 PB7  
        HAL_GPIO_DeInit(I2C_SCL_PORT, I2C_SCL_PIN);  // SCL  
        HAL_GPIO_DeInit(I2C_SDA_PORT, I2C_SDA_PIN);  // SDA  
    }  
}

void HAL_I2C_SlaveCallback(I2C_HandleTypeDef *hi2c)
{
    uint32_t i2c_flag = 0xFF;
    HAL_NVIC_DisableIRQ(I2C_IRQn);
    HAL_I2C_Wait_Flag(hi2c, &i2c_flag);
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
            }else{ // 第二个数据
                // 寄存器溢出
                if((r_address_offset + 1) > DATA_LEN_MAX){
                    r_address_offset = 0;
                }
                // pData 存储写入的数据
                pData[r_address_offset++] = hi2c->Instance->DATA; // 80H
//				if(r_address_offset==r_address)
				select_pin=pData[CHANNEL_SELECT_ADDRESS];//写入通道选择
                rx_count++;  // 每次接收到的数据个数
//				begin_trans=true;
//				regaddr_flag = true;
            }
            break;
        case I2C_FLAG_SLAVE_STOP_RESTART:
            begin_trans = false;
            break;
        case I2C_FLAG_SLAVE_TX_SLAW_ACK: // A8H
            // 发送 pData register 处的寄存器
            if((r_address+transmit_size+1) > DATA_LEN_MAX){
                HAL_I2C_Send_Byte(hi2c,0);
            }else{
                HAL_I2C_Send_Byte(hi2c, i2c_buffer[r_address + transmit_size++]);
            }
            break;
        case I2C_FLAG_SLAVE_TX_DATA_ACK: // B8H 
            if((r_address+transmit_size+1) > DATA_LEN_MAX){
                HAL_I2C_Send_Byte(hi2c,0);
            }else{
                HAL_I2C_Send_Byte(hi2c, i2c_buffer[r_address + transmit_size++]);
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
