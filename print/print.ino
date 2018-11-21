#include<Arduino.h>

int i;
int rec=0;
int flour=1;
int drct=0;
int H[8]={6,11,A3,9,5,A2,4,A0};
int L[8]={10,3,2,7,A1,8,12,13};
int uparrow[8][8]={
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,1,0,
    0,0,0,0,0,1,1,1,
    0,0,0,0,0,0,1,0,
    0,0,0,0,0,0,1,0,
    0,0,0,0,0,0,1,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
};
int downarrow[8][8]={
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,1,0,
    0,0,0,0,0,0,1,0,
    0,0,0,0,0,0,1,0,
    0,0,0,0,0,1,1,1,
    0,0,0,0,0,0,1,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
};
int empty[8][8]={
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
};
int one[8][8]={
    0,0,0,0,0,0,0,0,
    0,0,1,0,0,0,0,0,
    0,0,1,0,0,0,0,0,
    0,0,1,0,0,0,0,0,
    0,0,1,0,0,0,0,0,
    0,0,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
};
int two[8][8]={
    0,0,0,0,0,0,0,0,
    1,1,1,0,0,0,0,0,
    0,0,1,0,0,0,0,0,
    1,1,1,0,0,0,0,0,
    1,0,0,0,0,0,0,0,
    1,1,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
};
int three[8][8]={
    0,0,0,0,0,0,0,0,
    1,1,1,0,0,0,0,0,
    0,0,1,0,0,0,0,0,
    1,1,1,0,0,0,0,0,
    0,0,1,0,0,0,0,0,
    1,1,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
};
int four[8][8]={
    0,0,0,0,0,0,0,0,
    1,0,1,0,0,0,0,0,
    1,0,1,0,0,0,0,0,
    1,1,1,0,0,0,0,0,
    0,0,1,0,0,0,0,0,
    0,0,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
};
int five[8][8]={
    0,0,0,0,0,0,0,0,
    1,1,1,0,0,0,0,0,
    1,0,0,0,0,0,0,0,
    1,1,1,0,0,0,0,0,
    0,0,1,0,0,0,0,0,
    1,1,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
};
int six[8][8]={
    0,0,0,0,0,0,0,0,
    1,1,1,0,0,0,0,0,
    1,0,0,0,0,0,0,0,
    1,1,1,0,0,0,0,0,
    1,0,1,0,0,0,0,0,
    1,1,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
};
int shengming[8][8]={
    0,0,0,0,0,0,0,0,
    1,1,1,0,0,1,1,1,
    1,0,1,1,1,1,0,1,
    1,1,1,0,0,1,1,1,
    0,0,0,0,0,0,0,0,
    0,0,1,0,0,1,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
};

void setup() {
    Serial.begin(9600); 
    for(i=2;i<=13;i++){
        pinMode(i,OUTPUT);
    }
    for(i=A1;i<=A4;i++){
        pinMode(i,OUTPUT);
    }
    for(i=0;i<8;i++)
        digitalWrite(H[i],HIGH);
}

void loop(){
    i=Serial.read();
    if(i>=20&&i<35){
        rec=i-20;
        flour=rec/3+1;
        drct=rec%3;
    }
    else
    switch(flour){
        case 1:
            draw(one);
            break;
        case 2:
            draw(two);
            break;
        case 3:
            draw(three);
            break;
        case 4:
            draw(four);
            break;
        case 5:
            draw(five);
            break;
        default:;
    }
    switch(drct){
        case 1:
            draw(uparrow);
            break;
        case 2:
            draw(downarrow);
            break;
        default:;
    }
}

void light(int i,int j){//点亮单个led灯，ij分别为横纵坐标
    digitalWrite(H[i],LOW);
    digitalWrite(L[j],HIGH);
    delay(1);
    digitalWrite(H[i],HIGH);
    digitalWrite(L[j],LOW);
}

void draw(int p[][8]){//点亮一个图案
    int i,j;
    for(i=0;i<8;i++){
        for(j=0;j<8;j++){
            if(p[i][j]==1){
                light(i,j);
            }
        }    
    }    
}
