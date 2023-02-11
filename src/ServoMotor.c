#include "ServoMotor.h"

///////////////////////////////////////////////////////////////////////////
//Use MSI
//PWM Mode 1
//ARR = period
//System clock = 4MHz
//Prescaler = 64 -> 4MHz / 64 = 62,500 = 62.5 kHz
//ARR = (62.5 kHz / 50 Hz) - 1 = 1250 - 1 = 1,249
//Compare register for compare output:
//CCR = (ARR + 1) * 0.5 -> 0.5 represents a 50% duty cycle
//CCR = (ARR + 1) * Duty Cycle
//Calculate the Duty Cycle to calculate the CCR
//Set the CCR, the computer will use that to set the PWM high when desired
//DUTY_CYCLE_MIN = 0.03 -> 3% of 20 ms is when servo is at -90 degrees
//DUTY_CYCLE_MID = 0.078 -> 7.8% of 20 ms is when servo is at 0 degrees
//DUTY_CYCLE_MAX = 0.125	-> 12.5% of 20 ms is when servo is at +90 degrees
////////////////////////////////////////////////////////////////////////////

//Goal is to reach an output of 50 Hz (20 ms) for the total PWM Period
int PRESCALER = 64;	// Set PSC -> PSC = System_Clock_Freq / __ Hz -> The __ Hz can be randomly chosen to get closer to the desired ARR
//ARR = __ Hz / 50 Hz -> This is the same __ Hz as in the line above and 50 Hz is the desired PWM period
int AUTO_RELOAD_VALUE = 1250 - 1;	// PWM period = (ARR + 1) * 1/(System_Clock_Freq / PSC) = __ sec = __ Hz

/*Set Duty Cycle -> CCR = (ARR + 1) * DUTY_CYCLE*/
double DUTY_CYCLE_0_DEG = ((100 * 0.0006) / 0.02) / 100;	//To go right -> Use 0.6 ms -> Duty Cycle = (100% * 0.6 ms) / 20 ms = 3% = 0.03
double DUTY_CYCLE_90_DEG = ((100 * 0.00156) / 0.02) / 100;	//To point upward -> Use 1.56 ms -> Duty Cycle = (100% * 1.56 ms) / 20 ms = 7.8% = 0.078
double DUTY_CYCLE_180_DEG = ((100 * 0.0025) / 0.02) / 100;	//To go right -> Use 0.6 ms -> Duty Cycle = (100% * 0.6 ms) / 20 ms = 3% = 0.03

void PWM_Init(void)
{
	/*Enable MSI as System Clock*/
	RCC->CR |= RCC_CR_MSION;	//Clock init RCC_CR using MSI
	while (!(RCC->CR&RCC_CR_MSIRDY));	//Wait until MSI is ready
	
	/*Enable Peripheral Clocks via RCC Registers*/
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;	// Enable GPIOA RCC
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;	// Enable GPIOB RCC
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;	// Enable TIM2 RCC
	
	/*Configure GPIO (Pins PA0, PA1, PA5, PB10, PB11)*/
	GPIOA->MODER &= 0xFFFFF3F0;	//Clear bits in PA0, PA1, and PA5
	GPIOA->MODER |= 0x0000080A;	//Set PA0, PA1, and PA5 set to Alternate Function (10)
	
	GPIOB->MODER &= 0xFF0FFFFF;	//Clear bits in PB10 and PB11
	GPIOB->MODER |= 0x00A00000;	//Set PB10 and PB11 to Alternate Function (10)
	
	GPIOA->AFR[0] &= 0xFF0FFF00;	//Clear bits in PA0, PA1, and PA5
	GPIOA->AFR[0] |= 0x00101111;	//Set alternate function pins (PA0, PA1, PA5) as TIM2_CHx (AF1)
	
	GPIOB->AFR[1] &= 0xFFFF00FF;	//Clear bits in PB10 and PB11
	GPIOB->AFR[1] |= 0x00001100;	//Set alternate function pins (PB10 and PB11) as TIM2_CHx (AF1)
	
	/*Set pins to push-pull with no pull-up and no pull-down*/
	//The reset-state of the microcontroller is push-pull, so no need to invoke OTYPER
	GPIOA->PUPDR &= 0xFFFFF3F0;	//Clear bits
	GPIOA->PUPDR |= 0x00000000;	//Set to no pull-up or pull-down
	GPIOB->PUPDR &= 0xFF0FFFFF;	//Clear bits PB10 and PB11
	GPIOB->PUPDR |= 0x00000000;	//Set to no pull-up or pull-down
}

