#include "misc.h"

#include "clock.h"

static uint16_t fac_us = 0;			   

void system_clk_set(void)
{ 
    ErrorStatus HSEStartUpStatus;
  RCC_DeInit();
 
  RCC_HSEConfig(RCC_HSE_ON );   //打开外部时钟
 
  HSEStartUpStatus = RCC_WaitForHSEStartUp();  //等待外部时钟打开至稳定
 
  if(HSEStartUpStatus == SUCCESS)     
  {
    FLASH_SetLatency(FLASH_Latency_5);   
    FLASH_PrefetchBufferCmd(ENABLE);       //flash时钟的相关配置  
		RCC_PLLCmd(DISABLE);  //配置PLL之前需要关闭PLL
		RCC_HCLKConfig(RCC_SYSCLK_Div1);   //HCLK分频
    RCC_PCLK2Config(RCC_HCLK_Div1);   //PCLK2分频
		RCC_PCLK1Config(RCC_HCLK_Div1);    //PCLK1分频
    RCC_PLLConfig(RCC_PLLSource_HSE, 8, 336, 2, 7);    //sysclk = 168MHZ  ,,计算公式参见数据手册
    RCC_PLLCmd(ENABLE); //使能PLL
 
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)      //等待，，直到PLL使能完毕
       {
       }
 
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  //选择PLL时钟为系统时钟
 
    while(RCC_GetSYSCLKSource() != 0x08)       //等待，至PLL时钟设置成系统时钟完毕
       { 
       }
     }
    #if(__FPU_PRESENT == 1)&&(__FPU_USED == 1)
			SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  //开启FPU
		#endif

}

void systick_config()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	fac_us = SystemCoreClock / 1000000 / 8;	

	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void delay_us(uint32_t us)
{		
	u32 tmp;

	SysTick->LOAD = us * fac_us;
	SysTick->VAL = 0x00;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;

	do {
		tmp = SysTick->CTRL;
	} while((tmp & 0x01) && !(tmp & (1<<16)));

	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL = 0X00;
}

void delay_ms(uint32_t ms)
{
	while(ms--)
		delay_us(1000);
}
