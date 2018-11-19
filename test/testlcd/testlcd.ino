#include<Arduino.h>
#include<LiquidCrystal.h>

// 初始化针脚
const int rs = 9, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
    //设置LCD要显示的列数、行数，即2行16列
    lcd.begin(16, 2);

    //输出Hello World
    lcd.print("hello, world!");
}

void loop() {
    //设置光标定位到第0列，第1行（从0开始）
    lcd.setCursor(0, 1);
    //打印从重置后的秒数
    lcd.print( millis() / 1000);
}