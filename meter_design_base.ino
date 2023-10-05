#include <random>
std::random_device rd;   // non-deterministic generator
std::mt19937 gen(rd());  // to seed mersenne twister.
std::uniform_int_distribution<> dist(0, 18);
uint8_t speed = 0;
uint8_t pre_speed = 0;

#include <Ticker.h>
#include <EEPROM.h>

#include "ILI9327.h"
#include "meter_base9.h"
#include "arma_logo_opening.h"
#include <SPI.h>

// #include <Fonts\FreeSansBold24pt7b.h>
// #include <Fonts\DSEG7_Modern_Bold_24.h>
#include "googleFont48pt.h"
#include <Fonts\FreeSans9pt7b.h>

#define DISPLAY_CS 5
#define DISPLAY_DC 16
#define DISPLAY_MOSI 23
#define DISPLAY_MISO 19
#define DISPLAY_SCK 18
#define DISPLAY_RST 17

const uint8_t display_bias_x = 365;
const uint8_t display_bias_w = 35;

uint16_t x_0 = 0;
uint16_t y_0 = 0;
const uint16_t b_x = 182;
const uint16_t b_y = 125;
const uint16_t R  = 96;
const uint16_t r  = 5;

uint16_t x_1;
uint16_t y_1;
uint16_t x_2;
uint16_t y_2;
uint16_t x_1_old=0;
uint16_t y_1_old=0;
uint16_t x_2_old=0;
uint16_t y_2_old=0;

ILI9327 display(DISPLAY_CS, DISPLAY_DC, &SPI, DISPLAY_RST);

const uint16_t winker_left_coor[6][2] = {
  {15, 35},
  {60, 15},
  {60, 55},

  {40, 35},
  {85, 15},
  {85, 55},
};
const uint16_t winker_right_coor[6][2] = {
  {350, 35},
  {305, 15},
  {305, 55},

  {325, 35},
  {280, 15},
  {280, 55},
};


const uint16_t gauge1_list[19][2] = {
  {R * cos( 90*PI/180) + b_x, R * sin( 90*PI/180) + b_y},
  {R * cos(100*PI/180) + b_x, R * sin(100*PI/180) + b_y},
  {R * cos(110*PI/180) + b_x, R * sin(110*PI/180) + b_y},
  {R * cos(120*PI/180) + b_x, R * sin(120*PI/180) + b_y},
  {R * cos(130*PI/180) + b_x, R * sin(130*PI/180) + b_y},
  {R * cos(140*PI/180) + b_x, R * sin(140*PI/180) + b_y},
  {R * cos(150*PI/180) + b_x, R * sin(150*PI/180) + b_y},
  {R * cos(160*PI/180) + b_x, R * sin(160*PI/180) + b_y},
  {R * cos(170*PI/180) + b_x, R * sin(170*PI/180) + b_y},
  {R * cos(180*PI/180) + b_x, R * sin(180*PI/180) + b_y},
  {R * cos(190*PI/180) + b_x, R * sin(190*PI/180) + b_y},
  {R * cos(200*PI/180) + b_x, R * sin(200*PI/180) + b_y},
  {R * cos(210*PI/180) + b_x, R * sin(210*PI/180) + b_y},
  {R * cos(220*PI/180) + b_x, R * sin(220*PI/180) + b_y},
  {R * cos(230*PI/180) + b_x, R * sin(230*PI/180) + b_y},
  {R * cos(240*PI/180) + b_x, R * sin(240*PI/180) + b_y},
  {R * cos(250*PI/180) + b_x, R * sin(250*PI/180) + b_y},
  {R * cos(260*PI/180) + b_x, R * sin(260*PI/180) + b_y},
  {R * cos(270*PI/180) + b_x, R * sin(270*PI/180) + b_y},
};

