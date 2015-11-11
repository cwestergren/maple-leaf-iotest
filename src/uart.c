#include <string.h>

#include "stm32f4xx_usart.h"
#include "misc.h"

#include "types.h"
#include "uart.h"
#include "cli/cli.h"

#define RX_INTERRUPT

void upper_UART_ISR(int i);
uint32 stopModeEntrys = 0;

#define USART_FLAG_ERRORS (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE)


void USART3_IRQHandler(void) {
	int i;

	stopModeEntrys++;

//	return;

	if ((USART3->SR & USART_FLAG_RXNE) != (u16)RESET)           
    {          
		i = USART_ReceiveData(USART3);

		USART_ClearFlag(USART2,USART_FLAG_RXNE);
	//     USART_GetFlagStatus(USART2,USART_FLAG_ORE);


		upper_UART_ISR(i);
	}

	uint32 status;
	status = USART3->SR;
    while(status & (USART_FLAG_RXNE |    USART_FLAG_ERRORS))
    {
        uint8 ch;

        ch = (volatile uint32_t)USART3->DR;    // Read Data, or Clear Error(s)
        (void)ch;

        status = USART3->SR;
    }

}


int uart_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// PB10  = TX
	// PB11 = RX

	// Clock the GPIO bank
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	// Clock the UART peripheral
	RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

#ifdef RX_INTERRUPT
	NVIC_InitTypeDef NVIC_InitStructure;

	memset(&NVIC_InitStructure,0,sizeof(NVIC_InitTypeDef));

	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_SetPriority(USART3_IRQn,5);
#endif

	// Set up AFs
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

	// Configure the GPIOs to be routed as AFs
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure USART Rx as alternate function  */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStruct);

	USART_ClearFlag(USART3, USART_IT_RXNE);
    USART_ClearFlag(USART3, USART_IT_TXE);
#if 0
	uint32 status;
	status = USART3->SR;
#define USART_FLAG_ERRORS (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE)
    while(status & (USART_FLAG_RXNE |    USART_FLAG_ERRORS))
    {
        uint8 ch;

        ch = USART3->DR;    // Read Data, or Clear Error(s)

        if (status & USART_FLAG_ERRORS) {
        }
        else {
//            Fifo_Insert(Uart->Rx, sizeof(ch), &ch);
        }

        status = USART3->SR;
    }
#endif

	/* Enable USART */
	USART_Cmd(USART3, ENABLE);

#ifdef RX_INTERRUPT
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
#endif

	return 0;
}

int uart_puts(const char *pzStr) {
	char *pS;

	pS = (char *)pzStr;
	while( *pS != 0 ) {
		USART_SendData(USART3, (uint8_t)*pS);

		/* Loop until the end of transmission */
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET) {}

		pS++;
	}

	return 0;
}
