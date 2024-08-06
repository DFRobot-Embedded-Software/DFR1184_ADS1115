#include "uart1.h"	
#include <string.h>
#define CHANNEL_SELECT_ADDRESS  0x20 //选择ad通道寄存器地址
#define UART_MAX_LEN        128		//最大储存长度
#define UART_READ_REGBUF    0xBB	//串口读模拟寄存器地址
#define UART_WRITE_REGBUF   0xCC	///串口写模拟寄存器地址
#define CHANNEL_DATA_ADDRESS  0x31 //ad数据地址
#define AT_CMD    'A'	


UART_HandleTypeDef huart1 = {0};
uint8_t regBuf[UART_MAX_LEN] = { 0 };
uint8_t SendBuffer[UART_MAX_LEN]={0};

uint8_t AT_flag=0;
uint8_t ReceiveData=0;
uint8_t ReceiveCount=0;
uint8_t ReceiveHandleCount=0;
uint8_t ReceiveBuffer[UART_MAX_LEN]= { 0 };

volatile uint8_t cs32TimerFlag=0;
extern void Error_Handler(void);
extern volatile uint8_t select_pin;
//extern bool senddata_flag;

void SerialInit(uint32_t baud_rate)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* Peripheral clock enable */
    __HAL_RCC_UART1_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
	
    GPIO_InitStruct.Pin = UART1_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF;
	GPIO_InitStruct.OpenDrain = GPIO_PUSHPULL;	
	GPIO_InitStruct.Debounce.Enable = GPIO_DEBOUNCE_DISABLE;
	GPIO_InitStruct.SlewRate = GPIO_SLEW_RATE_HIGH;
	GPIO_InitStruct.DrvStrength = GPIO_DRV_STRENGTH_HIGH;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Alternate = GPIO_AF5_UART1_TXD;
    HAL_GPIO_Init(UART1_TX_PORT, &GPIO_InitStruct);
		
    GPIO_InitStruct.Pin = UART1_RX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF;
	GPIO_InitStruct.Alternate = GPIO_AF5_UART1_RXD;
    HAL_GPIO_Init(UART1_RX_PORT, &GPIO_InitStruct);		
	
	
	huart1.Instance = UART1;
	huart1.Init.BaudRate = baud_rate;
	huart1.Init.BaudDouble = UART_BAUDDOUBLE_ENABLE;	
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	
	HAL_NVIC_SetPriority(UART1_IRQn,3);
	HAL_UART_Init(&huart1);

	HAL_UART_Receive_IT(&huart1, &ReceiveData, sizeof(ReceiveData));	
	HAL_NVIC_EnableIRQ(UART1_IRQn); // 中断使能
}


