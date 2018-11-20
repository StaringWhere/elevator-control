#include<Arduino.h>
#include<MsTimer2.h>

unsigned long time;

void setup(){
    Serial.begin(9600);
    MsTimer2::set(500,go);
    MsTimer2::start();
}

void loop(){
    delay(2000);
    Serial.println(millis());
}

void go(){
    Serial.println('1');
}