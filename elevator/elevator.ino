#include<Arduino.h>
#include<MsTimer2.h>
#include<LiquidCrystal.h>

#define NUM_OF_ORDER 15 //有13条指令，为方便处理，补全为15条
#define NUM_OF_FLOUR 5 //五层
#define ACCELERATION 8 //加速度为 8mm/(100ms)^2
#define MAX_VLCT 200 //最大速度为200mm/100ms
#define OPEN_PIN 2 //开门引脚
#define CLOSE_PIN 3 //关门引脚

// 初始化针脚
const int rs = 9, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int height=0; //电梯高度/mm
int drct=0; //运行方向，0为静止，1为向上，2为向下
int vlct=0; //电梯速度/(mm/100ms)
int i;
char order[NUM_OF_ORDER]={0}; //无序指令串
int target=0; //目标楼层/层,0代表无指令
int door=0; //电梯门的开合程度，0为关门，4为完全打开
int checkdoor=0; //检查电梯门是否打开，0为关闭，1为打开
int hold=0; //电梯是否无动作（包括开关门），0为有动作，1为无动作
unsigned long stoptime=0;
unsigned long starttime=0;

void setup() {
    Serial.begin(9600);
    pinMode(OPEN_PIN,INPUT);
    pinMode(CLOSE_PIN,INPUT);
    attachInterrupt(digitalPinToInterrupt(2),open,RISING); 
    attachInterrupt(digitalPinToInterrupt(3),close,RISING);
    lcd.begin(16, 2);
    while(Serial.available()<=0); //等待开始
    Serial.read(); //读掉's'
    show(); //显示
}

void loop(){
    if(checkdoor!=0){ //门未完全关闭执行的循环
        getorder();
        showorder();
        if(order[height/3000*3]==1||order[height/3000*3+drct]==1){ //无方向/顺电梯方向指令触发开门
            open();
        }
        delorder();  //尽量缩短getorder和delorder之间的距离，以增大遥控器信号有效概率
        showdoor(); //放在主函数里以免lcdcursor错位
        if(drct==0){ //静止方向的电梯从内部开门,根据第一个指令判断方向，先上后下
            catchthefirst();
            showdrct();
        };
        delay(100); //放在delorder后，使遥控器接收大几率落在delorder到getorder之间
        return;
    }
    getorder(); //更新指令串
    showorder(); //显示指令串
    search(); //搜索指令，赋值给target，无指令则target=0
    showtarget();
    if(target==0){
        drct=0;
        showdrct();
        //返回一层计时
        detachInterrupt(digitalPinToInterrupt(2)); //防止中断之后出现立刻触发返回一层
        detachInterrupt(digitalPinToInterrupt(3)); 
        if(hold==0){
            hold=1;
            starttime=millis();
            stoptime=starttime;
        }
        stoptime=millis();
        if(height!=0&&stoptime-starttime>=60000) //在一层不发出返回一层指令
            Serial.print('b'); //发送返回一层指令    
        attachInterrupt(digitalPinToInterrupt(2),open,RISING); 
        attachInterrupt(digitalPinToInterrupt(3),close,RISING);
        return;
    }
    hold=0; //退出无动作状态
    move(); //根据目标更新电梯状态(方向，高度，速度)
    if(height==(target-1)*3000){ //到达目标楼层
        inheritdrct(); //继承指令方向
        delorder(); //删除已完成指令
        target=0;
        open(); //开门
    }
    show(); //更新所有显示
    delay(100);
}

void getorder(){ //更新指令串
    Serial.print('o'); //发出获取指令串的指令
    for(i=0;i<NUM_OF_ORDER;i++){ //同步无序指令串
        while(Serial.available()<=0);
        order[i]=Serial.read();
    }
}

void delorder(){ //整楼层消除命令信号
    Serial.print(char(height/3000*3+drct)); //楼层数*3-1+运行方向
}

void search(){ //搜索指令，赋给target,无指令则target=0
    int sd=safedistance();
	if(drct==1||drct==0){
        i=(height+sd)/3000-((sd+height)%3000==0)+2; //计算安全停靠楼层
		if(searchup(i)==1)
			return;
		if(searchdown(NUM_OF_FLOUR)==1)
			return;
        if(searchup(1)==1)
            return;
	}
	else if(drct==2){
        i=(height-sd)/3000+1; //计算安全停靠楼层
		if(searchdown(i)==1)
            return;
        if(searchup(1)==1)
            return;
        if(searchdown(NUM_OF_FLOUR)==1)
            return;
	}
	target=0;
	return;
}

int searchup(int p){ //向上搜索指令，p为开始楼层数
	for(p=(p-1)*3;p<NUM_OF_ORDER;p+=3){
		if(order[p]==1||order[p+1]==1){
			target=p/3+1;
			return 1;
		}
	}
	return 0;
}

