#ifndef __TIMER_PWM_H
#define __TIMER_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/



/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif


#define RCC_APB2ENR (*((uint32_t *) (0x40021000 + 0x18)))
#define TIM1EN (0x01 << 11)
#define IOPCEN (0x01 << 4)



#define NVIC_ISER0 (*((uint32_t *) (0xE000E100 + 0x00)))
#define SETENA 0x01


#define TIM1_CR1 (*((uint32_t *) (0x40012C00 + 0x00)))
#define TIM1_SMCR (*((uint32_t *) (0x40012C00 + 0x08)))
#define TIM1_DIER (*((uint32_t *) (0x40012C00 + 0x0C)))
#define TIM1_SR (*((uint32_t *) (0x40012C00 + 0x10)))
#define TIM1_CNT (*((uint32_t *) (0x40012C00 + 0x24)))
#define TIM1_PSC (*((uint32_t *) (0x40012C00 + 0x28)))
#define TIM1_ARR (*((uint32_t *) (0x40012C00 + 0x2C)))
#define UIE 0x01
#define PSC_VALUE 7999 // frekvencija tajmera
#define ARR_VALUE 2
#define CEN 0x01


void init_RCC();
void init_NVIC();
void init_TIM1();

#endif /* __TIMER_PWM_H */
