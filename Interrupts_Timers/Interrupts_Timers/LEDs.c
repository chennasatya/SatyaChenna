#include "LEDs.h"
#include "timer.h"

#include <avr/io.h>
#include <stdio.h>

// RED LED is toggled in a cyclic executive defined in lab1.c
// Yellow and Green are toggled in interrupt handlers defined below.
// The interrupts are initialized in the timers.c file.
// Yellow is on Timer 3. Green is on Timer 1.

// GLOBALS
volatile int k,m,n;
extern uint32_t G_yellow_ticks;

extern uint16_t G_red_period;
extern uint16_t G_green_period;
extern uint16_t G_yellow_period;

volatile  uint32_t G_red_toggles;
volatile  uint32_t G_green_toggles;
volatile  uint32_t G_yellow_toggles;

void init_LEDs() {

	

	// Clear all data direction ports
	// Configure data direction as output
	DDRA = (1<<PORTA7);
	DDRA |= DDRA | (1<<PORTA4);
	DDRD = (1<<PIND5);
	


	// Turn LEDs on to make sure they are working


	// leave on for 2 seconds
	//int i;
	//for (i=0;i<200;i++)
		//WAIT_10MS;

	// Start all LEDs off


	// clear toggle counters
	//G_green_toggles = 0;
	//G_red_toggles = 0;
	//G_yellow_toggles = 0;
	
}


void set_toggle(char color, int ms) {

		// check toggle ms is positive and multiple of 100
		if (ms<0) {
			printf("Cannot toggle negative ms.\n");
			return;
		}

		if (~((ms%100)==0)) {
			ms = ms - (ms%100);
			printf("Converted to toggle period: %d.\n",ms);
		}
		
		// For each color, if ms is 0, turn it off by changing data direction to input.
		// If it is >0, set data direction to output.
		if ((color=='R') || (color=='A')) {
			if (ms==0)
				LED_OFF(RED);
			else
				LED_ON(RED);
			G_red_period = ms;
		}

		if ((color=='Y') || (color=='A')) {
			if (ms==0)
				LED_ON(YELLOW);
			else
				LED_OFF(YELLOW);
			G_yellow_period = ms;
		}

		if ((color=='G') || (color=='A')) {
			if (ms==0)
				LED_ON(GREEN);
			else
				LED_OFF(GREEN);

			// green has a limit on its period.
			if ( ms > 4000) ms = 4000;
			G_green_period = ms;
			
			// set the OCR1A (TOP) to get (approximately) the requested frequency.
			if ( ms > 0 ) {
				ICR1 = (uint32_t)((19531 / G_green_period) * 2);
				OCR1A   = (uint16_t) (ICR1 / 2);
				printf("Green to toggle at freq %dHz (period %d ms)\n", OCR1A ,G_green_period);	
			}
 		}
}


 
// INTERRUPT Names are defined in iom1284p.h

// INTERRUPT HANDLER for yellow LED
 ISR(TIMER3_COMPA_vect) {

	// This the Interrupt Service Routine for Toggling the yellow LED.
	// Each time the TCNT count is equal to the OCRxx register, this interrupt is enabled.
	// At creation of this file, it was initialized to interrupt every 100ms (10Hz).
	
	// Increment ticks. If it is time, toggle YELLOW and increment toggle counter.
	G_yellow_ticks++;
	if ((G_yellow_ticks % G_yellow_period) == 0)
	{
		//PORTA ^= (1<<PORTA7);
		LED_TOGGLE(YELLOW);
		G_yellow_toggles++;
		
		//for(k=0;k<51;k++)
		//{
			//WAIT_10MS
		//}
	}
	

}

// INTERRUPT HANDLER for green LED
 ISR(TIMER1_COMPA_vect) {

	// This the Interrupt Service Routine for tracking green toggles. The toggling is done in hardware.
	// Each time the TCNT count is equal to the OCRxx register, this interrupt is enabled.
	// This interrupts at the user-specified frequency for the green LED.
	
	G_green_toggles++;
	
	//for(m=0;m<11;m++)
	//{
		//WAIT_10MS
	//}
}

//Overflow Interrupt Handler for Green led
ISR(TIMER1_OVF_vect)
{
	
	

	G_green_toggles++;
	
	//for(n=0;n<11;n++)
	//{
		//WAIT_10MS
	//}
	
	
}


