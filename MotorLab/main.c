#include <pololu/orangutan.h>



int main()
{
	
	// Initialization
	
	unsigned char buttonP;
	int motorSpeed, pot, speedRate, reverseStatus, initSpeed;
	speedRate = 10;

	
	pot = read_trimpot();
	motorSpeed = 0;
	reverseStatus = 0;
	
	encoders_init(IO_A6,IO_A7,IO_A4,IO_A5);
	set_motors(0,0);
	

	while(1)
	{
		
		//pot = read_trimpot();
		buttonP = get_single_debounced_button_press(ANY_BUTTON);
		
		switch (buttonP)
		{
			
			case TOP_BUTTON:
			
			if(reverseStatus ==  0)
			{
				motorSpeed += speedRate;
			}
			else if(reverseStatus == 1)
			{
				motorSpeed += (speedRate * -1);
			}
			
			break;
			
			case BOTTOM_BUTTON:
			
			if (reverseStatus == 0)
			{
				motorSpeed -= speedRate;
			}
			else if (reverseStatus == 1)
			{
				motorSpeed -= (speedRate * -1);
			}
			
			break;
			
			case MIDDLE_BUTTON:
			
			reverseStatus =	!reverseStatus;
			motorSpeed = (-1) * motorSpeed;
			break;
			
		}

		if(reverseStatus ==  0)
		{
			
			if (motorSpeed <= speedRate)
			{
				motorSpeed = speedRate;
			}
			if (motorSpeed >= 355)
			{
				motorSpeed = 355;
			}
			
		}
		else if (reverseStatus == 1)
		{
			
			if (motorSpeed >= 0)
			{
				motorSpeed = speedRate * (-1);
			}
			if (motorSpeed <= -355)
			{
				motorSpeed = -355;
			}
		}

		delay_ms(50);
		set_motors(motorSpeed, motorSpeed);

		// print  motor speed
		lcd_goto_xy(0, 1);
		print("spd=");
		print_long(motorSpeed);
		print("  ");              // overwrite any left over digits
		
		// Read the counts for motor 2 and print to LCD.
		lcd_goto_xy(0,0);
		print("Ecnt=");
		print_long(encoders_get_counts_m2());
		print("  ");
		

	}
}


