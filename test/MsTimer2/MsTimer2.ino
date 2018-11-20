#include<Arduino.h>
#include<MsTimer2.h>

//start可以重新激活计时器
//set会重置计时器
//计时器只能存在一个

void setup(){
    MsTimer2::set(5000,onTime1);
    Serial.begin(9600);
    MsTimer2::start();
    delay(2000);
    MsTimer2::set(5000,onTime2);
}

void loop(){
}

void onTime1(){
    Serial.println('1');
}
void onTime2(){
    Serial.println('2');
}