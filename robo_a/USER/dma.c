#include "dma.h"
#include "mti.h"

void dma_config(void)
 { 
	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream2_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;   
	NVIC_Init(&NVIC_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Stream2);
	DMA_DeInit(DMA1_Stream4);
	DMA_StructInit(&DMA_InitStructure);

	DMA_InitStructure.DMA_BufferSize = MTI_SEND_SIZE ;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(UART4->DR)) ;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
 /* SEND */
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Channel = DMA_Channel_4 ; 
	DMA_InitStructure.DMA_DIR =  DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)mti_sendbuffer;
	DMA_Init(DMA1_Stream4, &DMA_InitStructure);
 /* REC */
	DMA_InitStructure.DMA_BufferSize = MTI_BUF_SIZE ;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Channel = DMA_Channel_4 ;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;
	DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)mti_buffer; 
	DMA_Init(DMA1_Stream2, &DMA_InitStructure); 

	DMA_Cmd(DMA1_Stream2, ENABLE);  // RX
	//DMA_Cmd(DMA1_Stream4, ENABLE);  // TX

	//USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE);
	USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);

	DMA_ITConfig(DMA1_Stream2, DMA_IT_TC, ENABLE);       
	//DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE);       
 }
