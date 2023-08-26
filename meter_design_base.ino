#include <Ticker.h>
#include <EEPROM.h>

#include "ILI9327.h"
#include "meter_base8.h"
#include <SPI.h>

// #include <Fonts\FreeSansBold24pt7b.h>
// #include <Fonts\DSEG7_Modern_Bold_24.h>
#include <Fonts\custom_windows_arial_24.h>
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
uint16_t b_x = 325-display_bias_w;
uint16_t b_y = 120;
uint16_t r  = 40;

uint16_t x_1 = x_0 + b_x;
uint16_t y_1 = y_0 + b_y;
uint16_t x_2;
uint16_t y_2;
uint16_t x_2_old=x_1;
uint16_t y_2_old=y_1;

ILI9327 display(DISPLAY_CS, DISPLAY_DC, &SPI, DISPLAY_RST);

uint16_t winker_coor[6][3] = {
  {76, 23, 1},
  {52, 23, 5},
  {20, 23, 10},
  {323-display_bias_w, 23, 1},
  {347-display_bias_w, 23, 5},
  {379-display_bias_w, 23, 10}
};

uint16_t color_table[] = {
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
    ILI9327_PINK};

uint16_t bt_table[6][2] = {
  {21, 202},
  {38, 219},
  {30, 227},
  {30, 195},
  {38, 202},
  {21, 219}
};

uint16_t horn_table[9][2] = {
  // triangle 
  {135, 220},
  {135, 205},
  {144, 213},
  
  // rectangle
  {142, 210},
  {42, 5},    // width, height

  // line
  {152, 214},
  {152, 223},
  {179, 223},
  {179, 214},
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
  Serial.println("Hello!");

  delay(500);
  display.begin(16000000);
  display.drawRGBBitmap(0, 0, img, imgWidth, imgHeight);



  // winker点滅割り込み
  winker_left.attach(0.2, []() {
    display.fillCircle(winker_coor[0][0], winker_coor[0][1], winker_coor[0][2], ILI9327_BLACK);
    display.fillCircle(winker_coor[1][0], winker_coor[1][1], winker_coor[1][2], ILI9327_BLACK);
    display.fillCircle(winker_coor[2][0], winker_coor[2][1], winker_coor[2][2], ILI9327_BLACK);

    if(winker_count_left%2==0){
      display.fillCircle(winker_coor[0][0], winker_coor[0][1], winker_coor[0][2], ILI9327_YELLOW);
      display.fillCircle(winker_coor[1][0], winker_coor[1][1], winker_coor[1][2], ILI9327_YELLOW);
      display.fillCircle(winker_coor[2][0], winker_coor[2][1], winker_coor[2][2], ILI9327_YELLOW);
    }
    winker_count_left++;
   });
  // winker点滅割り込み
  winker_right.attach(0.2, []() {
    display.fillCircle(winker_coor[3][0], winker_coor[3][1], winker_coor[3][2], ILI9327_BLACK);
    display.fillCircle(winker_coor[4][0], winker_coor[4][1], winker_coor[4][2], ILI9327_BLACK);
    display.fillCircle(winker_coor[5][0], winker_coor[5][1], winker_coor[5][2], ILI9327_BLACK);

    if(winker_count_right%2==0){
      display.fillCircle(winker_coor[3][0], winker_coor[3][1], winker_coor[3][2], ILI9327_YELLOW);
      display.fillCircle(winker_coor[4][0], winker_coor[4][1], winker_coor[4][2], ILI9327_YELLOW);
      display.fillCircle(winker_coor[5][0], winker_coor[5][1], winker_coor[5][2], ILI9327_YELLOW);
    }
    winker_count_right++;
   });

    // バッテリ残量
    battery.attach(1.0, [](){
    display.fillRect(330-display_bias_w+10*remained_battery, 210, 9, 20, ILI9327_BLACK);
    if(charge){
      display.fillRect(330-display_bias_w+10*remained_battery, 210, 9, 20, ILI9327_GREEN);
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
      for(int n=0; n<5; n++){
        display.drawLine(bt_table[n][0], bt_table[n][1], bt_table[n+1][0], bt_table[n+1][1], ILI9327_CYAN);
      }
    }
    else{
      for(int n=0; n<5; n++){
        display.drawLine(bt_table[n][0], bt_table[n][1], bt_table[n+1][0], bt_table[n+1][1], ILI9327_BLACK);
      }
    }
    buletooth_on = !buletooth_on;
  });


  // Light
  light.attach(1.0, [](){
    if(light_on){
      display.fillCircle(88, 204, 7, ILI9327_YELLOW);
    }
    else{
      display.fillCircle(88, 204, 7, ILI9327_BLACK);
    }
    light_on = !light_on;
  });


  // Horn
  horn.attach(1.0, [](){
    if(horn_on){
      // triangle
      display.fillTriangle(
        horn_table[0][0], horn_table[0][1],
        horn_table[1][0], horn_table[1][1],
        horn_table[2][0], horn_table[2][1],
        ILI9327_YELLOW
        );
      
      // rectangle
      display.fillRect(
        horn_table[3][0], horn_table[3][1],
        horn_table[4][0], horn_table[4][1],
        ILI9327_YELLOW
        );

      // line
      for(int i=5; i<8; i++){
        display.drawLine(
          horn_table[i][0], horn_table[i][1],
          horn_table[i+1][0], horn_table[i+1][1],
          ILI9327_YELLOW
        );
      }
      
    }
    else{
            // triangle
      display.fillTriangle(
        horn_table[0][0], horn_table[0][1],
        horn_table[1][0], horn_table[1][1],
        horn_table[2][0], horn_table[2][1],
        ILI9327_BLACK
        );
      
      // rectangle
      display.fillRect(
        horn_table[3][0], horn_table[3][1],
        horn_table[4][0], horn_table[4][1],
        ILI9327_BLACK
        );

      // line
      for(int i=5; i<8; i++){
        display.drawLine(
          horn_table[i][0], horn_table[i][1],
          horn_table[i+1][0], horn_table[i+1][1],
          ILI9327_BLACK
        );
      }
    }
    horn_on = !horn_on;
  });


  // ODO
  odo.attach(3.0, [](){
    display.setFont(&FreeSans9pt7b);
    display.setTextSize(1);
    display.setTextColor(ILI9327_BLACK, ILI9327_BLACK);
    display.setCursor(221,234);
    display.println(odo_val);  // クリア
    // display.setTextSize(3);
    odo_val++;
    display.setTextColor(ILI9327_WHITE, ILI9327_WHITE);               // 描画
    display.setCursor(221,234);
    display.println(odo_val);  // クリア

  });
  delay(10);
}

