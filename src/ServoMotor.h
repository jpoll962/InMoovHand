#ifndef __STM32L476R_NUCLEO_SERVOMOTOR_H
#define __STM32L476R_NUCLEO_SERVOMOTOR_H

#include "stm32l476xx.h"
#include "interrupts.h"

//Port: GPIOA & GPIOB
//Timer: TIM2
//Input_channel for:
//	- Thumb(PA0): Channel 1
//	- Index Finger(PA1): Channel 2
//	- Middle Finger(PB10): Channel 3
//	- Ring Finger(PB11): Channel 4
//	- Pinky(PA5): Channel 1

extern int PRESCALER;	
extern int AUTO_RELOAD_VALUE;

extern double DUTY_CYCLE_0_DEG;
extern double DUTY_CYCLE_90_DEG;
extern double DUTY_CYCLE_180_DEG;

void PWM_Init(void);
void Servo_Init(void);

void rock(void);
void paper(void);
void scissors(void);
void delay_ms(unsigned int ms);

#endif
