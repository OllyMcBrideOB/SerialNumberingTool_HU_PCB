/*	Open Bionics - Carrot HC
*	Author - Olly McBride
*	Date - February 2017
*
*	This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License.
*	To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/.
*
*	Website - http://www.openbionics.com/
*	GitHub - https://github.com/Open-Bionics
*	Email - ollymcbride@openbionics.com
*
*	I2C_EEPROM.h
*
*/

/* 24LC1025 1024 Kb (128KB) I2C EEPROM
*
*	Address 
*	- C3 C2 C1 C0 B0 A1 A0 (1010 = ctrl code, B0 = block select, A1 & A0 = hardware addr select)
*	- C3->C0	ctrl code (1010)
*	- B0		block select bit (1)
*	- A1->A0	address select bit (01)
*
*	Details
*	- 2 pages of 64K bytes
*	- 128 bytes per page
*  
*/
#ifndef I2C_EEPROM_H_
#define I2C_EEPROM_H_

#include <Arduino.h>

// EEPROM SETTINGS
#define EEPROM_I2C_ADDR			0x51	// first address of device block (occupies addresses 0x51 & 0x52)
#define EEPROM_NUM_BLOCKS		2		// number of memory blocks (of size EEPROM_BLOCK_SIZE)
#define EEPROM_TOTAL_SIZE		128000	// bytes. total size of the EEPROM
#define EEPROM_BLOCK_SIZE		64000	// bytes. size of each block
#define EEPROM_PAGE_SIZE		128		// bytes. size of each page (for page write)

#define EEPROM_ADDR_C_BITS		0x0A	// EEPROM address control bits (B1010)
#define EEPROM_ADDR_B_BIT_POS	2		// position of the block select bit within byte
#define EEPROM_ADDR_A_BITS		0x01	// hardware selectable address bits (B01)

#define EEPROM_TOTAL_SIZE		(EEPROM_BLOCK_SIZE * EEPROM_NUM_BLOCKS)		// total size of EEPROM

// WIRE FLAG
#define RESTART	0		// send RESTART instead of STOP

// CLASS 
class I2C_EEPROM
{	
	public:
		I2C_EEPROM();

		bool begin(void);			// initialise Wire and ping the device to confirm operation, return false if fails
		bool ping(void);			// check whether EEPROM is responding, returns false if no response

		int read(int loc);										// read a single value from EEPROM
		int readMany(int loc, int* val, int totalToRead);		// read many pieces of data from the EEPROM (by reading as a page)
		int readMany(int loc, uint8_t* val, int totalToRead);	// read many pieces of data from the EEPROM (by reading as a page), with the value passed as an int pointer

		int write(int addr, int val);							// write a single value to EEPROM
		int writeMany(int loc, int* val, int totalToWrite);		// write many pieces of data to EEPROM (by writing as a page)
		int writeMany(int loc, uint8_t* val, int totalToWrite);	// write many pieces of data to EEPROM (by writing as a page), with the value passed as an int pointer
		void writeAll(int loc, int val, int totalToWrite);		// set all values from 'start' to 'stop' to be 'val'
		void clearAll(int loc, int totalToClear);				// clear all values from 'start' to 'stop'

		int update(int loc, int val);							// only write the value if the value has been changed (return 0 = no update, 1 = update, -1 = error)

	private:

		uint8_t generateAddress(int loc);						// generate the EEPROM address, including block select bits (returns 0 if loc is not valid)
		int generateChunkSize(int currValLoc, int size);		// generate the size of the chunk, taking into account the page, block and end boundaries
};

extern I2C_EEPROM EEPROM;

#endif // I2C_EEPROM_H_