const uint16_t gauge2_list[19][2] = {
  {R * cos(450*PI/180) + b_x, R * sin(450*PI/180) + b_y},
  {R * cos(440*PI/180) + b_x, R * sin(440*PI/180) + b_y},
  {R * cos(430*PI/180) + b_x, R * sin(430*PI/180) + b_y},
  {R * cos(420*PI/180) + b_x, R * sin(420*PI/180) + b_y},
  {R * cos(410*PI/180) + b_x, R * sin(410*PI/180) + b_y},
  {R * cos(400*PI/180) + b_x, R * sin(400*PI/180) + b_y},
  {R * cos(390*PI/180) + b_x, R * sin(390*PI/180) + b_y},
  {R * cos(380*PI/180) + b_x, R * sin(380*PI/180) + b_y},
  {R * cos(370*PI/180) + b_x, R * sin(370*PI/180) + b_y},
  {R * cos(360*PI/180) + b_x, R * sin(360*PI/180) + b_y},
  {R * cos(350*PI/180) + b_x, R * sin(350*PI/180) + b_y},
  {R * cos(340*PI/180) + b_x, R * sin(340*PI/180) + b_y},
  {R * cos(330*PI/180) + b_x, R * sin(330*PI/180) + b_y},
  {R * cos(320*PI/180) + b_x, R * sin(320*PI/180) + b_y},
  {R * cos(310*PI/180) + b_x, R * sin(310*PI/180) + b_y},
  {R * cos(300*PI/180) + b_x, R * sin(300*PI/180) + b_y},
  {R * cos(290*PI/180) + b_x, R * sin(290*PI/180) + b_y},
  {R * cos(280*PI/180) + b_x, R * sin(280*PI/180) + b_y},
  {R * cos(270*PI/180) + b_x, R * sin(270*PI/180) + b_y},
};

const uint16_t color_table[] = {
    ILI9327_BLACK,
    ILI9327_NAVY,
    ILI9327_DARKGREEN,
    ILI9327_DARKCYAN,
    ILI9327_MAROON,
    ILI9327_PURPLE,
    ILI9327_OLIVE,
    ILI9327_LIGHTGREY,
    ILI9327_DARKGREY,
    ILI9327_BLUE,
    ILI9327_GREEN,
    ILI9327_CYAN,
    ILI9327_RED,
    ILI9327_MAGENTA,
    ILI9327_YELLOW,
    ILI9327_WHITE,
    ILI9327_ORANGE,
    ILI9327_GREENYELLOW,
    ILI9327_PINK,
    ILI9327_BACK_COLOR,
    };




Ticker winker_left;
Ticker winker_right;
int winker_count_left = 0;
int winker_count_right = 0;

Ticker battery;
int remained_battery = 4;
bool charge = false;

Ticker bluetooth;
int buletooth_on = false;

Ticker light;
int light_on = false;

Ticker horn;
int horn_on = false;

Ticker odo;
int odo_val = 0;
int odo_old_val = odo_val;

