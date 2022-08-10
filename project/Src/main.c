#include <stdio.h>
#include "stm32f103x6.h"
#include "util.h"
#include "timer_pwm.h"
#include "adc.h"
#include "usart.h"

#define GPIOC_CRL (*((uint32_t *) (0x40011000 + 0x00)))
#define GPIOC_CRH (*((uint32_t *) (0x40011000 + 0x04)))
#define GPIOC_ODR (*((uint32_t *) (0x40011000 + 0x0C)))
#define MODE_OUT_2MHZ 0x02
#define CNF_GP_OUT_PUSH_PULL 0x00
#define MODE_IN 0x00
#define CNF_GP_IN_PULL 0x02

#define BufferSize 32

uint32_t interrupt_count = 0;
uint8_t adc_interrupt_count = 0;
uint8_t uart_interrupt_count = 0;
uint8_t display_interrupt_count = 0;

uint8_t dot = 0;
uint8_t sent = 1;

uint8_t buffer[BufferSize];

uint8_t speed_up = 0;
uint8_t speed_down = 0;

uint8_t encoding[] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F,0x00};
uint8_t digits[] = { 0, 0, 0, 0, 0, 0 };
uint8_t current_digit = 0;

int speed = 0;
volatile int adc_dr;

void Motor_On(void){
	GPIOC->ODR |= GPIO_ODR_ODR3;
}
void Motor_Off(void){
	GPIOC->ODR &= ~GPIO_ODR_ODR3;
}
void process_tim1_update_interrupt()
{
	/*
	 * Timer_interrupt_frequency
	 *     = Timer_clock_frequency / ((TIMx_PSC + 1) * TIMx_ARR)
	 *     = 8MHz / ((7999 + 1) * 2)
	 *     = 500Hz
	 */
	if (++interrupt_count == 50) // 500
	{
		interrupt_count = 0;

		adc_interrupt_count++;
		adc_interrupt_count = adc_interrupt_count % 2; //slanje na 2 sekunde

		uart_interrupt_count++;
		uart_interrupt_count = uart_interrupt_count % 10; // na 10 sekundi

		display_interrupt_count++;
		display_interrupt_count = display_interrupt_count % 5; // 5 sekundi

	    if ( display_interrupt_count == 0)
		{

		 digits[0] = (speed / 100) % 10;
		 digits[1] = (speed/10) % 10;
		 digits[2] = speed % 10;
	   	 digits[3] = 10;
	     digits[4] = 10;
	   	 digits[5] = 10;
		 dot = 0;
		}

		if ( display_interrupt_count == 1)
		{
			int temp_ceo;
			int temp_dec;

			temp_ceo = adc_dr*5*100/4096;
			temp_dec = (adc_dr*5*625/256) % 100;

			digits[0] = (temp_ceo/10) % 10;
			digits[1] = temp_ceo % 10;
			digits[2] = (temp_dec/10) % 10;
			digits[3] = temp_dec % 10;
			digits[4] = 10;
			digits[5] = 10;
			dot = 1;
		}
	}

	if ((interrupt_count/5) < speed)
		Motor_On();
	else
		Motor_Off();

	GPIOB->ODR &= ~0x3F << 8;
	GPIOB->ODR |= 0xFF;
	GPIOB->ODR &= (~encoding[digits[current_digit]]);

	if ((dot) && (current_digit == 1))
    	GPIOB->ODR &= 0x7F;

	GPIOB->ODR |= (0x01 << (8 + current_digit));
	current_digit = (current_digit + 1) % 6;
}

void TIM1_UP_IRQHandler()
{
	if ((TIM1_SR & 0x01) != 0)
	{
		TIM1_SR = 0;
		/*
		 * Funkcija process_tim1_update_interrupt predstavlja
		 * izdvojenu logiku za obradu izuzetka
		 */
		process_tim1_update_interrupt();
	}
}

void init_GPIOB()
{
	RCC_APB2ENR |= (1 << 3);
	uint32_t gpioc_crl_value = GPIOB->CRL;

	for (int i = 0; i < 8; i++)
	{
		gpioc_crl_value &= ~(0x0F << (4 * i));
		gpioc_crl_value |= MODE_OUT_2MHZ << (4 * i) | CNF_GP_OUT_PUSH_PULL << (2 + 4 * i);
	}
	GPIOB->CRL = gpioc_crl_value;

	uint32_t gpioc_crh_value = GPIOB->CRH;

	for (int i = 0; i < 6; i++)
	{
		gpioc_crh_value &= ~(0x0F << (4 * i));
		gpioc_crh_value |= MODE_OUT_2MHZ << (4 * i) | CNF_GP_OUT_PUSH_PULL << (2 + 4 * i);
	}
	GPIOB->CRH = gpioc_crh_value;
	GPIOB->ODR = 0x0;
}

