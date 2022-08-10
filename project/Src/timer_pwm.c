#include "stm32f103x6.h"
#include "timer_pwm.h"



void init_RCC()
{
	RCC_APB2ENR |= TIM1EN | IOPCEN; //enable tajmer 1, porta C
}

void init_NVIC()
{
	NVIC_ISER0 = SETENA << 25;
}

void init_TIM1()
{
	/*
	 * Kako bi tajmer generisao zahtev za obradom izuzetka u trenutku kada se
	 * desi Update Event neophodno je postaviti bit UIE u okviru registra TIMx_DIER.
	 *
	 * Frekvencija generisanja zahteva za obradom izuzetka definisana je:
	 *    (1) frekvencijom signala takta tajmera,
	 *    (2) vrednoscu Prescaler registra i
	 *    (3) vrednoscu Auto-reload registra,
	 * prema sledecoj formuli:
	 *
	 *    Timer_interrupt_frequency = Timer_clock_frequency / ((TIMx_PSC + 1) * TIMx_ARR)
	 *
	 * Tajmer je neophodno omoguciti postavljanjem bita CEN u okviru registra TIMx_CR1.
	 *
	 * Izvor informacija:
	 *
	 * 03_STM32F103_Reference_Manual -> 14.4.4 TIM1 and TIM8 DMA/interrupt enable register (TIMx_DIER) (pg. 344)
	 * 02_STM32F103_Datasheet -> 2.1 Device overview -> Figure 2. Clock tree (pg. 13)
	 * 03_STM32F103_Reference_Manual -> 14.4.11 TIM1 and TIM8 prescaler (TIMx_PSC) (pg. 356)
	 * 03_STM32F103_Reference_Manual -> 14.4.12 TIM1 and TIM8 auto-reload register (TIMx_ARR) (pg. 356)
	 * 03_STM32F103_Reference_Manual -> 14.4.2 TIM1 and TIM8 control register 1 (TIMx_CR1) (pg. 338)
	 *
	 */
	TIM1_DIER |= UIE;
	TIM1_SR = 0;
	TIM1_PSC = PSC_VALUE;
	TIM1_ARR = ARR_VALUE;
	TIM1_CR1 |= CEN;
}

