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
 *	I2C_EEPROM.cpp
 *
 */

// 24LC1025 1024 Kb (128KB) I2C EEPROM

#include <Wire.h>

#include "I2C_EEPROM.h"


////////////////////////////// Public Methods //////////////////////////////
I2C_EEPROM::I2C_EEPROM()
{

}

// initialise Wire and ping the device to confirm operation
bool I2C_EEPROM::begin(void)
{
	//Wire.begin();

	return ping();
}

// check whether EEPROM is responding
bool I2C_EEPROM::ping(void)
{
	uint8_t response;

	Wire.beginTransmission(EEPROM_I2C_ADDR);
	response = Wire.endTransmission();

	if (response == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// read a single byte from location within EEPROM
int I2C_EEPROM::read(int loc)
{
	uint8_t addr, rxByte;

	// get address, including block select bits
	addr = generateAddress(loc);

	// if loc is not valid, return error
	if (addr == 0)
	{
		return (-1);
	}

	Wire.beginTransmission(addr);
	Wire.write((loc >> 8) & 0xFF);		// send high loc
	Wire.write((loc & 0xFF));			// send low loc
	Wire.endTransmission(RESTART);
	Wire.requestFrom(addr, 1);
	rxByte = Wire.read();
	
	return rxByte;
}

// read many pieces of data from the EEPROM (by reading as a page)
int I2C_EEPROM::readMany(int loc, uint8_t* val, int totalToRead)
{
	uint8_t addr;				// I2C address of EEPROM

	uint16_t chunkSize;			// size of the current chunk

	uint16_t valIndex = 0;		// number of values being read
	uint16_t currValLoc = 0;	// location within EEPROM of current value

	// split the data to be read into chunks and read from to the EEPROM
	do
	{
		currValLoc = loc + valIndex;	// location within EEPROM of the current value to be read

		// generate the size of the chunk, taking into account the number of values left to read as well as the page, block and end boundaries
		chunkSize = generateChunkSize(currValLoc, totalToRead - valIndex);

		// get address, including block select bits
		addr = generateAddress(currValLoc);

		// if loc is not valid, return error
		if (addr == 0)
		{
			return (-1);
		}

		// read from the page
		Wire.beginTransmission(addr);
		Wire.write((currValLoc >> 8) & 0xFF);		// send high loc
		Wire.write((currValLoc & 0xFF));			// send low loc
		Wire.endTransmission(RESTART);
		Wire.requestFrom(addr, (int)chunkSize);
		for (int i = 0; i < chunkSize; i++)
		{
			val[valIndex] = Wire.read();
			valIndex++;
		}
		Wire.endTransmission();

	//} while ((loc + valIndex) < totalToRead);
	} while (valIndex < totalToRead);

	return true;			// return success
}

// read many pieces of data from the EEPROM (by reading as a page), with the value passed as an int pointer
int I2C_EEPROM::readMany(int loc, int* val, int totalToRead)
{
	readMany(loc, (uint8_t*)val, totalToRead);
}

// write a single value to EEPROM
int I2C_EEPROM::write(int loc, int val)
{
	uint8_t addr;			// I2C address

	// get address, including block select bits
	addr = generateAddress(loc);

	// if loc is not valid, return error
	if (addr == 0)
		return (-1);

	Wire.beginTransmission(addr);
	Wire.write((loc >> 8) & 0xFF);		// send high loc
	Wire.write((loc & 0xFF));			// send low loc
	Wire.write(val);
	Wire.endTransmission();

	while (!ping());		// wait for the write cycle to be complete

	return true;
}


// write many pieces of data to EEPROM (by writing as a page)
int I2C_EEPROM::writeMany(int loc, uint8_t* val, int totalToWrite)
{
	uint8_t addr;				// I2C address of EEPROM

	uint16_t chunkSize;			// size of the current chunk

	uint16_t valIndex = 0;		// number of values being written
	uint16_t currValLoc = 0;	// location within EEPROM of current value

	// split the data into chunks and write to the EEPROM
	do
	{
		currValLoc = loc + valIndex;	// location within EEPROM of the current value to be written

		// generate the size of the chunk, taking into account the number of values left to write, as well as the page, block and end boundaries
		chunkSize = generateChunkSize(currValLoc, totalToWrite - valIndex);

		// get address, including block select bits
		addr = generateAddress(currValLoc);

		// if loc is not valid, return error
		if (addr == 0)
		{
			return (-1);
		}

		// write to the page
		Wire.beginTransmission(addr);
		Wire.write((currValLoc >> 8) & 0xFF);		// send high loc
		Wire.write((currValLoc & 0xFF));			// send low loc
		for (int i = 0; i < chunkSize; i++)
		{
			Wire.write(val[valIndex]);
			valIndex++;
		}
		Wire.endTransmission();

		while (!ping());	// wait for the write cycle to be complete

	} while (valIndex < totalToWrite);
	//} while ((loc + valIndex) < totalToWrite);


	return true;		// return success
}

// write many pieces of data to EEPROM (by writing as a page), with the value passed as an int pointer
int I2C_EEPROM::writeMany(int loc, int* val, int totalToWrite)
{
	writeMany(loc, (uint8_t*)val, totalToWrite);
}

// only write the value if the value has been changed (return 0 = no update, 1 = update, -1 = error)
int I2C_EEPROM::update(int loc, int val)
{
	uint8_t readVal;

	// read the stored EEPROM value
	readVal = read(loc);

	// if loc is not valid, return error
	if (readVal == 0)
		return (-1);

	// if the new value is different to the stored value
	if (readVal != val)
	{
		write(loc, val);	// store the new value
		return 1;			// return successful update
	}
	else
	{
		return 0;			// return 'no update'
	}
}

// set all values from 'start' to 'stop' to be 'val'
void I2C_EEPROM::writeAll(int loc, int val, int totalToWrite)
{
	int vals[EEPROM_PAGE_SIZE];

	// initialise vals[] array to write to EEPROM
	for (int i = 0; i < EEPROM_PAGE_SIZE; i++)
	{
		vals[i] = val;
	}

	// if totalToWrite is larger than the vals[] array, then write in array chunks
	while ((totalToWrite - loc) > EEPROM_PAGE_SIZE)
	{
		writeMany(loc, vals, EEPROM_PAGE_SIZE);
		loc += EEPROM_PAGE_SIZE;
	}

	// write the remaining values
	writeMany(loc, vals, totalToWrite - loc);
}

// clear all values from 'start' to 'stop'
void I2C_EEPROM::clearAll(int loc, int totalToClear)
{
	writeAll(loc, 255, totalToClear);
}


////////////////////////////// Private Methods //////////////////////////////

// generate the EEPROM address (including block select bits). returns 0 if loc is not valid
uint8_t I2C_EEPROM::generateAddress(int loc)
{
	uint8_t blockNum = loc / EEPROM_BLOCK_SIZE;

	return ((EEPROM_ADDR_C_BITS << 3) | (blockNum << EEPROM_ADDR_B_BIT_POS) | EEPROM_ADDR_A_BITS);		// generate I2C address with block select bit
}

// generate the size of the chunk, taking into account the number of values left to read/write, as well as the page, block and end boundaries
int I2C_EEPROM::generateChunkSize(int currValLoc, int size)
{
#if defined(ARDUINO_AVR_MEGA2560)
	const uint16_t I2C_buff_size = BUFFER_LENGTH - 1;			// buffer size = buffer + I2C_Addr_byte
#elif defined(ARDUINO_ARCH_SAMD)
	const uint16_t  I2C_buff_size = SERIAL_BUFFER_SIZE - 1;		// buffer size = buffer + I2C_Addr_byte
#elif defined(ARDUINO_ARCH_SAM)
	const uint16_t  I2C_buff_size = BUFFER_LENGTH - 1;			// buffer size = buffer + I2C_Addr_byte
#endif

																// constrain the chunk size to be <= I2C_buffer_size
	int chunkSize = constrain(size, 0, I2C_buff_size);

	// if the values fall across multiple pages, then the size of this chunk is the difference between the current loc and the page boundary
	if ((currValLoc / EEPROM_PAGE_SIZE) != ((currValLoc + chunkSize) / EEPROM_PAGE_SIZE))
	{
		// calculate the number of EEPROM locs between the currValLoc and the next page boundary
		if ((EEPROM_PAGE_SIZE - (currValLoc % EEPROM_PAGE_SIZE)) < chunkSize)			// if it less than the current chunk size (< I2C_buff_size)
		{
			chunkSize = EEPROM_PAGE_SIZE - (currValLoc % EEPROM_PAGE_SIZE);
		}
	}

	// if the values fall across multiple blocks, then the size of this chunk is the difference between the current loc and the page boundary
	if ((currValLoc / EEPROM_BLOCK_SIZE) != ((currValLoc + chunkSize) / EEPROM_BLOCK_SIZE))
	{
		// calculate the number of EEPROM locs between the currValLoc and the next block boundary
		if ((EEPROM_BLOCK_SIZE - (currValLoc % EEPROM_BLOCK_SIZE)) < chunkSize)			// if it less than the current chunk size (< I2C_buff_size)
		{
			chunkSize = EEPROM_BLOCK_SIZE - (currValLoc % EEPROM_BLOCK_SIZE);
		}
	}

	// if the values to be written will fall past the end of the total EEPROM memory, then the size of this chunk is the difference between the current loc and the end of the EEPROM memory
	if ((currValLoc + chunkSize) >= EEPROM_TOTAL_SIZE)
	{
		// calculate the number of EEPROM locs between the currValLoc and the end of the EEPROM memory
		if ((EEPROM_TOTAL_SIZE - currValLoc) < chunkSize)			// if it less than the current chunk size (< I2C_buff_size)
		{
			chunkSize = EEPROM_TOTAL_SIZE - currValLoc;
		}
	}

	return chunkSize;
}


I2C_EEPROM EEPROM;