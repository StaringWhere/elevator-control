#include <Arduino.h>
#include <IRremote.h>

//持续按住红外遥控按钮，只会发送一次对应code，接下来会发送‘FFFFFFFF’

#define REMOTE_PIN 10

int i;
IRrecv irrecv(REMOTE_PIN); 
decode_results results;

void setup(){
    pinMode(REMOTE_PIN,INPUT);
    pinMode(A5,OUTPUT);
    Serial.begin(9600);
    irrecv.enableIRIn(); //启动接收器
    Serial.println('1');
}

void loop(){
    if(irrecv.decode(&results)){ 
		// Serial.println(results.value, HEX); //以16进制换行输出接收代码
        Serial.println(results.value,HEX);
		switch(results.value){
		case 0xFF02FD: 
            digitalWrite(A5,HIGH);
			break;
		case 0xFF22DD: 
            digitalWrite(A5,LOW);
			break;
		}
		irrecv.resume(); //接收下一个值
    }
}