void setup()
{
  Serial.begin(115200);
  // Serial.println("Hello!");
  delay(500);
  display.begin(16000000);
  display.drawRGBBitmap(0, 0, logo_img, logo_imgWidth, logo_imgHeight);

  delay(2000);
  // display.begin(16000000);
  display.drawRGBBitmap(0, 0, img, imgWidth, imgHeight);



  // winker点滅割り込み
  winker_left.attach(0.5, []() {
    display.fillTriangle(
                          winker_left_coor[0][0], winker_left_coor[0][1], 
                          winker_left_coor[1][0], winker_left_coor[1][1], 
                          winker_left_coor[2][0], winker_left_coor[2][1], 
                          ILI9327_BLACK);
    display.fillTriangle(
                          winker_left_coor[3][0], winker_left_coor[3][1], 
                          winker_left_coor[4][0], winker_left_coor[4][1], 
                          winker_left_coor[5][0], winker_left_coor[5][1], 
                          ILI9327_BLACK);

    if(winker_count_left%2==0){
    display.fillTriangle(
                          winker_left_coor[0][0], winker_left_coor[0][1], 
                          winker_left_coor[1][0], winker_left_coor[1][1], 
                          winker_left_coor[2][0], winker_left_coor[2][1], 
                          ILI9327_YELLOW);
    display.fillTriangle(
                          winker_left_coor[3][0], winker_left_coor[3][1], 
                          winker_left_coor[4][0], winker_left_coor[4][1], 
                          winker_left_coor[5][0], winker_left_coor[5][1], 
                          ILI9327_YELLOW);
    display.drawTriangle(
                          winker_left_coor[3][0], winker_left_coor[3][1], 
                          winker_left_coor[4][0], winker_left_coor[4][1], 
                          winker_left_coor[5][0], winker_left_coor[5][1], 
                          ILI9327_BLACK);
    }

    winker_count_left++;
   });
  // winker点滅割り込み
  winker_right.attach(0.5, []() {
    display.fillTriangle(
                          winker_right_coor[0][0], winker_right_coor[0][1], 
                          winker_right_coor[1][0], winker_right_coor[1][1], 
                          winker_right_coor[2][0], winker_right_coor[2][1], 
                          ILI9327_BLACK);
    display.fillTriangle(
                          winker_right_coor[3][0], winker_right_coor[3][1], 
                          winker_right_coor[4][0], winker_right_coor[4][1], 
                          winker_right_coor[5][0], winker_right_coor[5][1], 
                          ILI9327_BLACK);

    if(winker_count_right%2==0){
    display.fillTriangle(
                          winker_right_coor[0][0], winker_right_coor[0][1], 
                          winker_right_coor[1][0], winker_right_coor[1][1], 
                          winker_right_coor[2][0], winker_right_coor[2][1], 
                          ILI9327_YELLOW);
    display.fillTriangle(
                          winker_right_coor[3][0], winker_right_coor[3][1], 
                          winker_right_coor[4][0], winker_right_coor[4][1], 
                          winker_right_coor[5][0], winker_right_coor[5][1], 
                          ILI9327_YELLOW);
    display.drawTriangle(
                          winker_right_coor[3][0], winker_right_coor[3][1], 
                          winker_right_coor[4][0], winker_right_coor[4][1], 
                          winker_right_coor[5][0], winker_right_coor[5][1], 
                          ILI9327_BLACK);
    }
    winker_count_right++;
   });

    // バッテリ残量
    battery.attach(1.0, [](){
    int c_x = 20;
    int c_y = 170;
    int H = 19;
    int dH = 2.5;
    int W = 35;
    if(remained_battery > 0)
      display.fillRect(c_x, c_y-H*remained_battery-dH*(remained_battery-1), W, H, ILI9327_BLACK);

    if(charge){
      // display.fillRect(20+10*remained_battery, 210, 9, H, ILI9327_GREEN);
      if(remained_battery > 0)
        display.fillRect(c_x, c_y-H*remained_battery-dH*(remained_battery-1), W, H, ILI9327_GREEN);

      remained_battery++;
    }
    else{
      remained_battery--;
    }

   if(remained_battery>4){
    charge = false;
    remained_battery = 4;
   }
  else if(remained_battery<0){
    charge = true;
    remained_battery = 0;
  }
  // Serial.println(remained_battery);
  });


  // Bluetooth
  bluetooth.attach(1.0, [](){
    if(buletooth_on){
      display.fillCircle(310, 151, 2, ILI9327_CYAN);
      display.fillCircle(328, 151, 2, ILI9327_CYAN);
    }
    else{
      display.fillCircle(310, 151, 2, ILI9327_BLACK);
      display.fillCircle(328, 151, 2, ILI9327_BLACK);
    }
    buletooth_on = !buletooth_on;
  });


  // Light
  light.attach(1.0, [](){
    if(light_on){
      display.fillRect(300, 74, 2, 10, ILI9327_YELLOW);
      display.fillRect(300, 86, 2, 10, ILI9327_YELLOW);
      display.fillRect(300, 98, 2, 10, ILI9327_YELLOW);
      display.fillCircle(325, 90, 7, ILI9327_YELLOW);
    }
    else{
      display.fillRect(300, 74, 2, 10, ILI9327_BLACK);
      display.fillRect(300, 86, 2, 10, ILI9327_BLACK);
      display.fillRect(300, 98, 2, 10, ILI9327_BLACK);
      display.fillCircle(325, 90, 7, ILI9327_BLACK);
    }
    light_on = !light_on;
  });

  // ODO
  // odo.attach(60.0, [](){
  //   display.setFont(&FreeSans9pt7b);
  //   display.setTextSize(1);
  //   display.setTextColor(ILI9327_BLACK, ILI9327_BLACK);
  //   display.setCursor(270,225);
  //   display.println(odo_val);  // クリア
  //   // display.setTextSize(3);
  //   odo_val++;
  //   display.setTextColor(ILI9327_CYAN, ILI9327_CYAN);               // 描画
  //   display.setCursor(270,225);
  //   display.println(odo_val);  // クリア
  // });

  delay(10);
}

int min_range = 130;
int max_range = 405;
int max_mph = 18;

int i = min_range;
int oi = i;

int gauge1 = 90;
int gauge2 = 450;
bool gauge_flag=true;

int aa, bb;
bool flag=true;

