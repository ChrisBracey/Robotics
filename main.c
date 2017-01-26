#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "oi.h"
#include "serial.h"

enum bool {false, true};
typedef unsigned char byte; //Makes byte = unsigned char
byte color = 255; //global variable for color

void init(Serial* serial, byte state)
{
    serialClose(serial);
    serialOpen(serial, "/dev/ttyUSB0", B115200, true);
    serialSend(serial, 128);    // Send Start
    serialSend(serial, state);    // Send state

    byte b;
    while( serialNumBytesWaiting(serial) > 0 )
   	 serialGetChar(serial, &b);
};

unsigned char bump(Serial* serial)
{
    byte c = 0;
    serialSend(serial, CmdSensors);
    serialSend(serial, 7); //bumps and wheel drops
    serialGetChar(serial, &c);
    c &= 3; //discards wheel drops
    return c;
};

void LED(Serial* serial, byte LED)
{
    serialSend(serial, CmdLeds);
    serialSend(serial, LED);
    serialSend(serial, color);
    serialSend(serial, 255);
};

int main(void)
{
    byte decrease = 16;
    byte b = 0;
    
    Serial serial;
    init(&serial, CmdFull); // Full Mode
    
    // display initial color
    LED(&serial, UserButton);

    while (true)
    {
		time_t start = time(NULL); // Gets current Time
		
		while ((time(NULL) - start) < 1) { // Do for the current second
		    
		    b = bump(&serial); // Gets bump value
		
		    if (b != 0) { // If a bumper was bumped
			    if (b == 3) { // If bumped in middle
				    LED(&serial, 9); // Check Robot and Debris
			    }
			    if (b == 2) { // If bumped on right
				    LED(&serial, 8); // check robot led
			    }
			    if (b == 1) { // If bumped on left
				    LED(&serial, 1); // debris led
                }
		    }
		    usleep(100000); // Sleep for 1/10 of a second
		}

		if ((color - decrease) > 0) {
			color -= decrease; //Decrement color but 16
		    LED(&serial, UserButton); // Change LED Color
		} else { 
			if (color > 0) { // if color == 15
				color = 0;
				LED(&serial, UserButton);
			} else { // Resets color back to red after it hits green
                color = 255;
				LED(&serial, UserButton);
			}
		}
		
    }
    
    return 0;
}

