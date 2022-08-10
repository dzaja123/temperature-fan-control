#include <stdio.h>
#include "stm32f103x6.h"


void ADC_General_Init(ADC_TypeDef *  ADCx){
	//RCC->CR |= RCC_CR_HSION; // Ukljucivanje Internal 8 MHz RC oscillator ON
	//while((RCC->CR & RCC_CR_HSIRDY) != RCC_CR_HSIRDY); // Sacekati dok (RCC->CR & RCC_CR_HSIRDY) = 1: Internal 8 MHz RC oscillator ready

	ADCx->CR2 |= ADC_CR2_ADON;

	ADCx->CR2 |= ADC_CR2_CAL;
	while ((ADCx->CR2 & ADC_CR2_CAL)==ADC_CR2_CAL);

	// Isključiti ADC dok se ne podese svi parametri
	ADCx->CR2 &= ~ADC_CR2_ADON;

	ADCx->CR2 &= ~ADC_CR2_CONT; // CONT = 0: Single conversion mode

	// Ukljucivanje internog temperaturnog senzora i referentnog napona
	ADCx->CR2 |= ADC_CR2_TSVREFE;

	// Želimo konverziju samo jednog kanala u regularnoj grupi
	// I to podešavamo setovanjem L[3:0] bitova u ADC_SQR1 registru
	ADCx->SQR1 &= ~ADC_SQR1_L;   // L[3:0]=0000: 1 conversion, ADC_SQR1

	// Odredjujemo kanal 1 (Channel 1) za prvu (ovde je to jedina) konverziju u regularnoj grupi
	// Setovanjem bitova SQ1[4:0] u ADC_SQR3 registru
	ADCx->SQR3 &= ~ADC_SQR3_SQ1; // Brišemo polje SQ1[4:0]
	ADCx->SQR3 |= (1U << ADC_SQR3_SQ1_Pos); // ADC_SQR3_SQ1_Pos oznacava poziciju polja SQ1 u registru ADC_SQR3. SQ1 = Channel 1;

	// Želimo softversko okidanje konvezije,
	// Podešavamo preko EXTSEL[2:0] bitova u registru ADC_CR2
	ADCx->CR2 &= ~ADC_CR2_EXTSEL;
	ADCx->CR2 |= (0b111U << ADC_CR2_EXTSEL_Pos); // EXTSEL[2:0] = 111: SWSTART

	// Uključiti ADC
	ADCx->CR2 |= ADC_CR2_ADON;

}

void ADC_GPIO_Init(void) {

	// Enable the peripheral clock of GPIO Port A
	RCC->APB2ENR |=   RCC_APB2ENR_IOPAEN;

	// ********************** ADC12_IN1 -> PA1 ***************************
	// ADC12_IN1 ulazni kanal se mapira na PA1 na osnovu tabele 5, strana 26 u STM32F103_Datasheet.pdf (manji dokument od 99 strana)
	// Kako se konfigurise GPIO pin da bi se koristio za ADC ?
	// Секција 9.1.1. GPIO configurations for device peripherals, видети Figure 19, страна 169, STM32F103 ref. manuala
	// ADC -> GPIO Pin configuration:Analog - Input

	// Podesavamo pin PA1 da radi u Input rezimu, Analog
	// Table 20. Port bit configuration table, Strana 161, Za neki pin x, MODEx[1:0]=00:Input, CNFx[1:0]=00:Analog
	GPIOA->CRL &= ~(GPIO_CRL_MODE1 | GPIO_CRL_CNF1); // Obrisati  CNF1[1:0] i MODE1[1:0] - za konfiguraciju PA1 pina da radi u Input, Analog modu
}

void ADC_Init(void) {
	// Enable the clock of ADC 1
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;  // Enable ADC 1 clock

	ADC_GPIO_Init();
	ADC_General_Init(ADC1);

	ADC1->CR1 |= ADC_CR1_EOCIE; // Dozvoliti interapt

	NVIC_SetPriority(ADC1_IRQn, 1);		// Set Priority to 1
	//NVIC->ISER[0] |= NVIC_ISER_SETENA_18;
	//NVIC_EnableIRQ(ADC1_IRQn);			// Enable interrupt of ADC 1 & 2
	NVIC_EnableIRQ(ADC1_2_IRQn); 		// Ovo je identicno
}

