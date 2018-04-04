/*
 Name:		HU_PCB_SerialNumTool.ino
 Created:	4/4/2018 12:05:11 PM
 Author:	Olly
*/

// Require keyboard control library
#include <KeyboardController.h>

#define EOL_ASCII	19 // (device control 3)

// Initialize USB Controller
USBHost usb;

// Attach keyboard controller to USB
KeyboardController keyboard(usb);

// This function intercepts key release
void keyReleased()
{
	printKey();
}

void printKey()
{
	char key = keyboard.getKey();

	if (key == EOL_ASCII)
	{
		Serial.print("\n");
	}
	else
	{
		Serial.print((char)key);
	}
}

void setup()
{
	Serial.begin(115200);
	Serial.println("Program started");
	delay(200);
}

void loop()
{
	// Process USB tasks
	usb.Task();
}