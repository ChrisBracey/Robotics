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

unsigned char lightBump(Serial* serial)
{
    byte c = 0;
    serialSend(serial, CmdSensors);
    serialSend(serial, 46);
    serialGetChar(serial, &c);
    int w = c;
    w= w << 8;
    byte d = 0;
    serialSend(serial, CmdSensors);
    serialSend(serial, 46);
    serialGetChar(serial, &d);
    w+=d;
    return w;
}

int main(void)
{
    byte b = 0;
    
    Serial serial;
    init(&serial, CmdFull); // Full Mode
    
    while (true)
    {
	    printf("\n\nLight Bump: %d\n\n", lightBump(&serial));
        usleep(1000000);
    }
    
    return 0;
}

