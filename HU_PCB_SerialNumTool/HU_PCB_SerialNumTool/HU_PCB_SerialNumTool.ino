/*
 Name:		HU_PCB_SerialNumTool.ino
 Created:	4/4/2018 12:05:11 PM
 Author:	Olly
*/

#include <Wire.h>
#include <KeyboardController.h>

#include "I2C_EEPROM.h"
#include "SerialNumber.h"

#define SER_NUM_EOL_ASCII			19 // (device control 3)


#define SER_NUM_EEPROM_LOC	128


// Initialize USB Controller
USBHost usb;

// Attach keyboard controller to USB
KeyboardController keyboard(usb);

SER_NUM_CLASS SerNum(SER_NUM_EEPROM_LOC);

String tempSerNum;
bool newSerNum = false;

void setup()
{
	Wire.begin();

	Serial.begin(115200);
	Serial.println("Program started");
	delay(200);
	
	Serial.print("EEPROM... ");
	if (!EEPROM.ping())
	{
		Serial.println("NOT DETECTED");
		while (1);
	}
	Serial.println(" DETECTED");

	Serial.print("SerNum from EEPROM: ");
	SerNum.load();
	Serial.println(SerNum.get());

	//Serial.print("Clearing...");
	//SerNum.set("");
	//SerNum.save();
	//SerNum.load();
	////Serial.print(SerNum.get());
	//Serial.println(" complete");
}

void loop()
{
	// Process USB tasks
	usb.Task();


	if (newSerNum)
	{
		newSerNum = false;
		Serial.println(tempSerNum);

		SerNum.set(tempSerNum);
		SerNum.save();
		Serial.println("Save complete");
		delay(100);
		SerNum.load();

		tempSerNum = "";
	}


	static long lastTime = millis();

	if (millis() - lastTime > 2000)
	{
		Serial.print("Current SerNum: ");
		Serial.println(SerNum.get());

		lastTime = millis();
	}
}


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
		//Serial.print("\n");
		newSerNum = true;
	}
	else
	{
		//Serial.print((char)key);
		tempSerNum += key;
	}
}
