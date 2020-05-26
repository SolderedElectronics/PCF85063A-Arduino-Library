#include "PCF85063A.h"

PCF85063A rtc;
 
uint8_t countdown_time = 5;    // timer countdown in seconds  
  
void setup() {  
  Serial.begin(115200); 

  //  setTime(hour, minute, sec);
  rtc.setTime(6, 54, 00);           // 24H mode, ex. 6:54:00
  //  setDate(weekday, day, month, yr);
  rtc.setDate(6, 16, 5, 2020);      // 0 for Sunday, ex. Saturday, 16.5.2020.

  Serial.print("Now is:" );
}   
  
void loop() {  
  printCurrentTime();
  Serial.print("Setting timer countdown, waking up in "); Serial.print(countdown_time); Serial.println(" seconds.");
  //while(!Serial.available());
  
 /*   source_clock
  *       PCF85063A::TIMER_CLOCK_4096HZ     -> clk = 4096Hz -> min timer = 244uS -> MAX timer = 62.256mS        
  *       PCF85063A::TIMER_CLOCK_64HZ       -> clk = 64Hz   -> min timer = 15.625mS -> MAX timer = 3.984s
  *       PCF85063A::TIMER_CLOCK_1HZ        -> clk = 1Hz    -> min timer = 1s -> MAX timer = 255s
  *       PCF85063A::TIMER_CLOCK_1PER60HZ   -> clk = 1/60Hz -> min timer = 60s -> MAX timer = 4h15min  
  *   value
  *       coundowntime in seconds
  *   int_enable
  *       true = enable interrupt; false = disable interrupt
  *   int_pulse
  *       true = interrupt generate a pulse; false = interrupt follows timer flag
  */
  rtc.timerSet(PCF85063A::TIMER_CLOCK_1HZ, countdown_time, false, false);
  
  Serial.print("Waiting for a countdown");
  while(! rtc.checkTimerFlag() ) {
    Serial.print(".");
    delay(1000);
  } 
 
  Serial.print("\nInterrupt triggered on: ");  
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
