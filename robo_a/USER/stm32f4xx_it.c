/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

#include "stm32f4xx_it.h"

#include "clock.h"
#include "debug.h"
#include "movement.h"
#include "multi_processing.h"
#include "pwm.h"
#include "usart.h"
#include "utils.h"
 

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}


/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}


void SPI2_IRQHandler(void)
{
	#ifdef DEBUG_SPI
	printf("SPI:0x%x\n", SPI_ReceiveData(SPI2));
	#endif
	
	#ifdef MP_SLAVE0
	SPI_SendData(SPI2, SPI_ReceiveData(SPI2));
	#endif
	
	SPI_ClearITPendingBit(SPI2, SPI_IT_RXNE);
}


void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART_SendData(UART5, USART_ReceiveData(USART1));
	}
}

void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		USART_SendData(UART5, USART_ReceiveData(USART2));
	}
}

#include "interpreter.h"
void USART3_IRQHandler(void)
{
	char data;

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		data = USART_ReceiveData(USART3);
		
		in_char_queue(&cmd_queue, data);
	}
}

void UART4_IRQHandler(void)
{
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		USART_SendData(UART5, USART_ReceiveData(UART4));
	}
}

//void UART5_IRQHandler(void)
//{
//	char data;
//	char tmp;

//	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
//	{
//		data = USART_ReceiveData(UART5);
//		USART_SendData(UART5, data);

//		#ifdef DEBUG_DB_EXEC
//		#include "database.h"
//		
//		static bool db_exec_start = false;
//		switch(data) {
//			case '\n':
//				if(db_exec_start) {
//					db_exec_start = false;
//					in_char_queue(&db_cmd_queue, data);
//					db_queue_exec();
//				} else {
//					while(-1 != out_char_queue(&db_cmd_queue, &tmp));
//				}
//				break;
//			case '#':
//				db_exec_start = true;
//				break;
//			default:
//				if(db_exec_start)
//					in_char_queue(&db_cmd_queue, data);
//				break;
//		}
//		#endif
//	}
//}

#include "automove.h"
void TIM1_UP_TIM10_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM10, TIM_IT_Update) != RESET) {
		automove_daemon();
		TIM_ClearITPendingBit(TIM10, TIM_IT_Update);
	}
}


/*
Used for the control of the wheels
*/
#include "fan.h"
#include "encoder.h"
void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
		static uint16_t count0 = 0;
		const uint16_t count_num = 2000;
		if(TIM_GetITStatus(TIM10, TIM_IT_Update) != RESET) {
			count0++;
			if(count0 <= count_num / 2) {
				if(0 == count0 % count_num / 10) {
					if(1 == fan_up_flag) {
						if(((get_pos_fan() - fan_height) > (fan_up_length + FAN_THOLD)) || ((get_pos_fan() - fan_height) < (fan_up_length - FAN_THOLD)))
							((get_pos_fan() - fan_height) < fan_up_length) ? fan_up(10) : fan_down(10);
						else stop_fan_up_down();
					}
				}
			}
			if(count0 > count_num / 2)
				stop_fan_up_down();
			count0 %= count_num;
			TIM_ClearITPendingBit(TIM10, TIM_IT_Update);
	}
	}
}


#ifndef USE_HPWM

void TIM6_DAC_IRQHandler(void)
{
	static uint16_t t[PWM_CHANNEL_NUM] = {0};
	uint8_t i;
	
	if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
	{
		for(i = 0; i < PWM_CHANNEL_NUM; i++) {
			t[i]++;
			if(t[i] > PWMHighTime[i]) {
				if(t[i] < PWMTotal[i]) {
					GPIO_WriteBit(PWMPorts[i], PWMPins[i], Bit_RESET);
				} else {
					GPIO_WriteBit(PWMPorts[i], PWMPins[i], Bit_SET);
					t[i] = 0;
				}
			}
		}
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	}
}
#endif


