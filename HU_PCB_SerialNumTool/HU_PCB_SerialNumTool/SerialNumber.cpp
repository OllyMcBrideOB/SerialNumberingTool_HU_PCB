#include "SerialNumber.h"

#include "WriteAnything.h"





SER_NUM_CLASS::SER_NUM_CLASS(uint32_t EEPROMLoc)
{
	_EEPROMLoc = EEPROMLoc;
	_validLoc = true;
}

String SER_NUM_CLASS::get(void)
{
	return _serNum;
}

bool SER_NUM_CLASS::set(String serNum)
{
	_serNum = serNum;
}


bool SER_NUM_CLASS::save(void)
{
	if (!_validLoc)
	{
		Serial.println("Unable to save, no valid EEPROM loc set");
		return false;
	}


	SerNumDescr_t SNumDesc;

	SNumDesc.ver = SER_NUM_DESCR_VER;
	SNumDesc.serNumLen = _serNum.length();

	// create a char array of serial number string
	char charBuff[SER_NUM_MAX_LEN];
	_serNum.toCharArray(charBuff, SNumDesc.serNumLen + 1);		// (allow for null terminator)

	//Serial.print("Saving SNum: ");
	//Serial.print(_serNum);
	//Serial.print("  len: ");
	//Serial.print(SNumDesc.serNumLen);
	//Serial.print("  loc: ");
	//Serial.println(_EEPROMLoc);

	// save the serial number descriptor
	EEPROM_writeAnyType(_EEPROMLoc, SNumDesc);

	// save the grip string after the descriptor
	EEPROM_writeAnyType(_EEPROMLoc + sizeof(SNumDesc), charBuff, SNumDesc.serNumLen);
}

bool SER_NUM_CLASS::load(void)
{
	if (!_validLoc)
	{
		Serial.println("Unable to save, no valid EEPROM loc set");
		return false;
	}

	SerNumDescr_t SNumDesc;

	// create a char array for serial number string
	char charBuff[SER_NUM_MAX_LEN];

	// load the serial number descriptor
	EEPROM_readAnyType(_EEPROMLoc, SNumDesc);

	//Serial.print("loaded SNumDesc  ver: ");
	//Serial.print(SNumDesc.ver);
	//Serial.print("  serNumLen: ");
	//Serial.print(SNumDesc.serNumLen);

	// if the serial number descriptor version is compatible
	if (SNumDesc.ver == SER_NUM_DESCR_VER)
	{
		if (SNumDesc.serNumLen == 0)
		{
			//Serial.print("   EEPROM SerNum is blank");
			_serNum = "";
		}
		else
		{
			// load the serial number
			EEPROM_readAnyType(_EEPROMLoc + sizeof(SNumDesc), charBuff, SNumDesc.serNumLen);

			// store char buff in the serial number string
			_serNum = "";
			_serNum = charBuff;
			_serNum = _serNum.substring(0, SNumDesc.serNumLen);	// limit string length
		}

		//Serial.print("   _serNum: ");
		//Serial.println(_serNum);

		return true;
	}
	else
	{
		//Serial.println(" version not compatible. NOT LOADED");

		return false;
	}
}
