#include <pololu/orangutan.h>  
//  
/*  
 * serial2: for the Orangutan SVP and Orangutan X2 controllers.
 *
 * This example listens for serial bytes transmitted via USB to the controller's
 * virtual COM port (USB_COMM).  Whenever it receives a byte, it performs a
 * custom action.  Whenever the user presses the middle button, it transmits a
 * greeting to the virtual COM port.
 *
 * This example will not work on the Orangutan LV, SV, Baby Orangutan, or 3pi robot.
 * 
 * http://www.pololu.com/docs/0J20 
 * http://www.pololu.com  
 * http://forum.pololu.com  
 */   

// receive_buffer: A ring buffer that we will use to receive bytes on USB_COMM.
// The OrangutanSerial library will put received bytes in to
// the buffer starting at the beginning (receiveBuffer[0]).
// After the buffer has been filled, the library will automatically
// start over at the beginning.
char receive_buffer[32];

// receive_buffer_position: This variable will keep track of which bytes in the receive buffer
// we have already processed.  It is the offset (0-31) of the next byte
// in the buffer to process.
unsigned char receive_buffer_position = 0;

// send_buffer: A buffer for sending bytes on USB_COMM.
char send_buffer[32];

//speed at which we need to glow led 
int long rate=0;

unsigned int elapsedTime=0;
unsigned int buttonOn=0;

// wait_for_sending_to_finish:  Waits for the bytes in the send buffer to
// finish transmitting on USB_COMM.  We must call this before modifying
// send_buffer or trying to send more bytes, because otherwise we could
// corrupt an existing transmission.
void wait_for_sending_to_finish()
{
	while(!serial_send_buffer_empty(USB_COMM))
		serial_check();		// USB_COMM port is always in SERIAL_CHECK mode
}

// process_received_byte: Responds to a byte that has been received on
// USB_COMM.  If you are writing your own serial program, you can
// replace all the code in this function with your own custom behaviors.
void process_received_byte(char byte)
{
	clear();		// clear LCD
	print("RX: ");
	print_character(byte);
	lcd_goto_xy(0, 1);	// go to start of second LCD row

	switch(byte)
	{
		// If the character 'G' or 'g' is received, toggle the green LED.
		case 'G':
		case 'g':
			green_led(TOGGLE);
			print("green LED");
			break;

		// If the character 'R' or 'r' is received, toggle the red LED.
		case 'R':
		case 'r':
			red_led(TOGGLE);
			print("red LED");
			break;

		// If the character 'C' or 'c' is received, play the note C.
		case 'C':
		case 'c':
			play_from_program_space(PSTR("c16"));
			print("play note C");
			break;

		// If the character 'D' or 'd' is received, play the note D.
		case 'D':
		case 'd':
			play_from_program_space(PSTR("d16"));
			print("play note D");
			break;
			
	   case '+':
	   rate=200;
	   //calculteTime();
	   //blinkLed();
	   break;
	   
	   case '-':
	   rate=-200;
	   //blinkLed();
	   break;

		// If any other character is received, change its capitalization and
		// send it back.
		default:
			wait_for_sending_to_finish();
			send_buffer[0] = byte ^ 0x20;
			serial_send(USB_COMM, send_buffer, 1);
			print("TX: ");
			print_character(send_buffer[0]);
			break;
	}
}

void check_for_new_bytes_received()
{
	while(serial_get_received_bytes(USB_COMM) != receive_buffer_position)
	{
		// Process the new byte that has just been received.
		process_received_byte(receive_buffer[receive_buffer_position]);

		// Increment receive_buffer_position, but wrap around when it gets to
		// the end of the buffer. 
		if (receive_buffer_position == sizeof(receive_buffer)-1)
		{
			receive_buffer_position = 0;
		}
		else
		{
			receive_buffer_position++;
		}
	}
}

void calculteTime()
{
	unsigned int currenttime = get_ms() ;
	
	unsigned int calc = currenttime - elapsedTime ;
	
	if(calc>2000+rate)
	{
		//print("say");
		print(elapsedTime);
		print_long(currenttime);
		PORTD ^= 0x08;
		elapsedTime = currenttime;
	}
	
	
}

void checkforbutton(unsigned char button)
{
	switch(button)
	{
		case BUTTON_C:
		if(buttonOn==1)
		{
			buttonOn=0;
			PORTD &= ~ 0x08;
		}
		
		else
		{
			buttonOn=1;
		}
		break;
	}
}

void blinkLed()
{
	clear();
	//print("in blink light");
	PORTD = 1<<PORTD3;
	delay_ms(5000+rate);
	//DDRD ^= 0x08;
	PORTD=1>>PORTD3;
	delay_ms(5000+rate);
	rate=0;
	
}




int main()
{
	clear();	// clear the LCD
	print("Send serial");
	lcd_goto_xy(0, 1);	// go to start of second LCD row
	print("or press B");
	DDRD =0x08;
	// Set the baud rate to 9600 bits per second.  Each byte takes ten bit
	// times, so you can get at most 960 bytes per second at this speed.
	serial_set_baud_rate(USB_COMM, 9600);

	// Start receiving bytes in the ring buffer.
	serial_receive_ring(USB_COMM, receive_buffer, sizeof(receive_buffer));

    while(1)
    {
		// USB_COMM is always in SERIAL_CHECK mode, so we need to call this
		// function often to make sure serial receptions and transmissions
		// occur.
		serial_check();

		// Deal with any new bytes received.
		check_for_new_bytes_received();
		unsigned char button = get_single_debounced_button_press(ANY_BUTTON);
		
		if(button)
		{
			//updated with comments
			checkforbutton(button);
			
		}
		if(buttonOn==1)
		{
			calculteTime();
		}
		
				

	}
    
}
