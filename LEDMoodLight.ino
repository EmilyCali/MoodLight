#include <EEPROM.h>

//Modes of operation
enum LightMode {ON, BREATHE, FLASH, STROBE};
LightMode mode;

bool onoff = false;
long timeout;
int timeV;

void setup() {
  // Increment the memory in the first byte of the EEPROM, this is the mode
  EEPROM.write(0,(EEPROM.read(0)+1)%4);
  
  //Start Serial comms
  Serial.begin(9600);
  
  //Get the current mode
  mode = LightMode((EEPROM.read(0)+1)%4);
  timeout = millis();
  
  //Set the time values for each mode in millis
  switch(mode)
  {
    case STROBE:
      timeV = 50;
      break;
    case FLASH:
      timeV = 500;
      break;
    default:
      timeV = 100;
      break; 
  }
}

void loop() {
  // Read the potentiometers to get the brightness of the R, G, and B channels
  int value1 = getValue(analogRead(A0));
  int value2 = getValue(analogRead(A5));
  int value3 = getValue(analogRead(A2));
  
  //If on, PWM the LEDs to get the color desired
  if (mode == ON)
  {
    analogWrite(3, value1);
    analogWrite(5, value2);
    analogWrite(6, value3);
	
  //If flash or strobe, figure out if we have waited the appropriate amount of time
  } else if (mode == FLASH || mode == STROBE) {
    if (millis() > timeout || (millis() < timeout && ((timeout-millis()) > timeV)))
    {
		//Turn off if on now
      if (onoff)
      {
        analogWrite(3, 0);
        analogWrite(5, 0);
        analogWrite(6, 0);
		//Otherwise turn on if off
      } else {
        analogWrite(3, value1);
        analogWrite(5, value2);
        analogWrite(6, value3);
      }
	  //Set the mode to reflect the current
      onoff = !onoff;
	  
	  //Set new time to wait to
      timeout = millis() + timeV;
    }
	//If breathe do the funky calculations
  } else if (mode == BREATHE) {
    int cTime = millis()%5120;
    long value;
    if (cTime < 2560)
    {
      value = getValue(int(cTime*4/10));
    } else {
      value = getValue(int((5120-cTime)*4/10));
    }
    if (value < 0) value = 0;
    analogWrite(3, int(value*255./float(value1)));
    analogWrite(5, int(value*255./float(value2)));
    analogWrite(6, int(value*255./float(value3))); 
  }
}

//Potentiometer voltage calculations
int getValue(int value)
{
  //delay(500);
  //Serial.println("Value: " + String(value));
  unsigned long newVal = long(value)*long(value)*long(value);
  //Serial.println("newVal: " + String(newVal));
  newVal = newVal/4200000;
  //Serial.println( "final: " + String(newVal));
  return int(newVal);
}
