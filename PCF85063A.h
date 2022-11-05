#ifndef PCF85063A_H
#define PCF85063A_H

#include "Arduino.h"
#include "Wire.h"

#define PCF85063A_ADDR            0x51

// registar overview - crtl & status reg
#define PCF85063A_CTRL_1 			0x0
#define PCF85063A_CTRL_2 			0x01
#define PCF85063A_OFFSET 			0x02
#define PCF85063A_RAM_by 			0x03
// registar overview - time & data reg
#define PCF85063A_SECOND_ADDR		0x04
#define PCF85063A_MINUTE_ADDR		0x05
#define PCF85063A_HOUR_ADDR		0x06
#define PCF85063A_DAY_ADDR		0x07
#define PCF85063A_WDAY_ADDR		0x08
#define PCF85063A_MONTH_ADDR		0x09
#define PCF85063A_YEAR_ADDR		0x0A	// years 0-99; calculate real year = 1970 + RCC reg year
// registar overview - alarm reg
#define PCF85063A_SECOND_ALARM	0x0B
#define PCF85063A_MINUTE_ALARM	0x0C
#define PCF85063A_HOUR_ALARM		0x0D
#define PCF85063A_DAY_ALARM		0x0E
#define PCF85063A_WDAY_ALARM		0x0F
// registar overview - timer reg
#define PCF85063A_TIMER_VAL 		0x10
#define PCF85063A_TIMER_MODE		0x11
#define PCF85063A_TIMER_TCF        0x08
#define PCF85063A_TIMER_TE         0x04
#define PCF85063A_TIMER_TIE        0x02
#define PCF85063A_TIMER_TI_TP      0x01
// format
#define PCF85063A_ALARM 			0x80	// set AEN_x registers
#define PCF85063A_ALARM_AIE		0x80 	// set AIE ; enable/disable interrupt output pin
#define PCF85063A_ALARM_AF		0x40 	// set AF register ; alarm flag needs to be cleared for alarm
#define PCF85063A_CTRL_2_DEFAULT	0x00
#define PCF85063A_TIMER_FLAG		0x08

class PCF85063A {
	public:
		PCF85063A(TwoWire *_Wire_Interface);

	    enum CountdownSrcClock {TIMER_CLOCK_4096HZ   = 0,
                 		TIMER_CLOCK_64HZ     = 1,
                 		TIMER_CLOCK_1HZ      = 2,
                 		TIMER_CLOCK_1PER60HZ = 3 };

        bool exist (void);

		void setTime(uint8_t hour, uint8_t minute, uint8_t sec);
		void setDate(uint8_t weekday, uint8_t day, uint8_t month, uint8_t yr);
		void readTime();
		void enableAlarm();	// called on setAlarm()
		void setAlarm(uint8_t alarm_second, uint8_t alarm_minute, uint8_t alarm_hour, uint8_t alarm_day, uint8_t alarm_weekday);
		void readAlarm();
		bool changeTimeFormat(); // returns false for 24H format & true for 12H format
		void timerSet(CountdownSrcClock source_clock, uint8_t value, bool int_enable, bool int_pulse);
		bool checkTimerFlag();
		void reset();
		/* read RTC times */
		uint8_t getSecond();
		uint8_t getMinute();
		uint8_t getHour();
		uint8_t getDay();
		uint8_t getWeekday();
		uint8_t getMonth();
		uint16_t getYear();
		/* read alarm times */ 			// if return = 99, no alarm
		uint8_t getAlarmSecond();
		uint8_t getAlarmMinute();
		uint8_t getAlarmHour();
		uint8_t getAlarmDay();
		uint8_t getAlarmWeekday();

	private:
        TwoWire *Wire_Interface;
		uint8_t decToBcd(uint8_t val);
		uint8_t bcdToDec(uint8_t val);
		/* time variables */
		uint8_t hour;
		uint8_t minute;
		uint8_t second;
		uint8_t day;
		uint8_t weekday;
		uint8_t month;
		uint16_t year;
		/* alarm */
		uint8_t alarm_second;
		uint8_t alarm_minute;
		uint8_t alarm_hour;
		uint8_t alarm_day;
		uint8_t alarm_weekday;
		/* support */
		uint8_t control_2;
};
#endif