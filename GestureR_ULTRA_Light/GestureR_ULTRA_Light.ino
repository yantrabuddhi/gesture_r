#include <Wire.h>
#include <PCA9685.h>

PCA9685 ledDriver; 

unsigned char ALS_mode=0;
unsigned int lux=0;

void I2C_write(unsigned char add,unsigned char reg,unsigned char data) {
  Wire.beginTransmission(add);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission(1);  
}

unsigned short RGB_Cnt=0;
unsigned short RGB_Cnt2=0;
boolean RGB_Plus=true;
void RGB_LED() {
  if (RGB_Plus) {
    RGB_Cnt++;
  } 
  else {
    RGB_Cnt--; 
  }
  ledDriver.setLEDDimmed(RGB_Cnt2, RGB_Cnt*2);
  if (RGB_Cnt>=50) {
    RGB_Plus=false;
  } 
  else if (RGB_Cnt<1) {
    RGB_Plus=true;
    (RGB_Cnt2>=2)?(RGB_Cnt2=0):(RGB_Cnt2++);
  }
}

const unsigned int delay_value=250;
void LED_Blink() {
  unsigned char i;
  for(i=3; i<15; i++)  {
    ledDriver.setLEDOn(i); 
  }
  delay(delay_value);
  for(i=3; i<15; i++)  {
    ledDriver.setLEDOff(i); 
  }
  delay(delay_value); 
}

void LED_Off() {
  unsigned char i;
  for(i=3; i<15; i++)  {
    ledDriver.setLEDOff(i); 
  }
}

void LED_On() {
  unsigned char i;
  for(i=3; i<15; i++)  {
    ledDriver.setLEDOn(i); 
  }
}

void setup()
{
  ////////IO Init
  pinMode(12, INPUT_PULLUP);  
  pinMode(13, INPUT_PULLUP);  
  pinMode(10, OUTPUT);               
  digitalWrite(10, LOW);  
  pinMode(11, OUTPUT);               
  digitalWrite(11, LOW); 
  pinMode(9, OUTPUT);               
  digitalWrite(9, LOW);  
  pinMode(8, INPUT_PULLUP);
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(115200);  // start serial for output

  ///////LED Driver Init
  ledDriver.begin(B000000);  // Address pins A5-A0 set to B111000
  ledDriver.init();
  ledDriver.setLEDOff(0); 
  ledDriver.setLEDOff(1); 
  ledDriver.setLEDOff(2); 

  ///////Sensor Init
  Wire.beginTransmission(0x39);
  Wire.write(0x00);
  Wire.write(0xd0);
  Wire.write(0x25);
  Wire.write(0x40);
  Wire.write(0x80);
  Wire.endTransmission(1);

  /////All Leds Blinks two times
  LED_Blink();
  LED_Blink();
}

void loop()
{
  ////Getting Light Data from Sensor
  unsigned short AD0=0,AD1=0;
  Wire.beginTransmission(0x39);
  Wire.write(0x0c);
  Wire.endTransmission(0);
  Wire.requestFrom(0x39, 4);    // request 4 bytes from slave device

  unsigned char i=0;
  while(Wire.available())
  { 
    unsigned char c = Wire.read(); // receive a byte as character
    switch (i) {
    case 0:
      AD0=c;
      break;  
    case 1:
      AD0+=(((unsigned short)c)<<8);
      break; 
    case 2:
      AD1=c;
      break; 
    case 3:
      AD1+=(((unsigned short)c)<<8);
      break; 
    }
    i++;
  }

  //// Calculation & get light results ////
  if (ALS_mode==0) {
    if (AD0>16000) {
      ALS_mode=1;
      I2C_write(0x39,0x00,0x0c);
      I2C_write(0x39,0x01,0x27);
      I2C_write(0x39,0x00,0xcc);
    }
    else {
      float ratio=(float)AD1/(float)AD0;
      if (ratio<0.67) {
        lux=(unsigned int)((6.650*(float)AD0)-(9.653*(float)AD1));
      }
      else {
        if (ratio>0.9) {
          lux=0;
        } 
        else {
          lux = (unsigned int)((1.805*(float)AD0)-(1.977*(float)AD1));
        } 
      }
    } 
  }
  else {
    if (AD0<1000) {
      ALS_mode=0;
      I2C_write(0x39,0x00,0x0c);
      I2C_write(0x39,0x01,0x25);
      I2C_write(0x39,0x00,0xcc);
    }
    else {
      float ratio=(float)AD1/(float)AD0;
      if (ratio<=0.67) {
        lux=4*(unsigned int)((6.650*(float)AD0)-(9.653*(float)AD1));
      }
      else {
        if (ratio>0.9) {
          lux=0;
        } 
        else {
          lux = 4*(unsigned int)((1.805*(float)AD0)-(1.977*(float)AD1));
        } 
      }
    } 
  }


  //////Display Result
  //////Use Serial Monitor with baudrate 115200 can read the result, the result is also displayed by the Leds cross
  //////User may modify codes below
  Serial.print("Lux : "); 
  Serial.println(lux,DEC);


  if (lux>16000) {
    LED_On();
  } 
  else {
    if (lux>3000) {
      ledDriver.setLEDOff(3); 
      ledDriver.setLEDOn(4); 
      ledDriver.setLEDOn(5); 
      ledDriver.setLEDOff(6); 
      ledDriver.setLEDOn(7); 
      ledDriver.setLEDOn(8); 
      ledDriver.setLEDOn(9); 
      ledDriver.setLEDOn(10); 
      ledDriver.setLEDOff(11); 
      ledDriver.setLEDOff(12); 
      ledDriver.setLEDOn(13); 
      ledDriver.setLEDOn(14);  
    }
    else {
      if (lux>500) {
        ledDriver.setLEDOff(3); 
        ledDriver.setLEDOff(4); 
        ledDriver.setLEDOn(5); 
        ledDriver.setLEDOff(6); 
        ledDriver.setLEDOff(7); 
        ledDriver.setLEDOn(8); 
        ledDriver.setLEDOn(9); 
        ledDriver.setLEDOff(10); 
        ledDriver.setLEDOff(11); 
        ledDriver.setLEDOff(12); 
        ledDriver.setLEDOff(13); 
        ledDriver.setLEDOn(14);  
      } 
      else {
        LED_Off(); 
      }
    } 
  }

  delay(200);
}










































