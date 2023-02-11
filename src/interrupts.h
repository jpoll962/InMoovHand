#ifndef __STM32L476R_NUCLEO_INTERRUPTS_H
#define __STM32L476R_NUCLEO_INTERRUPTS_H

#include "stm32l476xx.h"
#include "ServoMotor.h"

//Port: GPIOC
//Buttons:
//	- Red (Rock): PC0
//	- Yellow (Paper): PC1
//	- Green (Scissors): PC4

void Interrupt_Init(void);
void systick_init(unsigned int ticks);
void SysTick_Handler(void);
void EXTI0_IRQHandler(void);  
void EXTI1_IRQHandler(void);  
void EXTI4_IRQHandler(void);

#endif
