/*!
 * @file  main.c
 * @brief  这是一个ADC模块（DFR1184）专用的固件
 * @details  cs32 用硬件 i2c 或 uart1 和主控通信;
 * @n  cs32 用软件 i2c 与 ads1115 通信
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license  The MIT License (MIT)
 * @author  [lr]
 * @version  V1.0
 * @date  2024-07-19
 */
 /* Includes ------------------------------------------------------------------*/
#include "main.h"	
#include "stdlib.h"
#include "string.h"

#define UART_MAX_LEN        128
#define UART_READ_REGBUF    0xBB
#define UART_WRITE_REGBUF   0xCC
uint8_t ReceiveData;
uint8_t ReceiveBuffer[UART_MAX_LEN]= { 0 };
uint8_t regBuf[DATA_LEN_MAX] = { 0 };
uint8_t text[10];
uint8_t uart_buffer[5];
volatile uint8_t select_pin=0;//bug初始化为0不工作？
uint8_t ReceiveCount=0;
uint8_t ReceiveHandleCount=0;
uint8_t SendBuffer[DATA_LEN_MAX]={0};
uint32_t  adval;
bool senddata_flag=0;
bool addr_change_flag=0;
uint8_t Addr0=0,Addr1=0;
uint8_t Addr0_last=0,Addr1_last=0;
extern UART_HandleTypeDef huart1;

/**
 * @brief 按键初始化函数
 * @param void
 * @return void
 */
void key_init(void){
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitTypeDef gpio_key;
	gpio_key.Pin    = GPIO_PIN_4|GPIO_PIN_5;
	gpio_key.Mode	=	GPIO_MODE_INPUT;
	gpio_key.Debounce.Enable = GPIO_DEBOUNCE_ENABLE; 
	gpio_key.SlewRate = GPIO_SLEW_RATE_HIGH; 
	HAL_GPIO_Init(GPIOB, &gpio_key);
	gpio_key.Pin =GPIO_PIN_3;
	HAL_GPIO_Init(GPIOA, &gpio_key);
}

/**
 * @brief 串口回调函数
 * @param huart
 * @return void
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(&huart1 == huart){
		if(ReceiveCount>=UART_MAX_LEN){
			ReceiveCount%=UART_MAX_LEN;
		}
		ReceiveBuffer[ReceiveCount++]=ReceiveData;	
		HAL_UART_Receive_IT(&huart1, &ReceiveData,1);
	}	
	HAL_UART_Receive_IT(&huart1, &ReceiveData,1);	
}
/**
 * @brief 串口数据解析
 * @details 串口数据分为如下读写两类:
 * @n 写操作 :
 * @n 发送 : UART0_WRITE_REGBUF + 寄存器地址 + 写入数据长度 + 对应长度的数据字节
 * @n 读操作 :
 * @n 如果是读取RTC模块的数据，需要先将(寄存器地址 + 读取数据长度) 写入(通过写操作) (REG_RTC_READ_REG + REG_RTC_READ_LEN)，以更新对应寄存器数据
 * @n 发送 : UART0_READ_REGBUF + 寄存器地址 + 读取数据长度 ; 接收 : 读取长度的字节
 */
void uart_command(void)
{
	if((ReceiveCount-ReceiveHandleCount)>=3){
		uint8_t* data = &ReceiveBuffer[ReceiveHandleCount];
		uint8_t type = data[0];
		uint8_t reg = data[1];
		uint8_t len = data[2];
		ReceiveHandleCount+=3;	
//		HAL_UART_Transmit_IT(&huart1, &ReceiveBuffer[ReceiveHandleCount-3],4);
		switch (type)
		{
			case UART_WRITE_REGBUF:
				if((ReceiveCount-ReceiveHandleCount)>=len){
					memcpy(&regBuf[reg], &data[3], len);
					if(reg==CHANNEL_SELECT_ADDRESS)
						select_pin=regBuf[reg];
//					HAL_UART_Transmit_IT(&huart1, &select_pin,1);
					ReceiveHandleCount += len;
				}
				break;
			case UART_READ_REGBUF:
				
				if(reg==CHANNEL_DATA_ADDRESS){
					senddata_flag=1;
				}
				break;
			default:		//数据错误从新接收
				ReceiveCount = 0;
				ReceiveHandleCount = 0;
				break;
		}	
		if (ReceiveCount == ReceiveHandleCount) {  
			ReceiveCount = 0;
			ReceiveHandleCount = 0;
	  }		
//		HAL_UART_Transmit_IT(&huart1, &ReceiveHandleCount,1);
	}
}



