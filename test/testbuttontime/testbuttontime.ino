#include <Arduino.h>
//按按钮的时长，正常在80-200ms，但也可能按到10ms
unsigned long starttime;
unsigned long stoptime;
unsigned long looptime;

void setup(){
pinMode(2,INPUT);
Serial.begin(9600);
}

void loop(){
    while(digitalRead(2)==LOW);
    starttime = millis();
    while(digitalRead(2)==HIGH);
    stoptime = millis();
    looptime = stoptime - starttime;
    Serial.println(looptime);
}