#include "leds.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"


int leds_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

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
