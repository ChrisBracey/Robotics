#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "oi.h"
#include "serial.h"

enum bool {false, true};
typedef unsigned char byte;
unsigned char color = 255;

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
    unsigned char c = 0;
    serialSend(serial, CmdSensors);
    serialSend(serial, 7); //bumps and wheel drops
    serialGetChar(serial, &c);
    c &= 3; //discards wheel drops
    return c;
};

void changeColor(Serial* serial)
{
    serialSend(serial, CmdLeds);
    serialSend(serial, UserButton);
    serialSend(serial, color);
    serialSend(serial, 255);
};

void activateLED(Serial* serial, unsigned char LED)
{
    serialSend(serial, CmdLeds);
    serialSend(serial, LED);
    serialSend(serial, color);
    serialSend(serial, 255);
};

int main(void)
{
    unsigned char count = 16;
    
    unsigned char b = 0;
    
    Serial serial;
    init(&serial, 132); //full mode
    
    // display initial color
    changeColor(&serial);

    while (true)
    {
   	 
		int loopCount = 0;
		
		time_t start = time(NULL);
		
		
		while ((time(NULL) - start) < 1) {
		
		b = bump(&serial);
		
		if (b != 0) {
			if (b == 3) {
				activateLED(&serial, 9);
			}
			else {
				if (b == 2) {
					activateLED(&serial, 8); // check robot led
				}
				if (b == 1) {
					activateLED(&serial, 1); // debris led
				}
			}

		}
		usleep(100000);
		}// end while(loopCount)

		if ((color - count) > 0) { // this works up to count - 1
			color -= count;
			changeColor(&serial);
		} else { // finishes off remaining iteration
			if (color > 0) {
				color = 0;
				changeColor(&serial);
			} else { // color = 0
				color = 255;
				changeColor(&serial);
			}
		}
		
    }
    
    return 0;
}

