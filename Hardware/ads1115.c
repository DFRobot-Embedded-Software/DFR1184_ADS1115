#include "ads1115.h"	


int16_t ads1115_read_data(void);
/**
 * @brief IIC延时
 * @param  无
 * @return 无
 */
void IIC_Delay(uint8_t time)
{
    uint32_t i = time * 5; // 用户根据自己的MCU时钟进行设置倍数 //*240*20 =ms

    while (i--)
    {
        ;
    }
}

/**
 * @brief IIC初始化
 * @param  无
 * @return 无
 */
void Soft_IIC_Init(void)
{
    IIC_SDA_H();
    IIC_SCL_H();
}

/**
 * @brief SDA引脚设置输出模式
 * @param  无
 * @return 无
 */
static void Soft_IIC_Output(void)
{

	GPIO_InitTypeDef SOFT_IIC_GPIO_STRUCT;
    SOFT_IIC_GPIO_STRUCT.Mode = GPIO_MODE_OUTPUT;
    SOFT_IIC_GPIO_STRUCT.Pin = IIC_SDA_PIN;
	SOFT_IIC_GPIO_STRUCT.OpenDrain = GPIO_OPENDRAIN; 
    SOFT_IIC_GPIO_STRUCT.SlewRate = GPIO_SLEW_RATE_HIGH; // 电压转换速率
	SOFT_IIC_GPIO_STRUCT.Debounce.Enable = GPIO_DEBOUNCE_DISABLE; // 禁止输入去抖动
	SOFT_IIC_GPIO_STRUCT.DrvStrength = GPIO_DRV_STRENGTH_HIGH; // 驱动强度
	SOFT_IIC_GPIO_STRUCT.Pull = GPIO_NOPULL;			// 上拉
	
    HAL_GPIO_Init(IIC_SDA_PORT, &SOFT_IIC_GPIO_STRUCT);
		
}

/**
 * @brief SDA引脚设置输入模式
 * @param  无
 * @return 无
 */
static void Soft_IIC_Input(void)
{
    GPIO_InitTypeDef SOFT_IIC_GPIO_STRUCT;
    SOFT_IIC_GPIO_STRUCT.Mode = GPIO_MODE_INPUT;
    SOFT_IIC_GPIO_STRUCT.Pin = IIC_SDA_PIN;
	SOFT_IIC_GPIO_STRUCT.OpenDrain = GPIO_OPENDRAIN; //开漏
    SOFT_IIC_GPIO_STRUCT.SlewRate = GPIO_SLEW_RATE_HIGH; // 电压转换速率
	SOFT_IIC_GPIO_STRUCT.Debounce.Enable = GPIO_DEBOUNCE_DISABLE; // 禁止输入去抖动
	SOFT_IIC_GPIO_STRUCT.DrvStrength = GPIO_DRV_STRENGTH_HIGH; // 驱动强度
	SOFT_IIC_GPIO_STRUCT.Pull = GPIO_NOPULL;			// 
	
    HAL_GPIO_Init(IIC_SDA_PORT, &SOFT_IIC_GPIO_STRUCT);
	

	
}

/**
 * @brief IIC起始信号
 * @param  无
 * @return 无
 */
void Soft_IIC_Start(void)
{
    Soft_IIC_Output();
    IIC_SCL_L();
    IIC_SDA_H();
    IIC_SCL_H();
    IIC_Delay(IIC_DELAY_TIME);
    IIC_SDA_L();
    IIC_Delay(IIC_DELAY_TIME);
    IIC_SCL_L();
}

/**
 * @brief IIC停止信号
 * @param  无
 * @return 无
 */
void Soft_IIC_Stop(void)
{
    Soft_IIC_Output();
    IIC_SCL_L();
    IIC_SDA_L();
    IIC_SCL_H();
    IIC_Delay(IIC_DELAY_TIME);
    IIC_SDA_H();
    IIC_Delay(IIC_DELAY_TIME);
}

/**
 * @brief IIC应答信号
 * @param  无
 * @return 无
 */
void Soft_IIC_ACK(void)
{
    Soft_IIC_Output();
    IIC_SCL_L();
    IIC_SDA_L();
    IIC_Delay(IIC_DELAY_TIME);
    IIC_SCL_H();
    IIC_Delay(IIC_DELAY_TIME);
    IIC_SCL_L();
}

/**
 * @brief IIC无应答信号
 * @param  无
 * @return 无
 */
void Soft_IIC_NACK(void)
{
	Soft_IIC_Output();
    IIC_SCL_L();
    IIC_SDA_H();
    IIC_Delay(IIC_DELAY_TIME);
    IIC_SCL_H();
    IIC_Delay(IIC_DELAY_TIME);
	IIC_SCL_L();
	
}