/*Configure Timers(TIM2)*/
void Servo_Init(void)
{
	/*Select the counting direction: 0 = up-counting, 1 = down-counting*/
	TIM2->CR1 &= ~TIM_CR1_DIR;	//Set to up-counting
	
	TIM2->PSC = (uint32_t)PRESCALER;	//Prescaler, slow down the input clock by a factor of (1 + prescaler)
	TIM2->ARR = (uint32_t)AUTO_RELOAD_VALUE;	//Auto-reload
	
	/*Channel 1*/
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;	//Clear output compare bits
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;	//Select PWM Mode 1 output (OC1M = 110)
	
	/*Channel 2*/
	TIM2->CCMR1 &= ~TIM_CCMR1_OC2M;	//Clear output compare bits
	TIM2->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;	//Select PWM Mode 1 output (OC2M = 110)
	
	/*Channel 3*/
	TIM2->CCMR2 &= ~TIM_CCMR2_OC3M;	//Clear output compare bits
	TIM2->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;	//Select PWM Mode 1 output (OC3M = 110)
	
	/*Channel 4*/
	TIM2->CCMR2 &= ~TIM_CCMR2_OC4M;	//Clear output compare bits
	TIM2->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;	//Select PWM Mode 1 output (OC4M = 110)
	
	TIM2->CR1 |= TIM_CR1_ARPE;	//Enable TIM2_ARR register preload
	
	/*Enable Output*/
	TIM2->CCER |= TIM_CCER_CC1E;	//Channel 1
	TIM2->CCER |= TIM_CCER_CC2E;	//Channel 2
	TIM2->CCER |= TIM_CCER_CC3E;	//Channel 3
	TIM2->CCER |= TIM_CCER_CC4E;	//Channel 4
	
	/*Select output polarity: 0 = Active high, 1 = Active Low*/
	TIM2->CCER &= ~TIM_CCER_CC1P;	//Channel 1
	TIM2->CCER &= ~TIM_CCER_CC2P;	//Channel 2
	TIM2->CCER &= ~TIM_CCER_CC3P;	//Channel 3
	TIM2->CCER &= ~TIM_CCER_CC4P;	//Channel 4
	
	TIM2->CR1 |= TIM_CR1_CEN;	//Enable counter
	
}

/*--Hand Orientation--*/
/*Rock (Hand closed in Fist)*/
//	- Thumb(PA0->CH1): Closed (180 degrees)
//	- Index Finger(PA1->CH2): Closed (180 degrees)
//	- Middle Finger(PB10->CH3): Closed (180 degrees)
//	- Ring Finger(PB11->CH4): Closed (180 degrees)
//	- Pinky(PA5->CH1): Closed (180 degrees)

/*Paper (Hand open flat)*/
//	- Thumb(PA0->CH1): Open (0 degrees)
//	- Index Finger(PA1->CH2): Open (0 degrees)
//	- Middle Finger(PB10->CH3): Open (0 degrees)
//	- Ring Finger(PB11->CH4): Open (0 degrees)
//	- Pinky(PA5->CH1): Open (0 degrees)

/*Scissors (Index and Middle Finger straight; remainder closed)*/
//	- Thumb(PA0->CH1): Closed (180 degrees)
//	- Index Finger(PA1->CH2): Open (0 degrees)
//	- Middle Finger(PB10->CH3): Open (0 degrees)
//	- Ring Finger(PB11->CH4): Closed (180 degrees)
//	- Pinky(PA5->CH1): Closed (180 degrees)

/*Rock (Hand closed in Fist)*/
void rock(void)
{
	/*Channel 1	(Thumb, Pinky)*/
	TIM2->CCR1 = (uint32_t)((AUTO_RELOAD_VALUE + 1) * DUTY_CYCLE_180_DEG);	//Move Servo to +90 degrees
	
	/*Channel 2 (Index Finger)*/
	TIM2->CCR2 = (uint32_t)((AUTO_RELOAD_VALUE + 1) * DUTY_CYCLE_180_DEG);	//Move Servo to +90 degrees
	
	/*Channel 3 (Middle Finger)*/
	TIM2->CCR3 = (uint32_t)((AUTO_RELOAD_VALUE + 1) * DUTY_CYCLE_180_DEG);	//Move Servo to +90 degrees
	
	/*Channel 4 (Ring Finger)*/
	TIM2->CCR4 = (uint32_t)((AUTO_RELOAD_VALUE + 1) * DUTY_CYCLE_180_DEG);	//Move Servo to +90 degrees
}

/*Paper (Hand open flat)*/
void paper(void)
{
	/*Channel 1 (Thumb, Pinky)*/
	TIM2->CCR1 = (uint32_t)((AUTO_RELOAD_VALUE + 1) * DUTY_CYCLE_0_DEG);	//Move Servo to 0 degrees
	
	/*Channel 2 (Index Finger)*/
	TIM2->CCR2 = (uint32_t)((AUTO_RELOAD_VALUE + 1) * DUTY_CYCLE_0_DEG);	//Move Servo to 0 degrees

	/*Channel 3 (Middle Finger)*/
	TIM2->CCR3 = (uint32_t)((AUTO_RELOAD_VALUE + 1) * DUTY_CYCLE_0_DEG);	//Move Servo to 0 degrees
	
	/*Channel 4 (Ring Finger)*/
	TIM2->CCR4 = (uint32_t)((AUTO_RELOAD_VALUE + 1) * DUTY_CYCLE_0_DEG);	//Move Servo to 0 degrees
}

//Scissors (Index and Middle Finger straight; remainder closed)
void scissors(void)
{
	/*Channel 1	(Thumb, Pinky)*/
	TIM2->CCR1 = (uint32_t)((AUTO_RELOAD_VALUE + 1) * DUTY_CYCLE_180_DEG);	//Move Servo to +90 degrees
	
	/*Channel 2 (Index Finger)*/
	TIM2->CCR2 = (uint32_t)((AUTO_RELOAD_VALUE + 1) * DUTY_CYCLE_0_DEG);	//Move Servo to 0 degrees
	
	/*Channel (Middle Finger)*/
	TIM2->CCR3 = (uint32_t)((AUTO_RELOAD_VALUE + 1) * DUTY_CYCLE_0_DEG);	//Move Servo to 0 degrees
	
	/*Channel 4 (Ring Finger)*/
	TIM2->CCR4 = (uint32_t)((AUTO_RELOAD_VALUE + 1) * DUTY_CYCLE_180_DEG);	//Move Servo to +90 degrees
}

/*Delay function*/
void delay_ms(unsigned int ms)
{
	unsigned int i, j;
	for(i=0; i < ms; i++)
	{
		volatile int temp = 0;
		for(j=0;j<800;j++)
		{
			temp++;
		}
	}
}
