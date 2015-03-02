#include "timer.h"
#include "LEDs.h"

#include <avr/interrupt.h>

// GLOBALS
extern uint32_t G_green_ticks;
extern uint32_t G_yellow_ticks;
extern uint32_t G_ms_ticks;

extern uint16_t G_red_period;
extern uint16_t G_green_period;
extern uint16_t G_yellow_period;

extern uint16_t G_release_red;

void init_timers() {

	// -------------------------  RED --------------------------------------//
	// Software Clock Using Timer/Counter 0.
	// THE ISR for this is below.

	// SET appropriate bits in TCCR....
	
	


	// Using CTC mode with OCR0 for TOP. This is mode X, thus WGM0/1/0 = .

	
	// Using pre-scaler XX. This is CS0/2/1/0 = 

	//part2: 8 bit timer with 1ms (1000 Hz) resolution for red led
	
	TCCR0A = (1<< WGM01);
	TCCR0B = (1<< CS02); // prescaler 256

	
	// Software Clock Interrupt Frequency: 1000 = f_IO / (prescaler*OCR0)
	// Set OCR0 appropriately for TOP to generate desired frequency of 1KHz
	printf("Initializing software clock to freq 1000Hz (period 1 ms)\n");	
	OCR0A = 77;


	//Enable output compare match interrupt on timer 0A
	TIMSK0 = (1 << OCIE0A);

	// Initialize counter
	G_ms_ticks = 0;


	//--------------------------- YELLOW ----------------------------------//
	// Set-up of interrupt for toggling yellow LEDs. 
	// This task is "self-scheduled" in that it runs inside the ISR that is 
	// generated from a COMPARE MATCH of 
	//      Timer/Counter 3 to OCR3A.
	// Obviously, we could use a single timer to schedule everything, but we are experimenting here!
	// THE ISR for this is in the LEDs.c file


	// SET appropriate bits in TCCR ...

	// Using CTC mode with OCR3A for TOP. This is mode XX, thus WGM1/3210 = .

	
	// Using pre-scaler XX. This is CS1_210 = 
	// part3: 16 bit timer with 100ms (10 Hz) resolution for yellow led
	
	
	TCCR3A = (1<< COM3A1);
	TCCR3B = (1 << WGM32); // Configure timer 1 for CTC mode
	TCCR3B |= ((1 << CS30) | (1 << CS31)); // Start timer at Fcpu/64
	
	

	// Interrupt Frequency: 10 = f_IO / (prescaler*OCR3A)
	// Set OCR3A appropriately for TOP to generate desired frequency using Y_TIMER_RESOLUTION (100 ms).
	// NOTE: This is not the toggle frequency, rather a tick frequency used to time toggles.
	OCR3A = 31249; //Set CTC compare value to 10Hz, with a prescaler of 64
	printf("Initializing yellow clock to freq %dHz (period %d ms)\n",(int)(OCR3A),Y_TIMER_RESOLUTION);	

	//Enable output compare match interrupt on timer 3A
	TIMSK3 = (1 << OCIE3A); // Enable CTC interrupt

	G_yellow_ticks = 0;


	//--------------------------- GREEN ----------------------------------//
	// Set-up of interrupt for toggling green LED. 
	// This "task" is implemented in hardware, because the OC1A pin will be toggled when 
	// a COMPARE MATCH is made of 
	//      Timer/Counter 1 to OCR1A.
	// We will keep track of the number of matches (thus toggles) inside the ISR (in the LEDs.c file)
	// Limits are being placed on the frequency because the frequency of the clock
	// used to toggle the LED is limited.

	// Using CTC mode with OCR1A for TOP. This is mode XX, thus WGM3/3210 = .
	//DDRD = (1<<PIND5);
	TCCR1A = (1<< COM1A1) | (1 << WGM11); //PWM Fast Mode
	TCCR1B = (1 << WGM13) | (1 << WGM12); // Configure timer 1 for CTC mode

	// Toggle OC1A on a compare match. Thus COM1A_10 = 01

	
	// Using pre-scaler 1024. This is CS1/2/1/0 = XXX

	TCCR1B |= ((1 << CS10) | (1 << CS12)); // Start timer at prescaler 1024
		
	
	// Interrupt Frequency: ? = f_IO / (1024*OCR1A)
	// Set OCR1A appropriately for TOP to generate desired frequency.
	// NOTE: This IS the toggle frequency.
	
	printf("green period %d\n",G_green_period);
	ICR1 = (uint32_t)((19530 / G_green_period) * 2); 
	OCR1A   = (uint16_t) (ICR1 / 2);
	//OCR1A = (uint16_t) (XXXX);
	printf("Set OCR1A to %d\n",OCR1A);
	printf("Initializing green clock to freq %dHz (period %d ms)\n",(int)(OCR1A),G_green_period);	

	// A match to this will toggle the green LED.
	// Regardless of its value (provided it is less than OCR1A), it will match at the frequency of timer 1.
	//OCR1B = 1;

	//Enable output compare match interrupt on timer 1B
	TIMSK1 = (1 << OCIE1A) | (1<<TOIE1);
	

}


//INTERRUPT HANDLERS
 ISR(TIMER0_COMPA_vect) {

	// This is the Interrupt Service Routine for Timer0 (10ms clock used for scheduling red).
	// Each time the TCNT count is equal to the OCR0 register, this interrupt is "fired".

	// Increment ticks
	G_ms_ticks++;

	// if time to toggle the RED LED, set flag to release
	if ( ( G_ms_ticks % G_red_period ) == 0 )
		G_release_red = 1;
}

