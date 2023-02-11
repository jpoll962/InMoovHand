#include "interrupts.h"

void Interrupt_Init()
{
	/*Enable GPIO Clock*/
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

	/*Set moder to input for PC0, PC1, PC4*/
	GPIOC->MODER &= 0xFFFFFCF0;

	/*Set to Pull down for PC0, PC1, PC4*/
	GPIOC->PUPDR &= 0xFFFFFCF0;
	GPIOC->PUPDR |= 0x0000020A;
	
	/*Enable Interrupts*/
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI1_IRQn);
	NVIC_EnableIRQ(EXTI4_IRQn);
	
	/*Red Button*/
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI0);
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PC;
	EXTI->IMR1 |= EXTI_IMR1_IM0;	//Interrupt Mask Register
	EXTI->RTSR1 |= EXTI_RTSR1_RT0;	//Rising trigger selection

	/*Yellow Button*/
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI1);
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PC;
	EXTI->IMR1 |= EXTI_IMR1_IM1;	//Interrupt Mask Register
	EXTI->RTSR1 |= EXTI_RTSR1_RT1;	//Rising trigger selection

	/*Green Button*/
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[1] &= ~(SYSCFG_EXTICR2_EXTI4);
	SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PC;
	EXTI->IMR1 |= EXTI_IMR1_IM4;	//Interrupt Mask Register
	EXTI->RTSR1 |= EXTI_RTSR1_RT4;	//Rising trigger selection
}

/*Initialize SysTick Timer*/
void systick_init(unsigned int ticks)
{
	SysTick->CTRL = 0;	//Disable SysTick
	SysTick->LOAD = ticks - 1;	//Set reload register
	SysTick->VAL = 0;	//Reset SysTick counter value
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;	//Select processor clock
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;	//Enables SysTick interrupt
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;	//Enable SysTick
}

/*SysTick interrupt service routine*/
void SysTick_Handler()
{

}

/*Red Button ISR*/
void EXTI0_IRQHandler(void)
{ 
	rock();
	delay_ms(200);	//Delay
	EXTI->PR1 |= EXTI_PR1_PIF0;
}

/*Yellow Button ISR*/
void EXTI1_IRQHandler(void)
{ 
	paper();
	delay_ms(200);	//Delay
	EXTI->PR1 |= EXTI_PR1_PIF1;
}

/*Green Button ISR*/
void EXTI4_IRQHandler(void)
{
	scissors();
	delay_ms(200);	//Delay
	EXTI->PR1 |= EXTI_PR1_PIF4;
}