BASETIM_HandleTypeDef sBaseTimHandle = {0};
void SerialAnalysisTimerInit(void)
{
  __HAL_RCC_BASETIM_CLK_ENABLE();

  // 系统时钟为24M,且该定时器只能向上计数
  // 24 000 / 24 000 000 = 1 / 1000 s = 1 ms
  sBaseTimHandle.Instance = TIM11;                                 // TIM11
  sBaseTimHandle.Init.CntTimSel = BASETIM_TIMER_SELECT;            // 选择为定时器功能
  sBaseTimHandle.Init.AutoReload = BASETIM_AUTORELOAD_ENABLE;      // 使能自动重装载
  sBaseTimHandle.Init.MaxCntLevel = BASETIM_MAXCNTLEVEL_16BIT;     // 设置计数器的最大计数值为16bit
  sBaseTimHandle.Init.OneShot = BASETIM_REPEAT_MODE;               // 计数器运行模式为重复模式
  sBaseTimHandle.Init.Prescaler = BASETIM_PRESCALER_DIV64;         // 预分频系数为64, 24000 计数值为 64ms
  sBaseTimHandle.Init.Period = BASETIM_MAXCNTVALUE_16BIT - 24000 / 64;  //设置计数器周期装载值, 延时1ms (多一个计数值的影响)

  if (HAL_BASETIM_Base_Init(&sBaseTimHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  HAL_NVIC_EnableIRQ(TIM11_IRQn);

  HAL_NVIC_SetPriority(TIM11_IRQn, 0);

  if (HAL_BASETIM_Base_Start_IT(&sBaseTimHandle) != HAL_OK)
  {
    /* Starting Error */
    Error_Handler();
  }
   cs32TimerFlag = 0;
}
/**
  * @brief  Period elapsed callback in non blocking mode 
  * @param  htim : BASETIM handle
  * @retval None
  */
void HAL_BASETIM_PeriodElapsedCallback(BASETIM_HandleTypeDef *htim)
{
  cs32TimerFlag++;	
}


//释放串口
void UART_DeInit() {   
    // 关闭 UART 外设  
    HAL_UART_DeInit(&huart1);  
    // 如果需要，可以调用 MSP 去初始化函数，通常用于释放占用的资源  
    HAL_UART_MspDeInit(&huart1);  
}  

// MSP DeInit 函数，释放相关 GPIO 和时钟资源  
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart) {  
    if (huart->Instance == UART1) {  
        // 关闭 USART1 的时钟  
        __HAL_RCC_UART1_CLK_DISABLE();  
        // DeInit GPIO 引脚
        HAL_GPIO_DeInit(UART1_TX_PORT, UART1_TX_PIN);  // TX  
        HAL_GPIO_DeInit(UART1_RX_PORT, UART1_RX_PIN); // RX  	
    }  
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
			ReceiveCount%=UART_MAX_LEN;  //接收到的数据先放缓存区，后面统一处理
		}
		cs32TimerFlag = 0;//收到一个字节就清时间计数
		ReceiveBuffer[ReceiveCount++]=ReceiveData;	
//		HAL_UART_Transmit(&huart1, ReceiveData, 1, 10);  // 阻塞发送
		HAL_UART_Receive_IT(&huart1, &ReceiveData,1);
		
	}	
}
/**
 * @brief 串口数据解析
 * @details 串口数据分为如下读写两类:
 * @n 写操作 :UART_WRITE_REGBUF + 寄存器地址 + 写入数据长度 + 数据
 * @n 读操作 :UART_READ_REGBUF + 寄存器地址+读取数据长度
 */
extern uint8_t uart_buffer[5];

void uart_command(void)
{
	static uint8_t buf[20]={0};
	if((ReceiveCount-ReceiveHandleCount)>=3){
		uint8_t* data = &ReceiveBuffer[ReceiveHandleCount];
		uint8_t type = data[0];
		uint8_t reg = data[1];
		uint8_t len = data[2];
//		ReceiveHandleCount+=3;	
		switch (type){
			case UART_WRITE_REGBUF:
				ReceiveHandleCount+=3;	
				if((ReceiveCount-ReceiveHandleCount)>=len){
					memcpy(&regBuf[reg], &data[3], len);
					if(reg==CHANNEL_SELECT_ADDRESS)
						select_pin=regBuf[reg];
					ReceiveHandleCount += len;
				}
				break;
			case UART_READ_REGBUF:
				ReceiveHandleCount+=3;	
				if(reg==CHANNEL_DATA_ADDRESS){
//					senddata_flag=1;
					HAL_UART_Transmit_IT(&huart1, uart_buffer,3);//发送
				}
				break;
			case AT_CMD:		//AT指令兼容		\r\n
				if(strncmp((char*)data,"AT\r\n",4)==0){
					len =4;
					ReceiveHandleCount += len;
					sprintf((char*)&buf[0],"OK\r\n");	
					HAL_UART_Transmit_IT(&huart1, buf,4);
				}else if(strncmp((char*)data,"AT+AIN1=?\r\n",11)==0){	
					len =11;
					ReceiveHandleCount += len;
					select_pin=1;
					AT_flag=1;
					
				}else if(strncmp((char*)data,"AT+AIN2=?\r\n",11)==0){	
					len =11;
					ReceiveHandleCount += len;
					select_pin=2;
					AT_flag=2;
				}
				else{	//数据错误

					ReceiveCount = 0;
					ReceiveHandleCount = 0;
					sprintf((char*)buf,"ERROR\r\n");//再看下
					HAL_UART_Transmit_IT(&huart1, buf,9);
				}
				break;
			default:		//数据错误
				
				ReceiveCount = 0;
				ReceiveHandleCount = 0;
				sprintf((char*)buf,"ERROR\r\n");//再看下
				HAL_UART_Transmit_IT(&huart1, buf,9);
				break;
		}	
		if (ReceiveCount == ReceiveHandleCount) {  
			ReceiveCount = 0;
			ReceiveHandleCount = 0;
	  }		
	}
	else{//数据错误
			ReceiveCount = 0;
			ReceiveHandleCount = 0;
	}
	
}

