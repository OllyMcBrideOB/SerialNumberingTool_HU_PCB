

#ifndef WRITE_ANYTHING_H_
#define WRITE_ANYTHING_H_

#include "I2C_EEPROM.h"

// write any data type to EEPROM
template <class T>
int EEPROM_writeAnyType(int loc, const T& value)
{
	const byte* p = (const byte*)(const void*)&value;

	EEPROM.writeMany(loc, (uint8_t*)p, sizeof(value));

	return sizeof(value);
}

// write any data type to EEPROM of a set size
template <class T>
int EEPROM_writeAnyType(int loc, const T& value, int size)
{
	const byte* p = (const byte*)(const void*)&value;

	EEPROM.writeMany(loc, (uint8_t*)p, size);

	return size;
}


// read any data type from EEPROM
template <class T>
int EEPROM_readAnyType(int loc, const T& value)
{
	byte* p = (byte*)(void*)&value;

	EEPROM.readMany(loc, (uint8_t*)p, sizeof(value));

	return sizeof(value);
}


// read any data type from EEPROM of a set size
template <class T>
int EEPROM_readAnyType(int loc, const T& value, int size)
{
	byte* p = (byte*)(void*)&value;

	EEPROM.readMany(loc, (uint8_t*)p, size);

	return size;
}

#endif // !WRITE_ANYTHING_H_
