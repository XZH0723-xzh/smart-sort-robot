#include <Arduino.h>

int sensor[4]= {A0,A2,A4,A3};

boolean SensorTrigger( int which );

void setup()

{

Serial.begin(9600);

for(int i=0;i<4;i++)

pinMode(sensor[i],INPUT);

}



void loop()

{

if(SensorTrigger(0)){

    Serial.println('#');

}

delay(100);

}



boolean SensorTrigger( int which )

{

  boolean where = false;

  if( !digitalRead( sensor[ which ] ) )

  {

    delay( 100 );

    if( !digitalRead( sensor[ which ] ) ) where = true;

  }   

  return( where );

}