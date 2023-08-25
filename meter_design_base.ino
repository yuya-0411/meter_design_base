#include <Ticker.h>
#include <EEPROM.h>

#include "ILI9327.h"
#include "meter_base7.h"
#include <SPI.h>

// #include <Fonts\FreeSansBold24pt7b.h>
// #include <Fonts\DSEG7_Modern_Bold_24.h>
#include <Fonts\custom_windows_arial_24.h>


#define DISPLAY_CS 5
#define DISPLAY_DC 26
#define DISPLAY_MOSI 23
#define DISPLAY_MISO 19
#define DISPLAY_SCK 18
#define DISPLAY_RST 27

uint16_t x_0 = 0;
uint16_t y_0 = 0;
uint16_t b_x = 325;
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
  {323, 23, 1},
  {347, 23, 5},
  {379, 23, 10}
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



Ticker winker_left;
Ticker winker_right;
int winker_count_left = 0;
int winker_count_right = 0;

Ticker battery;
int remained_battery = 4;
int old_remained_battery = remained_battery;
bool charge = false;

Ticker bluetooth;
int buletooth_on = false;

Ticker light;
int light_on = false;

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
    display.fillRect(330+10*remained_battery, 210, 9, 20, ILI9327_BLACK);
    if(charge){
      display.fillRect(330+10*remained_battery, 210, 9, 20, ILI9327_GREEN);
      // old_remained_battery = remained_battery;
      remained_battery++;
    }
    else{
      // old_remained_battery = remained_battery;
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
    display.setFont(&custom_windows_arial_24);
    display.setTextSize(3);
    display.setTextColor(ILI9327_BLACK, ILI9327_BLACK);
    display.setCursor(40*aa,150);
    display.println(max_mph*(oi-min_range)/(max_range - min_range));
    // display.setTextSize(3);
    display.setTextColor(ILI9327_WHITE, ILI9327_WHITE);
    display.setCursor(40*bb,150);
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
