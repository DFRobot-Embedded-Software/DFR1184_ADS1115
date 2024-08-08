/*!
 * @file  main.c
 * @brief  这是一个ADC模块（DFR1184）专用的固件
 * @details  cs32 用硬件 i2c 或 uart1 和主控通信;
 * @n  cs32 用软件 i2c 与 ads1115 通信
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license  The MIT License (MIT)
 * @author  [lr]
 * @version  V0.1
 * @date  2024-07-19
 */
 /* Includes ------------------------------------------------------------------*/
#include "main.h"	
#include "stdlib.h"
#include "string.h"

#define KEY_A0_PORT GPIOB
#define KEY_A0_PIN  GPIO_PIN_4
#define KEY_A1_PORT GPIOB
#define KEY_A1_PIN  GPIO_PIN_5
#define KEY_SELECT_PORT GPIOA
#define KEY_SELECT_PIN  GPIO_PIN_3


void SystemClock_Config(void);
uint8_t text[10]={0};
uint8_t uart_buffer[5]={0};
uint8_t Addr0=0,Addr1=0;
uint8_t Addr0_last=0,Addr1_last=0;
uint8_t mode_flag=2;//1为uart 0为iic 2为初始值
uint32_t  adval=0;
volatile uint8_t select_pin=0;
//bool senddata_flag=0;
bool addr_change_flag=0;

extern UART_HandleTypeDef huart1;
extern uint8_t ReceiveData;
extern I2C_HandleTypeDef i2cSlave ;
/**
 * @brief 按键初始化函数
 * @param void
 * @return void
 */
void key_init(void){
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitTypeDef gpio_key;
	gpio_key.Pin    = KEY_A0_PIN|KEY_A1_PIN;
	gpio_key.Mode	=	GPIO_MODE_INPUT;
	gpio_key.Debounce.Enable = GPIO_DEBOUNCE_ENABLE; 
	gpio_key.SlewRate = GPIO_SLEW_RATE_HIGH; 
	HAL_GPIO_Init(KEY_A0_PORT, &gpio_key);
	gpio_key.Pin =KEY_SELECT_PIN;//选择引脚
	HAL_GPIO_Init(KEY_SELECT_PORT, &gpio_key);
}

int main(void)
{
	
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();		
    /* Configure the system clock to HIRC 24MHz*/
    SystemClock_Config();
	
	__HAL_RCC_GPIOC_CLK_ENABLE();	
	__HAL_RCC_I2C_CLK_ENABLE();
	key_init();	//按键初始化
	Ads1115_I2C_Init();//读ads1115的软件IIC引脚初始化
	SerialAnalysisTimerInit();// /* uart解析任务的定时器初始化 */
	//拨码以改变地址 改变地址后必须重新上电 才能改变地址
	HAL_Delay(100);
	Addr0 =HAL_GPIO_ReadPin(KEY_A0_PORT,KEY_A0_PIN)? 0 : 1;
	Addr1 =HAL_GPIO_ReadPin(KEY_A1_PORT,KEY_A1_PIN)? 0 : 1;
	mode_flag=HAL_GPIO_ReadPin(KEY_SELECT_PORT,KEY_SELECT_PIN)?1:0;
	if(mode_flag==1){ 
		SerialInit(9600);
	}else if(mode_flag==0){
		i2cInitSlave();
		i2cIRQConfig();
	}
	
	while (1){	
		
		#if 0	//不支持热切换模式
		static uint8_t last_mode=0;
		last_mode=mode_flag;//模式记录 
		//判断按键以确定是使用iic还是uart
		mode_flag=HAL_GPIO_ReadPin(KEY_SELECT_PORT,KEY_SELECT_PIN)?1:0;
		if(last_mode!=mode_flag){	//模式状态改变 重新初始化iic或者uart
			if(mode_flag==1){ 
				HAL_I2C_DeInit(&i2cSlave);  // 调用 HAL 库的去初始化函数  
				HAL_I2C_MspDeInit(&i2cSlave);//调用 MSP 去初始化函数  
				memset(uart_buffer,0,sizeof(uart_buffer));//清空数据块
				SerialInit(9600);
			}	
			else if(mode_flag==0){
				UART_DeInit();
				AT_flag=0;
				senddata_flag=0;
				memset(uart_buffer,0,sizeof(i2c_buffer));//清空数据块
				i2cInitSlave();
				i2cIRQConfig();
			}
		}
		#endif
				
		if ((cs32TimerFlag > 5)&&(mode_flag==1)) {// 连续5ms没有收到数据认为接收结束 解析一次串口数据 此处可确保接收完数据才开始解析
			uart_command();
			cs32TimerFlag = 0;
		}
		
		float   val=0;	
		if(select_pin==1)//判断是采集A1还是A2通道数据
			val=ads1115_get_voltage_val(ADS1015_REG_POINTER_CONFIG,CONFIG_REG_H|ADS1115_REG_CONFIG_MUX_SINGLE_1,CONFIG_REG_L);
		else if(select_pin==2)
			val=ads1115_get_voltage_val(ADS1015_REG_POINTER_CONFIG,CONFIG_REG_H|ADS1115_REG_CONFIG_MUX_SINGLE_2,CONFIG_REG_L);
		adval=val*100;//取到mv后两位，避免浮点数
		
 	
		i2c_buffer[CHANNEL_DATA_ADDRESS]=adval>>16;//共需要3字节传输
		i2c_buffer[CHANNEL_DATA_ADDRESS+1]=adval>>8;//共需要3字节传输
		i2c_buffer[CHANNEL_DATA_ADDRESS+2]=adval>>0;//共需要3字节传输
		
		uart_buffer[0]=adval>>16;//共需要3字节传输
		uart_buffer[1]=adval>>8;//共需要3字节传输
		uart_buffer[2]=adval>>0;//共需要3字节传输
		
		static uint8_t cmd_sendbuf[30]={0};//容易被释放 所以加了static
		if((mode_flag==1)&&(AT_flag!=0)){
			sprintf((char*)cmd_sendbuf,"AT+AIN%d=%4.2fmv\r\n",AT_flag,val);
			AT_flag=0;
			HAL_UART_Transmit_IT(&huart1, cmd_sendbuf,strlen((char*)cmd_sendbuf));
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



