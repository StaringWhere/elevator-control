#include<Arduino.h>

#define ANA_PIN A0

int i;

void setup(){
    Serial.begin(9600);
    pinMode(ANA_PIN,INPUT);
}

void loop(){
    i=analogRead(ANA_PIN);
    Serial.println(i);
    delay(1000);
}