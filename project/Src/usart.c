#include <stdio.h>
#include "stm32f103x6.h"
#include "util.h"

void USART_Init (USART_TypeDef * USARTx) {
	// Default setting:
	// No hardware flow control, 8 data bits, no parity, 1 start bit and 1 stop bit
	USARTx->CR1 &= ~USART_CR1_UE;  // Disable USART

	// Configure word length to 8 bit
	USARTx->CR1 &= ~USART_CR1_M;   // M: 00 = 8 data bits, 01 = 9 data bits, 10 = 7 data bits

	// Configure oversampling mode: Oversampling by 16
	// CORTEX M4 samo - USARTx->CR1 &= ~USART_CR1_OVER8;  // 0 = oversampling by 16, 1 = oversampling by 8


	// Configure stop bits to 1 stop bit
	//   00: 1 Stop bit;      01: 0.5 Stop bit
	//   10: 2 Stop bits;     11: 1.5 Stop bit
	USARTx->CR2 &= ~USART_CR2_STOP;

	// Podesavanje Baudrate - Strana 798 i 799, 27.3.4 Fractional baud rate generation, Tabela 192. na strani 799
	// CSet Baudrate to 9600 using APB frequency
	// Tx/Rx baud = f_CK / (16*USARTDIV),
	// BRR = 0x1D4C za USARTDIV=468,75 za 72MHz, odnosno BRR = 0xEA6, USARTDIV = 234,75 za 36MHZ, Limited to 16 bits

	// Default clock je 8MHz, USARTDIV = 8.000.000/(16*9600) = 52,083 ,
	// DIV_Mantissa = 52 = 0x34, DIV_Fraction=0,083 u hex broju = 0,083*16=1,328->DIV_Fraction=0x1
	// -> BRR = 0x341
	USARTx->BRR  = 0x341;

	USARTx->CR1  |= (USART_CR1_RE | USART_CR1_TE);  	// Transmitter and Receiver enable

	USARTx->CR1  |= USART_CR1_UE; // USART enable

	//CORTEX M4- while ( (USARTx->ISR & USART_ISR_TEACK) == 0); // Verify that the USART is ready for reception
	//CORTEX M4 - while ( (USARTx->ISR & USART_ISR_REACK) == 0); // Verify that the USART is ready for transmission
}

void UART1_GPIO_Init(void) {

	// Enable the peripheral clock of GPIO Port A
	RCC->APB2ENR |=   RCC_APB2ENR_IOPAEN;

	// ********************** USART 1 ***************************
	// U Reference Manualu STM32F103R6 treba videti na koje pinove GPIO porta se mapira USART1 i u kojem rezimu treba setovati te pinove GPIO porta
	// jer se ukljucuje alternativna funkcija
	// PA9 = USART1_TX, PA10 = USART1_RX (strana 181)
	// Rezim rada GPIO Porta - strana 166 i 167
	// USARTx_TX - Ako radi u Full duplex rezimu podesiti: Alternate function push-pull (The USART_TX pin can also be configured as alternate function open drain.)
	// USARTx_TX - Ako radi u Half duplex synchronous rezimu podesiti: Alternate function push-pull
	// USARTx_RX - Ako radi u Full duplex rezimu, podesiti port kao: Input floating / Input pull-up
	// USARTx_RX - Ako radi u Half duplex synchronous rezimu, onda se ne koristi pin: Not used. Can be used as a general IO

	// Prvo podesavamo pin PA9 na koji se povezuje USART1_TX, tako da pin treba da radi u izlaznom rezimu sa alternativnom funkcijom
	GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9); // Obrisati  CNF9[1:0] i MODE9[1:0] - za konfiguraciju 9 pina, porta A
	// Alternativno moze da se koristi GPIOA_CRH &= ~(0xF << 4);
	// Ili GPIOA_CRH &= ~0xF0;

	// Prvo se podesava rezim rada pina 9 porta A - ulazni, ili jedan od tri izlazna.
	// Vrednosti za MODE[1:0]: 00: Input mode (reset state), 01: Output mode, max speed 10 MHz.
	// 10: Output mode, max speed 2 MHz, 11: Output mode, max speed 50 MHz.

	GPIOA->CRH |= (GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0);// Setujemo vrednost 11 za Mode9[1:0] = Output mode, max speed 50 MHz.
	// isto moze i GPIO->CRH |= 0b11 << 4; ili GPIO->CRH |= 0x30;

	// Sada se podesava kako da pin radi u izlaznom rezimu.
	// Vrednosti za GPIOx_CNF[1:0]: Ako polje MODE[1:0] nije = 00, odnosno pin je setovan u nekom od izlaznih rezima, onda:
	// 00: General purpose output push-pull, 01: General purpose output Open-drain
	// 10: Alternate function output Push-pull, 11: Alternate function output Open-drain

	GPIOA->CRH |= GPIO_CRH_CNF9_1;	// Setuje se vrednost 10 za Mode9[1:0] -> Alternate function push-pull
	// Moze i GPIO->CRH |= 0x80;

	// Zatim podesavamo pin PA10 na koji se povezuje USART1_RX, tako da pin treba da radi u ulaznom rezimu sa alternativnom funkcijom
	GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10); // Obrisati  CNF9[1:0] i MODE9[1:0] - za konfiguraciju 10 pina, porta A

	// Prvo podesavamo rezim rada pina 10, porta A
	// Vrednosti za MODE[1:0]: 00: Input mode (reset state), 01: Output mode, max speed 10 MHz. 10: Output mode, max speed 2 MHz, 11: Output mode, max speed 50 MHz.
	// Mi hocemo da postavimo vrednost 00: Input mode, ali posto smo prethodno obrisali te bitove, ne diramo nista

	// Vrednosti za GPIOx_CNF[1:0]: Ako je polje MODE[1:0] = 00, odnosno pin je u input rezimu, onda:
	// 00: Analog mode, 01: Floating input (reset state), 10: Input with pull-up / pull-down, 11: Reserved

	GPIOA->CRH |= GPIO_CRH_CNF10_0; // USART radi u full-duplex rezimu, pa setujemo kao Floating input = 01, videti gore ili strana 167

}
void UART1_Init(void) {
	// Enable the clock of USART 1
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;  // Enable USART 1 clock

	UART1_GPIO_Init();
	USART_Init(USART1);

	//NVIC_SetPriority(USART1_IRQn, 0);			// Set Priority to 1
	//NVIC_EnableIRQ(USART1_IRQn);				// Enable interrupt of USART1 peripheral
}

uint8_t USART_Read (USART_TypeDef * USARTx) {
	// SR_RXNE (Read data register not empty) bit is set by hardware
	while (!(USARTx->SR & USART_SR_RXNE));  // Wait until RXNE (RX not empty) bit is set
	// USART resets the RXNE flag automatically after reading DR
	return ((uint8_t)USARTx->DR); // & 0xFF));
	// Reading USART_DR automatically clears the RXNE flag
}
void USART_Write(USART_TypeDef * USARTx, uint8_t *buffer, uint32_t nBytes) {
	int i;
	// TXE is cleared by a write to the USART_DR register.
	// TXE is set by hardware when the content of the TDR
	// register has been transferred into the shift register.
	for (i = 0; i < nBytes; i++) {
		while (!(USARTx->SR & USART_SR_TXE));   	// wait until TXE (TX empty) bit is set
		// Writing USART_DR automatically clears the TXE flag
		USARTx->DR = buffer[i]; // & 0xFF;
		delay(300);
	}
	while (!(USARTx->SR & USART_SR_TC));   		  // wait until TC bit is set
	USARTx->SR &= ~USART_SR_TC;
}
