#include <string.h>

#include "types.h"
#include "adc.h"
#include "log.h"

#include "stm32f4xx_adc.h"
#include "stm32f4xx_dma.h"

static uint8 uhADCxConvertedValue[2048];

void DMA2_Stream0_IRQHandler(void) {
	if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))
	{
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
	}
}

static void dma_init(void) {
	DMA_InitTypeDef DMA_InitStruct;

	DMA_DeInit(DMA2_Stream0);

	LOG("ADC1 DR ?  %p",&ADC1->DR);

	DMA_InitStruct.DMA_Channel = DMA_Channel_0;  
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)( &ADC1->DR );
	DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)uhADCxConvertedValue;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStruct.DMA_BufferSize = 1024;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStruct);

	DMA_Cmd(DMA2_Stream0,ENABLE);

#if 0
	NVIC_InitTypeDef NVIC_InitStruct;

	memset(&NVIC_InitStruct,0,sizeof(NVIC_InitTypeDef));

	/* Enable the DMA2_S0 Interrupt */
	NVIC_InitStruct.NVIC_IRQChannel = DMA2_Stream0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	NVIC_SetPriority(DMA2_Stream0_IRQn,5);

	DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,ENABLE);
#endif
}

int adc_init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);

	ADC_DeInit();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA,&GPIO_InitStruct);


	ADC_CommonInitTypeDef ADC_CommonInitStruct;
	ADC_CommonStructInit(&ADC_CommonInitStruct);
	ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
	//ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
//	ADC_CommonInitStruct.ADC_NbrOfChannel = 1;
	ADC_CommonInit(&ADC_CommonInitStruct);

	ADC_InitTypeDef ADC_InitStruct;
	ADC_StructInit(&ADC_InitStruct);
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1,&ADC_InitStruct);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_3Cycles);
#if 0

	dma_init();

	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1,ENABLE);
	ADC_Cmd(ADC1,ENABLE);
	ADC_SoftwareStartConv(ADC1);
#else

	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	ADC_Cmd(ADC1,ENABLE);


//	ADC_DMACmd(ADC1,ENABLE);
	ADC_SoftwareStartConv(ADC1);
	ADC_ContinuousModeCmd(ADC1,ENABLE);
	ADC_DMACmd(ADC1,ENABLE);

	dma_init();

//	DMA_Cmd(DMA2_Stream0,ENABLE);
	//ADC_SoftwareStartConv(ADC1);
#endif
	return 0;
}

int adc_getBuffer(void **ppBuffer) {
	uint16 *pSamples;

	pSamples = (uint16*)uhADCxConvertedValue;

//	LOG("Sample0:  DMA=%i  MANUAL=%i",pSamples[0],ADC1->DR);

	return 0;
}