/**
 * @brief IIC等待应答信号
 * @param  无
 * @return 0无应答  1有应答
 */
uint8_t Soft_IIC_Wait_ACK(void)
{
    uint8_t wait=0;
    Soft_IIC_Output();
    IIC_SDA_H();
    Soft_IIC_Input();
    IIC_SCL_H();
    IIC_Delay(IIC_DELAY_TIME);
    while (HAL_GPIO_ReadPin(IIC_SDA_PORT, IIC_SDA_PIN))
    {
        wait++;
        if (wait > 200)//超时判断
        {
//            Soft_IIC_Stop();
            return 0;
        }
    }
    IIC_SCL_L();
    return 1;
}

/**
 * @brief IIC写数据1
 * @param  无
 * @return 无
 */
void Soft_IIC_Write_High(void)
{
    IIC_SCL_L();
    IIC_SDA_H();
    IIC_Delay(IIC_DELAY_TIME);
    IIC_SCL_H();
    IIC_Delay(IIC_DELAY_TIME);
    IIC_SCL_L();
}

/**
 * @brief IIC写数据0
 * @param  无
 * @return 无
 */
void Soft_IIC_Write_Low(void)
{
    IIC_SCL_L();
    IIC_SDA_L();
    IIC_Delay(IIC_DELAY_TIME);
    IIC_SCL_H();
    IIC_Delay(IIC_DELAY_TIME);
    IIC_SCL_L();
}

/**
 * @brief IIC写入单个数据
 * @param  无
 * @return 应答信号, 0无应答 1有应答
 */
uint8_t Soft_IIC_Write_Byte(uint8_t Byte)
{
    uint8_t i;
    Soft_IIC_Output();
    for (i = 0x80; i != 0; i >>= 1)
    {
        if (Byte & i)
        {
            Soft_IIC_Write_High();
        }
        else
        {
            Soft_IIC_Write_Low();
        }
    }
	return 0;
    //return (Soft_IIC_Wait_ACK());
}

/**
 * @brief IIC读一个数据
 * @param  ACK:应答 NACK:不应答
 * @return 返回读到的数据
 */
uint8_t Soft_IIC_Recv_Byte(ACK_STATUS ack_sta)
{
    uint8_t data = 0, i;
    Soft_IIC_Input();
    IIC_Delay(IIC_DELAY_TIME);
    for (i = 0x80; i != 0; i >>= 1)
    {
		IIC_SCL_L();
		IIC_Delay(IIC_DELAY_TIME);
		IIC_SCL_H();
		IIC_Delay(IIC_DELAY_TIME);
		if (HAL_GPIO_ReadPin(IIC_SDA_PORT, IIC_SDA_PIN) == 1)
        {
            data |= i;
        }	
    }
    if (ack_sta == ACK)
    {
        Soft_IIC_ACK();
    }
    else
    {
        Soft_IIC_NACK();
    }
    return data;
}



void IIC_Write(uint8_t addr ,uint8_t reg ,uint8_t * buf ,uint8_t size)
{
	#if 1
	uint8_t i = 0;
	Soft_IIC_Start();    																					// iic begin
	Soft_IIC_Write_Byte(addr );
	if(Soft_IIC_Wait_ACK()==0)
		return ;

	Soft_IIC_Write_Byte(reg );
	if(Soft_IIC_Wait_ACK()==0)
		return ;
	
	for(i = 0; i < size; i++){																		// 写数据
		Soft_IIC_Write_Byte(buf[i]);
		if(Soft_IIC_Wait_ACK()==0)
			return ;
	}
		
	Soft_IIC_Stop();																							// iic end
	IIC_Delay(10);
	#endif	
}


uint8_t IIC_Read(uint8_t addr ,uint8_t reg ,uint8_t * buf ,uint8_t size)
{	 
	uint8_t i = 0;
	memset((void *)buf,(int)0,(size_t)size);														// 清空传进来的buffer

	Soft_IIC_Start();    																					// iic begin	
	Soft_IIC_Write_Byte(addr );
	while(Soft_IIC_Wait_ACK()){
		Soft_IIC_Write_Byte(addr); 															// iic 地址
	}
	Soft_IIC_Write_Byte(reg);
	while(Soft_IIC_Wait_ACK()){																		// 寄存器地址
		Soft_IIC_Write_Byte(reg);
	}
	Soft_IIC_Stop();																							// 定位到指定iic
	
	Soft_IIC_Start();
	Soft_IIC_Write_Byte((addr ) | 0x01);															// 读数据
	while(Soft_IIC_Wait_ACK()){
		Soft_IIC_Write_Byte((addr ) | 0x01);
	}
	if(size == 1){															// 只读一个数据
		buf[size-1] = Soft_IIC_Recv_Byte(NACK);
	}else{																			// 读大于一个数据
		for(i = 0; i < size-1; i++){
			buf[i] = Soft_IIC_Recv_Byte(ACK);
		}
		buf[size-1] = Soft_IIC_Recv_Byte(NACK);
	}
	Soft_IIC_Stop();	
	IIC_Delay(10);
	
	return buf[0];
}