void loop()
{
  speed = dist(gen);
  // 文字をいい感じ配置するためのバイアス
  // if(max_mph*(i-min_range)/(max_range - min_range) != max_mph*(oi-min_range)/(max_range - min_range)){
  //   if(max_mph*(oi-min_range)/(max_range - min_range) < 10){
  //     aa = 50;
  //     }
  //   else{
  //     aa = 0; 
  //     }
  //   if(max_mph*(i-min_range)/(max_range - min_range) < 10){
  //     bb = 50;
  //     }
  //   else{
  //     bb = 0; 
  //     }

  //   //デジタル表示 

    // 文字消去処理
    display.setTextColor(ILI9327_BLACK, ILI9327_BLACK);
    display.setCursor(160+aa-display_bias_w,150);
    display.setFont(&NotoSansJP_Bold48pt7b);
    display.setTextSize(1);
    // display.println(max_mph*(oi-min_range)/(max_range - min_range));  // クリア
    display.println(pre_speed);  // クリア
    // 文字書き込み処理
    display.setTextColor(ILI9327_CYAN, ILI9327_CYAN);               // 描画
    display.setCursor(160+bb-display_bias_w,150);
    display.setFont(&NotoSansJP_Bold48pt7b);
    display.setTextSize(1);
    // display.println(max_mph*(i-min_range)/(max_range - min_range));
    display.println(speed);
  // }
  // oi = i;


  // if(!flag){
  //   // display.drawLine(x_1, y_1, x_2_old, y_2_old, ILI9327_BLACK);
  //   display.fillCircle(x_1_old, y_1_old, r, ILI9327_BLACK);
  //   display.fillCircle(x_2_old, y_2_old, r, ILI9327_BLACK);
  // }

  // x_1 = gauge1_list[max_mph*(i-min_range)/(max_range - min_range)][0];//(uint16_t)(R * cos(gauge1*PI/180) + b_x);
  // y_1 = gauge1_list[max_mph*(i-min_range)/(max_range - min_range)][1];//(uint16_t)(R * sin(gauge1*PI/180) + b_y);
  // x_2 = gauge2_list[max_mph*(i-min_range)/(max_range - min_range)][0];//(uint16_t)(R * cos(gauge2*PI/180) + b_x);
  // y_2 = gauge2_list[max_mph*(i-min_range)/(max_range - min_range)][1];//(uint16_t)(R * sin(gauge2*PI/180) + b_y);
 
  x_1 = gauge1_list[speed][0];//(uint16_t)(R * cos(gauge1*PI/180) + b_x);
  y_1 = gauge1_list[speed][1];//(uint16_t)(R * sin(gauge1*PI/180) + b_y);
  x_2 = gauge2_list[speed][0];//(uint16_t)(R * cos(gauge2*PI/180) + b_x);
  y_2 = gauge2_list[speed][1];//(uint16_t)(R * sin(gauge2*PI/180) + b_y);
  x_1_old = x_1;
  y_1_old = y_1;
  x_2_old = x_2;
  y_2_old = y_2;

  // for(int index=18; index > max_mph*(i-min_range)/(max_range - min_range)-1; index--){
  for(int index=18; index > speed-1; index--){
    
    display.fillCircle(
                        gauge1_list[index][0], 
                        gauge1_list[index][1], 
                        r, ILI9327_BLACK);
    display.fillCircle(
                        gauge2_list[index][0], 
                        gauge2_list[index][1], 
                        r, ILI9327_BLACK);
  }

  // for(int index=0; index < max_mph*(i-min_range)/(max_range - min_range); index++){
  for(int index=0; index < speed; index++){

    display.fillCircle(
                        gauge1_list[index][0], 
                        gauge1_list[index][1], 
                        r, ILI9327_CYAN);
    display.fillCircle(
                        gauge2_list[index][0], 
                        gauge2_list[index][1], 
                        r, ILI9327_CYAN);
  }


  delay(500);
  pre_speed = speed;


  if(flag){
    i++;
  }
  else{
    i--;
  }
  if (i>max_range)
    flag = false;
  else if (i<min_range)
    flag = true;


  display.setFont(&FreeSans9pt7b);
  display.setTextSize(1);
  display.setTextColor(ILI9327_BLACK, ILI9327_BLACK);
  display.setCursor(270,225);
  display.println(odo_val);  // クリア
  odo_val++;
  display.setTextColor(ILI9327_CYAN, ILI9327_CYAN);               // 描画
  display.setCursor(270,225);
  display.println(odo_val);  // クリア

}