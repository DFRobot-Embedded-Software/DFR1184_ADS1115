/*!
 * @file  main.c
 * @brief  这是一个ADC模块（DFR1184）专用的固件
 * @details  cs32 用硬件 i2c 或 uart1 和主控通信;
 * @n  cs32 用软件 i2c 与 ads1115 通信
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license  The MIT License (MIT)
 * @author  [qsjhyy](yihuan.huang@dfrobot.com)
 * @version  V1.0
 * @date  2024-07-19
 */
 /* Includes ------------------------------------------------------------------*/
#include "main.h"	
#include "stdlib.h"
#include "string.h"

uint8_t ReceiveData[3];
uint8_t text[10];
bool select_pin=0;
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
	if((ReceiveData[0]==0xf0)&&(ReceiveData[2]==0x0f)){
		select_pin=ReceiveData[1];//1 A2  0 A1
		senddata_flag=1;
	}
	//bug如遇到非3字节命令会出现错误 后期可以通过单字节接收解决 
	HAL_UART_Receive_IT(&huart1, ReceiveData,sizeof(ReceiveData));		
}


int main(void)
{
	float   val;
	uint32_t   temp_val;
	static uint8_t last_mode=0;
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();		
    /* Configure the system clock to HIRC 24MHz*/
    SystemClock_Config();
	
	__HAL_RCC_GPIOC_CLK_ENABLE();	
	__HAL_RCC_I2C_CLK_ENABLE();
	key_init();	//按键初始化
	Ads1115_I2C_Init();//软件IIC的引脚初始化
	
  while (1)
	{	
		//改变地址后必须重新上电 	才能正常使用iic
		Addr1_last=Addr1;//尝试不断电改变iic地址，无效
		Addr0_last=Addr0;//尝试不断电改变iic地址，无效
		last_mode=mode_flag;
		
		//判断外部按键以改变地址
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5))Addr1=0;else	Addr1=1;// 1 Addr1=0
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4))Addr0=0;else	Addr0=1; //1 Addr0=0
		
		//若地址改变 重新初始化IIC
		if((Addr0_last!=Addr0)||(Addr1_last!=Addr1)){
			HAL_I2C_DeInit (&i2cSlave);
			__HAL_RCC_I2C_CLK_ENABLE();
			i2cInitSlave();i2cIRQConfig();
			HAL_Delay(10);
			mode_flag=0;}	//重新初始化IIC
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
		if(select_pin)//1 A2  0 A1
			val=ads1115_get_voltage_val(i2c_ads1115,0x01,CONFIG_REG_H|ADS1115_REG_CONFIG_MUX_SINGLE_2,CONFIG_REG_L);
		else
			val=ads1115_get_voltage_val(i2c_ads1115,0x01,CONFIG_REG_H|ADS1115_REG_CONFIG_MUX_SINGLE_1,CONFIG_REG_L);
		//取到mv后两位，避免浮点数
		temp_val=val*100;
		//通过uart或者iic发送电压值
		if((mode_flag==1)&&(senddata_flag==1)){
			sprintf((char*)text,"%d\r\n",temp_val);
			HAL_UART_Transmit_IT(&huart1, text,strlen((char*)text));
			senddata_flag=0;
		}		
		else if(mode_flag==0){//iic
			i2c_buffer[CHANNEL_DATA_ADDRESS+1]=temp_val>>16;//共需要3字节传输
			i2c_buffer[CHANNEL_DATA_ADDRESS+2]=temp_val>>8;//共需要3字节传输
			i2c_buffer[CHANNEL_DATA_ADDRESS+3]=temp_val>>0;//共需要3字节传输
			for(int i=0;i<6;i++ ){	
				temp_val=temp_val/10;
				if(temp_val<=0){
					i2c_buffer[CHANNEL_DATA_ADDRESS]=i+1;//发共有多少数据位	最大六位			
					break;
				}	   
			}
		} 		
		HAL_Delay(100);
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



