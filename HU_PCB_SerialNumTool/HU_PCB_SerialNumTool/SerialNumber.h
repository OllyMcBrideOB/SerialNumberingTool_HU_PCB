
#ifndef SERIAL_NUMBER_H_
#define SERIAL_NUMBER_H_

#include <Arduino.h>


#define SER_NUM_DESCR_VER				11		// version of the serial number descriptor
#define SER_NUM_MAX_LEN					127		// must be less than EEPROM_PAGE_SIZE (128)


// SERIAL NUMBER EEPROM DESCRIPTOR
typedef struct _SerNumDescr_t
{
	uint8_t ver;				// version of serial number descriptor
	uint16_t serNumLen;			// length of following serial number string
} SerNumDescr_t;


class SER_NUM_CLASS
{
	public:

		SER_NUM_CLASS(uint32_t EEPROMLoc);

		String get(void);
		bool set(String serNum);

		bool save(void);
		bool load(void);


	private:
		uint32_t _EEPROMLoc;
		bool _validLoc = false;

		String _serNum;

};



#endif // !SERIAL_NUMBER_H_
