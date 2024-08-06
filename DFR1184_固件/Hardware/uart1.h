#ifndef __UART1_H
#define __UART1_H

#include "cs32l010_hal.h"
#include "stdbool.h"	
#include "string.h"	

#define UART1_RX_PORT GPIOC
#define UART1_RX_PIN  GPIO_PIN_4

#define UART1_TX_PORT GPIOC
#define UART1_TX_PIN  GPIO_PIN_3


extern uint8_t AT_flag;
extern volatile uint8_t cs32TimerFlag;
void SerialInit(uint32_t baud_rate);
void uart_command(void);
void SerialAnalysisTimerInit(void);

void UART_DeInit(void); //ÊÍ·Å´®¿Ú
#endif