int searchdown(int p){ //向下搜索指令，p为开始楼层数
	for(p=(p-1)*3;p>=0;p-=3){
		if(order[p]==1||order[p+2]==1){
			target=p/3+1;
			return 1;
		}
	}
	return 0;
}

int safedistance(){ //计算安全距离
    return ((long)vlct*vlct/2/ACCELERATION); //long防止int溢出
}

void move(){ //根据指令更新电梯状态(方向，高度，速度)
    int distance=(target-1)*3000-height; //距离目标楼层的距离（有正负）
    int sd=safedistance(); //安全距离
    if(distance==0)
        return; //方向不变
    if(distance>0)
        drct=1;
    else
        drct=2;
    int distancetemp=abs(distance)-vlct; //匀速100ms后距离目标楼层的距离
    if(distancetemp>=sd){ //不需减速，判断是否能加速
        if(vlct<MAX_VLCT){
            int distancetemp2=abs(distance)-vlct-ACCELERATION/2; //保持100ms加速后距离目标楼层的距离（有正负）
            if(distancetemp2>=sd){
                height+=(3-2*drct)*(vlct+ACCELERATION/2); //(3-2*drct)是根据方向决定加减
                vlct+=ACCELERATION;
            }
            else{
                height+=(3-2*drct)*vlct;
            }
        }
        else
            height+=(3-2*drct)*vlct;
    }
    else if(distancetemp<safedistance()&&vlct>ACCELERATION){ //需要减速且没到最后100ms
            height+=(3-2*drct)*(vlct-ACCELERATION/2);
            vlct-=ACCELERATION;
    }
    else{ //需要减速且到减速的最后200ms
        delay(100);
        vlct=0;
        height=(target-1)*3000;
    }
}

void close(){
    if(vlct!=0)
        return;
    MsTimer2::set(800,closing);
    MsTimer2::start();
}
void closing(){
    if(door>0){
        door--;
    }
    if(door==0){
        checkdoor=0;
        MsTimer2::stop();
    }
}

void open(){
    if(vlct!=0)
        return;
    checkdoor=1;
    hold=0; //重置返回一层计时器
    MsTimer2::set(800,opening); //逐渐开门
    MsTimer2::start(); //开始计时
}

void opening(){
    if(door<4){
        door++;
    }
    if(door==4){
        MsTimer2::set(8000,close); //关门倒计时
        MsTimer2::start();
    }
}

void back(){
    order[0]=1;
}

void show(){
    showorder();
    showheight();
    showdrct();
    showvlct();
    showdoor();
    showtarget();
    led();
}
void led(){ //更新led显示
    Serial.print(char((height/3000)*3+drct+20)); //输出
}
void showorder(){ //更新指令串显示
    lcd.setCursor(0,0);
    for(i=0;i<NUM_OF_ORDER;i++)
        lcd.write(order[i]+48);
}
void showheight(){ //更新高度显示
    lcd.setCursor(2,1);
    i=5-countdigit(height)-(height==0);
    while(i--){ //补全位数
        lcd.write(' ');
    }
    lcd.print(height);
}
void showdrct(){ //更新方向显示
    lcd.setCursor(8,1);
    lcd.print(drct);
}
void showvlct(){ //更新速度显示
    lcd.setCursor(10,1);
    i=3-countdigit(vlct)-(vlct==0);
    while(i--){
        lcd.write(' ');
    }
    lcd.print(vlct);
}
void showdoor(){ //更新门的状态显示
    lcd.setCursor(14,1);
    lcd.print(door);
}
void showtarget(){ //更新目标楼层显示
    lcd.setCursor(0,1);
    lcd.print(target);
}

int countdigit(int p){ //求整数位数
    int count=0;
    while(p>0){
        p/=10;
        count++;
    }
    return count;
}

void inheritdrct(){ //继承指令方向，若指令无方向，则继承之前电梯方向
    int i=height/3000;
    if(drct<2){
        if(order[i*3+1]==1){
            drct=1;
            return;
        }
        if(order[i*3+2]==1){
            drct=2;
            return;
        }
    }
    else{
        if(order[i*3+2]==1){
            return;
        }
        if(order[i*3+1]==1){
            drct=1;
            return;
        }
    }
}

void catchthefirst(){ //根据第一个命令改变方向
    int i;
    for(i=NUM_OF_ORDER-1;i>=0;i--){ //从高向低搜索
        if(order[i]==1)
            break;
    }
    if(i>=0){
        if(i/3==height/3000){
            if(order[i/3*3+1]==1)
                drct=1;
            else if(order[i/3*3+2]==1)
                drct=2;
        }
        else if(i/3>height/3000)
            drct=1;
        else
            drct=2;
    }
}