#ifndef __USART_H
#define __USART_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f103x6.h"

/* Exported functions prototypes ---------------------------------------------*/
void UART1_Init();
void USART_Write(USART_TypeDef * USARTx, uint8_t *buffer, uint32_t nBytes);
/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __USART_H */