int main(void)
{
	static uint16_t uartTimer=0;
	static uint8_t last_mode=0;
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();		
    /* Configure the system clock to HIRC 24MHz*/
    SystemClock_Config();
	
	__HAL_RCC_GPIOC_CLK_ENABLE();	
	__HAL_RCC_I2C_CLK_ENABLE();
	key_init();	//按键初始化
	Ads1115_I2C_Init();//读ads1115的软件IIC引脚初始化
	
  while (1)
	{	
		//改变地址后必须重新上电 	才能正常使用iic
		Addr1_last=Addr1;
		Addr0_last=Addr0;
		last_mode=mode_flag;
		float   val=0;
		//判断外部按键以改变地址
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5))Addr1=0;else	Addr1=1;// 1 Addr1=0
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4))Addr0=0;else	Addr0=1; //1 Addr0=0
		
		//尝试不断电改变iic地址，无效
		if((Addr0_last!=Addr0)||(Addr1_last!=Addr1)){
			HAL_I2C_DeInit (&i2cSlave);
			__HAL_RCC_I2C_CLK_ENABLE();
			i2cInitSlave();i2cIRQConfig();
			HAL_Delay(10);
			mode_flag=0;
		}
		//判断按键以确定是使用iic还是uart
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3))
			mode_flag=1;   //1 uart 0 iic
		else 
			mode_flag=0;
		//模式状态改变 重新初始化iic或者uart
		if(last_mode!=mode_flag){
			if(mode_flag==1)LogInit();
			else if(mode_flag==0){i2cInitSlave();i2cIRQConfig();}
		}
		//根据收到的命令决定是采集A1 还是A2通道的数据 函数内部在发送修改指令后10ms后才读数据
		if(select_pin==2)//2 A2  1 A1
			val=ads1115_get_voltage_val(i2c_ads1115,0x01,CONFIG_REG_H|ADS1115_REG_CONFIG_MUX_SINGLE_2,CONFIG_REG_L);
		else if(select_pin==1)
			val=ads1115_get_voltage_val(i2c_ads1115,0x01,CONFIG_REG_H|ADS1115_REG_CONFIG_MUX_SINGLE_1,CONFIG_REG_L);
		//取到mv后两位，避免浮点数
		adval=val*100;
		//串口数据解析 
		HAL_Delay(10);//大于10ms解析一次 避免串口数据没发完就解析
		uart_command();
		
		//通过uart或者iic发送电压值
		if((mode_flag==1)&&(senddata_flag==1)){
			uart_buffer[0]=adval>>16;//共需要3字节传输
			uart_buffer[1]=adval>>8;//共需要3字节传输
			uart_buffer[2]=adval>>0;//共需要3字节传输		
			HAL_UART_Transmit_IT(&huart1, uart_buffer,3);
			senddata_flag=0;
		}		
		else if(mode_flag==0){//iic
			i2c_buffer[CHANNEL_DATA_ADDRESS]=adval>>16;//共需要3字节传输
			i2c_buffer[CHANNEL_DATA_ADDRESS+1]=adval>>8;//共需要3字节传输
			i2c_buffer[CHANNEL_DATA_ADDRESS+2]=adval>>0;//共需要3字节传输   
		} 		
	}
}



/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};	
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HIRC;
  RCC_OscInitStruct.HIRCState = RCC_HIRC_ON;
  RCC_OscInitStruct.HIRCCalibrationValue = RCC_HIRCCALIBRATION_24M;
	
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
	
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HIRC;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APBCLKDivider = RCC_PCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }	
}
 

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */


/* Private function -------------------------------------------------------*/