int min_range = 130;
int max_range = 405;
int max_mph = 18;

int i = min_range;
int oi = i;

int aa, bb;
bool flag=true;

void loop()
{
  // 文字をいい感じ配置するためのバイアス
  if(max_mph*(i-min_range)/(max_range - min_range) != max_mph*(oi-min_range)/(max_range - min_range)){
    if(max_mph*(oi-min_range)/(max_range - min_range) < 10){
      aa = 2.5;
      }
    else{
      aa = 1; 
      }
    if(max_mph*(i-min_range)/(max_range - min_range) < 10){
      bb = 2.5;
      }
    else{
      bb = 1; 
      }

    //デジタル表示 
    display.setFont(&custom_windows_arial_24);
    display.setTextSize(3);
    display.setTextColor(ILI9327_BLACK, ILI9327_BLACK);
    display.setCursor(40*aa-display_bias_w,150);
    display.println(max_mph*(oi-min_range)/(max_range - min_range));  // クリア
    // display.setTextSize(3);
    display.setTextColor(ILI9327_WHITE, ILI9327_WHITE);               // 描画
    display.setCursor(40*bb-display_bias_w,150);
    display.println(max_mph*(i-min_range)/(max_range - min_range));
  }
  oi = i;

  display.drawLine(x_1, y_1, x_2_old, y_2_old, ILI9327_BLACK);

  x_2 = (uint16_t)(r * cos(i*PI/180.0) + b_x);
  y_2 = (uint16_t)(r * sin(i*PI/180.0) + b_y);
  x_2_old = x_2;
  y_2_old = y_2;

  display.drawLine(x_1, y_1, x_2, y_2, ILI9327_GREEN);

  display.fillCircle(x_1, y_1, 5, ILI9327_PINK);

  delay(50);


  if(flag)
    i++;
  else
    i--;
  if (i>max_range)
    flag = false;
  else if (i<min_range)
    flag = true;
}