void Ads1115_I2C_Init(void)
{	
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	GPIO_InitTypeDef  gpioi2c={0};
	gpioi2c.Pin    = IIC_SCL_PIN;
	gpioi2c.Mode = GPIO_MODE_OUTPUT; 
	gpioi2c.OpenDrain = GPIO_OPENDRAIN;// 开漏输出
	gpioi2c.Debounce.Enable = GPIO_DEBOUNCE_DISABLE; // 禁止输入去抖动
	gpioi2c.SlewRate = GPIO_SLEW_RATE_HIGH; // 电压转换速率
	gpioi2c.DrvStrength = GPIO_DRV_STRENGTH_HIGH; // 驱动强度
	gpioi2c.Pull = GPIO_NOPULL;			
	HAL_GPIO_Init(IIC_SCL_PORT, &gpioi2c);
	
	
	gpioi2c.Pin = IIC_SDA_PIN;
	gpioi2c.Mode = GPIO_MODE_OUTPUT;
	gpioi2c.OpenDrain = GPIO_OPENDRAIN; // 开漏输出
	gpioi2c.Debounce.Enable = GPIO_DEBOUNCE_DISABLE;
	gpioi2c.SlewRate = GPIO_SLEW_RATE_HIGH;
	gpioi2c.DrvStrength = GPIO_DRV_STRENGTH_HIGH;
	gpioi2c.Pull = GPIO_NOPULL;	
	HAL_GPIO_Init(IIC_SDA_PORT, &gpioi2c);
	
}




void ads1115_config_register(uint8_t pointADD,uint8_t configH,uint8_t configL)
{
    uint8_t reg_data[2]={configH,configL};	
	IIC_Write(ADS1115_WRITE_ADDRESS,pointADD, reg_data, 2);
}
int16_t ads1115_read_data(void)
{
    int16_t data;
    uint8_t rx_data[2]={0};
	
	Soft_IIC_Start();    																					// iic begin
	Soft_IIC_Write_Byte(ADS1115_WRITE_ADDRESS);
	if(Soft_IIC_Wait_ACK()==0)
		return 0;
	Soft_IIC_Write_Byte(0x00);
	if(Soft_IIC_Wait_ACK()==0)
		return 0;
	Soft_IIC_Stop();// iic end	

	Soft_IIC_Start();   
	Soft_IIC_Write_Byte(ADS1115_READ_ADDRESS);
	if(Soft_IIC_Wait_ACK()==0)
		return 0;
	rx_data[0]=Soft_IIC_Recv_Byte(ACK);
	rx_data[1]=Soft_IIC_Recv_Byte(NACK);
	Soft_IIC_Stop();  
	
	data=rx_data[0]*256+rx_data[1];
    return data;
	
}
 
double ads1115_get_voltage_val(uint8_t pointADD,uint8_t configH,uint8_t configL)
{
    double val;
    int16_t ad_val;
 
    ads1115_config_register(pointADD,configH,configL);//01 D4 83
    HAL_Delay(20);//必须大于这个20ms才能保证读到数据的准确性
    ad_val=ads1115_read_data();
    if((ad_val==0x7FFF)|(ad_val==-0x7FFF))// 判断是否超量程了
    {
        ad_val=0;
//        if(mode_flag==1)printf("over!\r\n");
    }
 
    switch((0x0E&configH)>>1)//量程对应的分辨率
    {
        case(0x00):
            val=(double)ad_val/187.5/1000000.0;//	即187.5/1000000.0
        break;
        case(0x01):
            val=(double)ad_val*125/1000000.0;
        break;
        case(0x02):
            val=(double)ad_val*62.5/1000.0*5.0;//加了运放 
        break;
        case(0x03):
            val=(double)ad_val*31.25/1000000.0;
        break;
        case(0x04):
            val=(double)ad_val*15.625/1000000.0;
        break;
        case(0x05):
            val=(double)ad_val*7.8125/1000000.0;
        break;
    }
	if(val<=0)val=0;
    return val;
}