void init_GPIOC()
{
	// - Treba ukljuciti klok za Port C,
	// - Podesiti za PC0 i PC1 izlazni rezim na 2MHz sa output push-pull konfiguracijom

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;	// ukljucujemo klok za Port C
	GPIOC->CRL &= ~(GPIO_CRL_MODE3 | GPIO_CRL_CNF3); // brisemo podesavanja za PC1
	GPIOC->CRL |= GPIO_CRL_MODE3_1; // Polje MODE za PC1: Output mode, max speed 2 MHz,
	GPIOC->CRL &= ~GPIO_CRL_CNF3; // Polje CNF za PC1: 00: General purpose output push-pull, nismo morali da stavljamo uopste

	// Iskljucujemo motor na pocetku
	GPIOC->ODR &= ~(GPIO_ODR_ODR3);
	// a moze i GPIOC->ODR |= ~(3U); ili ~(0b11U);

	GPIOC->CRL &= ~(0x0F << (4 * 0));
	GPIOC->CRL |= MODE_IN << (4 * 0) | CNF_GP_IN_PULL << (2 + 4 * 0);
	GPIOC->CRL &= ~(0x0F << (4 * 1));
	GPIOC->CRL |= MODE_IN << (4 * 1) | CNF_GP_IN_PULL << (2 + 4 * 1);
	GPIOC->CRL &= ~(0x0F << (4 * 2));
	GPIOC->CRL |= MODE_IN << (4 * 2) | CNF_GP_IN_PULL << (2 + 4 * 2);
}

void ADC1_2_IRQHandler(void) {

	uint16_t statReg = (ADC1->SR & 0xFFFFU);
	if ((statReg & ADC_SR_EOC) == ADC_SR_EOC) {
		adc_dr = (ADC1->DR & 0xFFFFU);
	}
}

int main(void){

	int poruka;
	int temp_ceo;
	int temp_dec;
	int vinadc;

	init_RCC();
	init_NVIC();
	init_GPIOB();
	init_TIM1();
	init_GPIOC();
	ADC_Init();
	UART1_Init();

	while (1){

		if (adc_interrupt_count == 0)
			{
			 ADC1->CR2 |= ADC_CR2_SWSTART; // Startovanje konverzije
			 delay(100);
			 vinadc = adc_dr;
			}

		if ((GPIOC->IDR & 0x1)==0x1)
		{
			 if (vinadc > 300)
				 speed = 100;
			 else if (vinadc>150)
			 {
				 speed = (vinadc-150)*2/3;
			 }
			 else
				 speed = 0;
		}
		else
        {
			if (((GPIOC->IDR & 0x2)==0x2) && (speed_up == 0))
			{
				speed_up = 1;
				speed = speed + 5;
			}

			if (((GPIOC->IDR & 0x4)==0x4) && (speed_down == 0))
				{
					speed_down = 1;
					speed = speed - 5;
				}
		}

		if ((GPIOC->IDR & 0x2)!=0x2)
		{
		    speed_up = 0;
		}

		if ((GPIOC->IDR & 0x4)!=0x4)
		{
		    speed_down = 0;
		}

		if (speed > 100)
			speed = 100;
		if (speed < 0)
			speed = 0;

		if (uart_interrupt_count == 0 && sent == 0)
		{
		   vinadc = adc_dr;
		   temp_ceo = vinadc*5*100/4096;
		   temp_dec = (vinadc*5*625/256) % 100;

		   poruka = kopirajUString((char *)buffer, "Speed = %d percent \r\n", &speed);
		   USART_Write(USART1, buffer, poruka);

		   poruka = kopirajUString((char *)buffer, "Temperature = %d", &temp_ceo);
		   USART_Write(USART1, buffer, poruka);

		   poruka = kopirajUString((char *)buffer, ".%dC\r\n", &temp_dec);
		   USART_Write(USART1, buffer, poruka);

		   sent = 1;
		}
		if (uart_interrupt_count != 0)
			sent = 0;
	}
}
