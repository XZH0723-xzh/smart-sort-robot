#include <Arduino.h>

int sensor[4]= {A0,A2,A4,A3};

void nrf24l01_Init(){
    Serial.begin(9600);
    for(int i=0;i<4;i++)
    {
        pinMode(sensor[i],INPUT);
    }
} 
