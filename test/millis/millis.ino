#include<Arduino.h>
#include<MsTimer2.h>
//外部中断内部的delay无效，millis静止
//外部中断期间，外部millis静止
//外部中断期间，外部delay静止
//定时器中断内部的delay无效，millis静止
//定时器中断期间，外部的delay静止
//定时器的计时从每次定时函数执行完开始
//定时中断函数中插入外部中断，函数结束后执行
//定时器中断函数中插入定时器中断，函数结束后执行

unsigned long time;
int i=1;
void setup(){
    Serial.begin(9600);
    MsTimer2::set(1000,go);
    //MsTimer2::start();
    attachInterrupt(digitalPinToInterrupt(2),hi,RISING);
    Serial.println(i/3*3);
}

void loop(){
}

void go(){
    Serial.println('1');
    MsTimer2::set(100,hi);
    MsTimer2::start();
    int i=1000;
    while(i--)
        Serial.println('3');
}

void hi(){
    // int i;
    // for(i=0;i<1000;i++){
    //     Serial.print('w');
    // }
    Serial.println('2');
}