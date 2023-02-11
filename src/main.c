#include "interrupts.h"
#include "ServoMotor.h"
 
int main()
{
	Interrupt_Init();	//initialize button interrupts
	systick_init(4000);	//Initialize SysTick Timer
	PWM_Init();	//Initialize PWM frequency
	Servo_Init();	//Initialize Servo Motors
	
	while(1);
}
