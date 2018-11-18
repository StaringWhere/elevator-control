#include <Arduino.h>
#include <IRremote.h> //IRremote库声明

/*
输入板编码：
flag下标-指令-引脚
0-一楼-红外
1-一楼上-2
2-一楼下-空
3-二楼-红外
4-二楼上-3
5-二楼下-4
6-三楼-红外
7-三楼上-5
8-三楼下-6
9-四楼-红外
10-四楼上-7
11-四楼下-8
12-五楼-红外
13-五楼上-空
14-五楼下-9
无-开门-红外
无-关门-红外
*/


#define NUM_OF_ORDER 15 //除去开关门外，有13条指令，为方便处理，补全为15条
#define NUM_OF_FLOUR 5 //五层
#define REMOTE_PIN 10 //定义红外接收器的引脚
#define OPEN_PIN 11 //开门
#define CLOSE_PIN 12 //关门

char order[NUM_OF_ORDER]={'\0'}; //无序指令串
int i;
char t;
int count=0; //指令数
int drct=0; //运行方向，0为静止，1为向上，2为向下

IRrecv irrecv(REMOTE_PIN); 
decode_results results;

void setup(){
    for(i=2;i<11;i++)
        pinMode(i,INPUT);
	for(i=11;i<13;i++)
		pinMode(i,OUTPUT);
    Serial.begin(9600);
    irrecv.enableIRIn(); //启动接收器
}

void loop() {
	check();
	if(Serial.available()>0){
		t=Serial.peek();
		if(t=='o'){ //发送无序指令串
			Serial.read();
			sendorder();
		}			
		else if(t>=0&&t<NUM_OF_ORDER){ //删除对应指令
			Serial.read();
			delorder(t);
		}
	}
}

void check(){ //检测输入
	//检测按钮输入
	if(digitalRead(2)==1&&order[1]==0){ //一楼上
		order[1]=1;
		count++;
	}
	if(digitalRead(3)==1&&order[4]==0){ //二楼上
		order[4]=1;
		count++;
	}
	if(digitalRead(4)==1&&order[5]==0){ //二楼下
		order[5]=1;
		count++;
	}
	if(digitalRead(5)==1&&order[7]==0){ //三楼上
		order[7]=1;
		count++;
	}
	if(digitalRead(6)==1&&order[8]==0){ //三楼下
		order[8]=1;
		count++;
	}
	if(digitalRead(7)==1&&order[10]==0){ //四楼上
		order[10]=1;
		count++;
	}
	if(digitalRead(8)==1&&order[11]==0){ //四楼下
		order[11]=1;
		count++;
	}
	if(digitalRead(9)==1&&order[14]==0){ //五楼下
		order[14]=1;
		count++;
	}
	//接收遥控器信号
    if(irrecv.decode(&results)){ 
		// Serial.println(results.value, HEX); //以16进制换行输出接收代码
		switch(results.value){
		case 0xFF30CF: //一楼
			if(order[0]==0){
				order[0]=1;
				count++;
			}
			break;
		case 0xFF18E7: //二楼
			if(order[3]==0){
				order[3]=1;
				count++;
			}
			break;
		case 0xFF7A85: //三楼
			if(order[6]==0){
				order[6]=1;
				count++;
			}
			break;
		case 0xFF10EF: //四楼
			if(order[9]==0){
				order[9]=1;
				count++;
			}
			break;
		case 0xFF38C7: //五楼
			if(order[12]==0){
				order[12]=1;
				count++;
			}
			break;
		case 0xFFA857: //开门
			digitalWrite(OPEN_PIN,HIGH);
			digitalWrite(OPEN_PIN,LOW);
			break;
		case 0xFFE01F: //关门
			digitalWrite(CLOSE_PIN,HIGH);
			digitalWrite(CLOSE_PIN,LOW);
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