#include "PCF85063A.h"


// INIT
PCF85063A::PCF85063A(TwoWire *_Wire_Interface = &Wire)
{
    Wire_Interface = _Wire_Interface;
}

// PUBLIC
bool
PCF85063A::exist (void)
{
bool RetVal;

Wire_Interface->beginTransmission(PCF85063A_ADDR);
RetVal = (Wire_Interface->endTransmission() == 0);

return RetVal;
}

void PCF85063A::setTime(uint8_t hour, uint8_t minute, uint8_t second)
{
	Wire_Interface->beginTransmission(PCF85063A_ADDR);
	Wire_Interface->write(PCF85063A_SECOND_ADDR);
	Wire_Interface->write(decToBcd( second) );
	Wire_Interface->write(decToBcd( minute) );
	Wire_Interface->write(decToBcd( hour) );
	Wire_Interface->endTransmission();
}

void PCF85063A::setDate(uint8_t weekday, uint8_t day, uint8_t month, uint8_t yr)
{
	year = yr - 1970; 	// convert to RTC year format 0-99

	Wire_Interface->beginTransmission(PCF85063A_ADDR);
	Wire_Interface->write(PCF85063A_DAY_ADDR);
	Wire_Interface->write( decToBcd(day) );
	Wire_Interface->write( decToBcd(weekday) );
	Wire_Interface->write( decToBcd(month) );
	Wire_Interface->write( decToBcd(year) );
	Wire_Interface->endTransmission();
}

void PCF85063A::readTime()
{
	Wire_Interface->beginTransmission(PCF85063A_ADDR);
	Wire_Interface->write(PCF85063A_SECOND_ADDR);					// datasheet 8.4.
	Wire_Interface->endTransmission();

	Wire_Interface->requestFrom(PCF85063A_ADDR, 7);

	while( Wire_Interface->available() )
	{
		second = bcdToDec( Wire_Interface->read() & 0x7F ); 	// ignore bit 7
		minute = bcdToDec( Wire_Interface->read() & 0x7F );
		hour = bcdToDec( Wire_Interface->read() & 0x3F );		// ignore bits 7 & 6
		day = bcdToDec( Wire_Interface->read() & 0x3F );
		weekday = bcdToDec( Wire_Interface->read() & 0x07);	// ignore bits 7,6,5,4 & 3
		month = bcdToDec( Wire_Interface->read() & 0x1F);		// ignore bits 7,6 & 5
		year = bcdToDec( Wire_Interface->read()) + 1970;
	}
}

uint8_t PCF85063A::getSecond()
{
	readTime();
	return second;
}

uint8_t PCF85063A::getMinute()
{
	readTime();
	return minute;
}

uint8_t PCF85063A::getHour()
{
	readTime();
	return hour;
}

uint8_t PCF85063A::getDay()
{
	readTime();
	return day;
}

uint8_t PCF85063A::getWeekday()
{
	readTime();
	return weekday;
}

uint8_t PCF85063A::getMonth()
{
	readTime();
	return month;
}

uint16_t PCF85063A::getYear()
{
	readTime();
	return year;
}

void PCF85063A::enableAlarm() // datasheet 8.5.6.
{
	// check Table 2. Control_2
	control_2 = PCF85063A_CTRL_2_DEFAULT | PCF85063A_ALARM_AIE; 	// enable interrupt
	control_2 &= ~PCF85063A_ALARM_AF;							// clear alarm flag

	Wire_Interface->beginTransmission(PCF85063A_ADDR);
	Wire_Interface->write(PCF85063A_CTRL_2);
	Wire_Interface->write(control_2);
	Wire_Interface->endTransmission();
}

void PCF85063A::setAlarm(uint8_t alarm_second, uint8_t alarm_minute, uint8_t alarm_hour, uint8_t alarm_day, uint8_t alarm_weekday)
{
	if (alarm_second <99) {										// second
        alarm_second = constrain(alarm_second, 0, 59);
        alarm_second = decToBcd(alarm_second);
        alarm_second &= ~PCF85063A_ALARM;
    } else {
        alarm_second = 0x0; alarm_second |= PCF85063A_ALARM;
    }

	if (alarm_minute <99) {										// minute
        alarm_minute = constrain(alarm_minute, 0, 59);
        alarm_minute = decToBcd(alarm_minute);
        alarm_minute &= ~PCF85063A_ALARM;
    } else {
        alarm_minute = 0x0; alarm_minute |= PCF85063A_ALARM;
    }

    if (alarm_hour <99) {										// hour
        alarm_hour = constrain(alarm_hour, 0, 23);
        alarm_hour = decToBcd(alarm_hour);
        alarm_hour &= ~PCF85063A_ALARM;
    } else {
        alarm_hour = 0x0; alarm_hour |= PCF85063A_ALARM;
    }

    if (alarm_day <99) {										// day
        alarm_day = constrain(alarm_day, 1, 31);
        alarm_day = decToBcd(alarm_day);
        alarm_day &= ~PCF85063A_ALARM;
    } else {
        alarm_day = 0x0; alarm_day |= PCF85063A_ALARM;
    }

    if (alarm_weekday <99) {									// weekday
        alarm_weekday = constrain(alarm_weekday, 0, 6);
        alarm_weekday = decToBcd(alarm_weekday);
        alarm_weekday &= ~PCF85063A_ALARM;
    } else {
        alarm_weekday = 0x0; alarm_weekday |= PCF85063A_ALARM;
    }

    enableAlarm();

    Wire_Interface->beginTransmission(PCF85063A_ADDR);
    Wire_Interface->write(PCF85063A_SECOND_ALARM);
    Wire_Interface->write(alarm_second);
    Wire_Interface->write(alarm_minute);
    Wire_Interface->write(alarm_hour);
    Wire_Interface->write(alarm_day);
    Wire_Interface->write(alarm_weekday);
    Wire_Interface->endTransmission();
}

