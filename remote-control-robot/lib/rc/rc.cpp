#include <Arduino.h>
#include "pinout.h"



void read_lift_y(){
    Serial.print(analogRead(Left_y));
    Serial.println();
}

void read_lift_x(){
    Serial.print(analogRead(Left_x));
    Serial.println();
}

void read_right_x(){
    Serial.print(analogRead(Right_x));
    Serial.println();
}

void read_right_y(){
    Serial.print(analogRead(Right_y));
    Serial.println();
}