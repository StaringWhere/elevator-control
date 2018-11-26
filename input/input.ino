#include <Arduino.h>
#include <IRremote.h> //IRremote库声明

/*
输入板编码：
flag下标-指令-引脚
0-一楼-红外1
1-一楼上-2
2-一楼下-空
3-二楼-红外2
4-二楼上-3
5-二楼下-4
6-三楼-红外3
7-三楼上-5
8-三楼下-6
9-四楼-红外4
10-四楼上-7
11-四楼下-8
12-五楼-红外5
13-五楼上-空
14-五楼下-9
无-开门-红外+
无-关门-红外-
无-开始-13
*/


#define NUM_OF_ORDER 15 //除去开关门外，有13条指令，为方便处理，补全为15条
#define NUM_OF_FLOUR 5 //五层
#define REMOTE_PIN 10 //定义红外接收器的引脚
#define OPEN_PIN 11 //开门
#define CLOSE_PIN 12 //关门
#define START_PIN 13 //开始键，重置键
#define BUZZER_PIN A5 //警报

char order[NUM_OF_ORDER]={'\0'}; //无序指令串
int i;
char t;
int drct=0; //运行方向，0为静止，1为向上，2为向下

IRrecv irrecv(REMOTE_PIN); 
decode_results results;

void setup(){
    for(i=2;i<11;i++)
        pinMode(i,INPUT);
	for(i=11;i<13;i++)
		pinMode(i,OUTPUT);
	pinMode(START_PIN,INPUT);
	pinMode(A5,OUTPUT);
    Serial.begin(9600);
    irrecv.enableIRIn(); //启动接收器
	while(digitalRead(START_PIN)==LOW); //等待开始
	Serial.print('s'); //向elevator发送开始信号
}

void loop(){
	check();
	if(Serial.available()>0){
		t=Serial.read();
		if(t=='o'){ //发送无序指令串
			sendorder();
		}
		else if(t=='b'){
			order[0]=1;
		}
		else if(t>=0&&t<NUM_OF_ORDER){ //删除对应指令
			delorder(t);
		}
	}
}

void check(){ //检测输入
	//检测按钮输入
	if(digitalRead(2)==1){ //一楼上
		order[1]=1;
	}
	if(digitalRead(3)==1){ //二楼上
		order[4]=1;
	}
	if(digitalRead(4)==1){ //二楼下
		order[5]=1;
	}
	if(digitalRead(5)==1){ //三楼上
		order[7]=1;
	}
	if(digitalRead(6)==1){ //三楼下
		order[8]=1;
	}
	if(digitalRead(7)==1){ //四楼上
		order[10]=1;
	}
	if(digitalRead(8)==1){ //四楼下
		order[11]=1;
	}
	if(digitalRead(9)==1){ //五楼下
		order[14]=1;
	}
	//接收遥控器信号
    if(irrecv.decode(&results)){ 
		// Serial.println(results.value, HEX); //以16进制换行输出接收代码
		switch(results.value){
		case 0xFF30CF: //一楼
			order[0]=1;
			break;
		case 0xFF18E7: //二楼
			order[3]=1;
			break;
		case 0xFF7A85: //三楼
			order[6]=1;
			break;
		case 0xFF10EF: //四楼
			order[9]=1;
			break;
		case 0xFF38C7: //五楼
			order[12]=1;
			break;
		case 0xFFA857: //开门
			digitalWrite(OPEN_PIN,HIGH);
			digitalWrite(OPEN_PIN,LOW);
			break;
		case 0xFFE01F: //关门
			digitalWrite(CLOSE_PIN,HIGH);
			digitalWrite(CLOSE_PIN,LOW);
			break;
		case 0xFF02FD: //警报开
			digitalWrite(A5,HIGH);
			break;
		case 0xFF22DD: //警报关
			digitalWrite(A5,LOW);
			break;
		}
		irrecv.resume(); //接收下一个值
    }
}

void sendorder(){ //发送指令
	for(i=0;i<NUM_OF_ORDER;i++){
		Serial.print(order[i]);
	}
}

void delorder(int p){ //删除对应指令
	order[p]=0;
	order[p/3*3]=0;
}