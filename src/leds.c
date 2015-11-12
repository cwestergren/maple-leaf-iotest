#include "leds.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"


int leds_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	return 0;
}

static int bLit = 0;

int led_toggle(void){

	if( bLit ) {
		GPIO_ResetBits(GPIOC,GPIO_Pin_12);
		//GPIOA->BRR = 1 << 6;
	} else {
		//GPIOA->BSRR = 1 << 6;
		GPIO_SetBits(GPIOC,GPIO_Pin_12);
	}

	bLit = !bLit;
	return bLit;

}