#include "suart.h"
void TIM1_TRG_COM_TIM11_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM11, TIM_IT_Update) != RESET) {
		suart_check();
		TIM_ClearITPendingBit(TIM11, TIM_IT_Update);
	}
}


#include "mti.h"
void DMA1_Stream2_IRQHandler(void)
{
	static bool mti_init_flag = false;

	if(SET == DMA_GetITStatus(DMA1_Stream2, DMA_IT_TCIF2) ) {
		DMA_Cmd(DMA1_Stream2, DISABLE);
		
		mti_angle_new = mti();
		
		if(0 == mti_angle_new) {
			while(1);
		}
		
		mti_angle += mti_angle_new - mti_angle_old;;
		
		if(mti_angle_new < -PI / 2 && mti_angle_old > PI / 2) {
			mti_angle += 2 * PI;
		} else if(mti_angle_new > PI / 2 && mti_angle_old < - PI / 2) {
			mti_angle += -2 * PI;
		}
		
		mti_angle_old = mti_angle_new;
		
		if(!mti_init_flag) {
			mti_angle = 0;
			mti_init_flag = true;
		}
		
		mti_value_flag = 1;
		//uprintf(UART5,"angle%f\r\n",mti_angle);//²âÊÔÓÃ
		DMA_ClearITPendingBit(DMA1_Stream2, DMA_IT_TCIF2);
		DMA_Cmd(DMA1_Stream2, ENABLE);
		mti_value_flag = 0;
	}
}

void DMA1_Stream4_IRQHandler(void)
{
	if(SET == DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4)) {
		uprintf(UART5,"SendOK\r\n");
		DMA_Cmd(DMA1_Stream4, DISABLE);
		DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
	}
}


#include "fan.h"
#include "encoder.h"
void TIM7_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM7, TIM_FLAG_Update);

		g_rotary_magnet += (TIM_GetCounter(TIM1)-4000);
		TIM1->CNT = 4000;
		
		g_rotary_fan += (TIM_GetCounter(TIM3)-4000);
		TIM3->CNT = 4000;
		
		g_rotary_x += (TIM_GetCounter(TIM4)-4000);
		TIM4->CNT = 4000;
		
		g_rotary_y += (TIM_GetCounter(TIM5)-4000);
		TIM5->CNT = 4000;

		g_rotary_tim8 += (TIM_GetCounter(TIM8)-4000);
		TIM8->CNT = 4000;
	}
}

#include "watchdog.h"
void TIM8_UP_TIM13_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM13, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM13, TIM_FLAG_Update);
		IWDG_ReloadCounter();
		
		#ifdef DEBUG_IWDG
		static uint8_t t;
		static uint32_t t1;
		t++;
		if(t > 10) {
			t = 0;
			t1++;
			printf("%ld\n", t1);
		}
		#endif
	}
}

/*
	exti8 exti11 exti3
	switch 0\1\2
	fan_down_stop\fan_up_stop\light_electricity
*/
#include "switch.h"
void EXTI9_5_IRQHandler(void)
{
	delay_ms(10);
	if(SET == EXTI_GetITStatus(EXTI_Line8)){
		if(0 == GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_8)) {
			if(get_pos_fan() > get_pos_fan()) stop_fan_up_down();
			#ifdef DEBUG
			printf("\nstop_fan_up_down()\n");
			#endif
		}
		EXTI_ClearITPendingBit(EXTI_Line8);
	}
}

void EXTI15_10_IRQHandler(void)
{
	delay_ms(10);
	if(SET == EXTI_GetITStatus(EXTI_Line11)){
		if(0 == GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_11)) {
			stop_fan_up_down();
			fan_up_stop_auto();
			#ifdef DEBUG
			printf("\nstop_fan_up_down()\n");
			#endif
		}
		EXTI_ClearITPendingBit(EXTI_Line11);
	}
}
#include "movement.h"
void EXTI3_IRQHandler(void)
{
	if(0 == GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_3)) {
		/*stop car*/
		stop_all();
	}
	EXTI_ClearITPendingBit(EXTI_Line3);
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