void PCF85063A::readAlarm()
{
	Wire_Interface->beginTransmission(PCF85063A_ADDR);
	Wire_Interface->write(PCF85063A_SECOND_ALARM);					// datasheet 8.4.
	Wire_Interface->endTransmission();

	Wire_Interface->requestFrom(PCF85063A_ADDR, 5);

	while( Wire_Interface->available() )
	{
		alarm_second = Wire_Interface->read(); 				// read PCF85063A_SECOND_ALARM register
		if( PCF85063A_ALARM &  alarm_second)				// check is AEN = 1 (second alarm disabled)
		{
			alarm_second = 99;						// using 99 as code for no alarm
		} else {									// else if AEN = 0 (second alarm enabled)
			alarm_second = bcdToDec( alarm_second & ~PCF85063A_ALARM);	// remove AEN flag and convert to dec number
		}

		alarm_minute = Wire_Interface->read(); 				// minute
		if( PCF85063A_ALARM &  alarm_minute)
		{
			alarm_minute = 99;
		} else {
			alarm_minute = bcdToDec( alarm_minute & ~PCF85063A_ALARM);
		}

		alarm_hour = Wire_Interface->read(); 				// hour
		if( PCF85063A_ALARM &  alarm_hour)
		{
			alarm_hour = 99;
		} else {
			alarm_hour = bcdToDec( alarm_hour & 0x3F);	// remove bits 7 & 6
		}

		alarm_day = Wire_Interface->read(); 				// day
		if( PCF85063A_ALARM &  alarm_day)
		{
			alarm_day = 99;
		} else {
			alarm_day = bcdToDec( alarm_day & 0x3F);	// remove bits 7 & 6
		}

		alarm_weekday = Wire_Interface->read(); 				// weekday
		if( PCF85063A_ALARM &  alarm_weekday)
		{
			alarm_weekday = 99;
		} else {
			alarm_weekday = bcdToDec( alarm_weekday & 0x07);	// remove bits 7,6,5,4 & 3
		}
	}
}

uint8_t PCF85063A::getAlarmSecond()
{
	readAlarm();
	return alarm_second;
}

uint8_t PCF85063A::getAlarmMinute()
{
	readAlarm();
	return alarm_minute;
}

uint8_t PCF85063A::getAlarmHour()
{
	readAlarm();
	return alarm_hour;
}

uint8_t PCF85063A::getAlarmDay()
{
	readAlarm();
	return alarm_day;
}

uint8_t PCF85063A::getAlarmWeekday()
{
	readAlarm();
	return alarm_weekday;
}

void PCF85063A::timerSet(CountdownSrcClock source_clock, uint8_t value, bool int_enable, bool int_pulse)
{
	uint8_t timer_reg[2] = {0};

	// disable the countdown timer
	Wire_Interface->beginTransmission(PCF85063A_ADDR);
	Wire_Interface->write(PCF85063A_TIMER_MODE);
	Wire_Interface->write(0x18);	// default
	Wire_Interface->endTransmission();

	// clear Control_2
	Wire_Interface->beginTransmission(PCF85063A_ADDR);
	Wire_Interface->write(PCF85063A_CTRL_2);
	Wire_Interface->write(0x00);	// default
	Wire_Interface->endTransmission();

	// reconfigure timer
	timer_reg[1] |= PCF85063A_TIMER_TE;						// enable timer
	if (int_enable) timer_reg[1] |= PCF85063A_TIMER_TIE;		// enable interrupr
	if (int_pulse) timer_reg[1] |= PCF85063A_TIMER_TI_TP;		// interrupt mode
	timer_reg[1] |= source_clock << 3;					// clock source
	//timer_reg[1] = 0b00011111;

	timer_reg[0] = value;

	// write timer value
	Wire_Interface->beginTransmission(PCF85063A_ADDR);
	Wire_Interface->write(PCF85063A_TIMER_VAL);
	Wire_Interface->write(timer_reg[0]);
	Wire_Interface->write(timer_reg[1]);
	Wire_Interface->endTransmission();
}

bool PCF85063A::checkTimerFlag()
{
	uint8_t _crtl_2 = PCF85063A_TIMER_FLAG;

	Wire_Interface->beginTransmission(PCF85063A_ADDR);
	Wire_Interface->write(PCF85063A_CTRL_2);
	Wire_Interface->endTransmission();
	Wire_Interface->requestFrom(PCF85063A_ADDR, 1);
	_crtl_2 &= Wire_Interface->read();

	return _crtl_2;
}

void PCF85063A::reset()	// datasheet 8.2.1.3.
{
	Wire_Interface->beginTransmission(PCF85063A_ADDR);
	Wire_Interface->write(PCF85063A_CTRL_1);
	Wire_Interface->write(0x58);
	Wire_Interface->endTransmission();
}


// PRIVATE
uint8_t PCF85063A::decToBcd(uint8_t val)
{
  return ( (val/10*16) + (val%10) );
}

uint8_t PCF85063A::bcdToDec(uint8_t val)
{
  return ( (val/16*10) + (val%16) );
}