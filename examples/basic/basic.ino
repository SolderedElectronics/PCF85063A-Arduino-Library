#include <Wire.h>
#include "PCF85063A.h"

PCF85063A rtc(&Wire);

void setup() {
  Serial.begin(115200);

  Wire.begin();

  //  setTime(hour, minute, sec);
  rtc.setTime(6, 54, 00);           // 24H mode, ex. 6:54:00
  //  setDate(weekday, day, month, yr);
  rtc.setDate(6, 16, 5, 2020);      // 0 for Sunday, ex. Saturday, 16.5.2020.
}

void loop() {
  printCurrentTime();
  delay(1000);
}

void printCurrentTime() {
  switch( rtc.getWeekday() )
  {
    case 0:
      Serial.print("Sunday , ");
      break;
    case 1:
      Serial.print("Monday , ");
      break;
    case 2:
      Serial.print("Tuesday , ");
      break;
    case 3:
      Serial.print("Wednesday , ");
      break;
    case 4:
      Serial.print("Thursday , ");
      break;
    case 5:
      Serial.print("Friday , ");
      break;
    case 6:
      Serial.print("Saturday , ");
      break;
  }

  Serial.print(rtc.getDay()); Serial.print(".");
  Serial.print(rtc.getMonth()); Serial.print(".");
  Serial.print(rtc.getYear()); Serial.print(". ");
  Serial.print(rtc.getHour()); Serial.print(":");
  Serial.print(rtc.getMinute()); Serial.print(":");
  Serial.println(rtc.getSecond());
}
