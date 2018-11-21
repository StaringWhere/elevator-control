#include<Arduino.h>
#include<MsTimer2.h>
//外部中断内部的delay和millis无效
//外部中断期间，外部millis静止
//外部中断期间，外部delay静止
//定时器中断内部的delay和millis无效
//定时器中断期间，外部的delay静止
//定时器的计时从每次定时函数执行完开始
//定时中断函数中插入外部中断，函数结束后执行

unsigned long time;

void setup(){
    Serial.begin(9600);
    MsTimer2::set(1000,go);
    MsTimer2::start();
    attachInterrupt(0,hi,RISING);
}

void loop(){
    delay(5000);
    Serial.println('a');
}

void go(){
    Serial.println('1');
}

void hi(){
    int i;
    for(i=0;i<1000;i++){
        Serial.print('w');
    }
    Serial.println('2');
}