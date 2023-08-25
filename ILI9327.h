#ifndef _ILI9327_H_
#define _ILI9327_H_

#include "Adafruit_GFX.h"
#include "Arduino.h"
#include "Print.h"
#include "Adafruit_SPITFT_DBI_TYPE_C_MODIFIED.h"
#include <SPI.h>

#define ILI9327_TFTWIDTH 400  ///< ILI9327 max TFT width
#define ILI9327_TFTHEIGHT 240 ///< ILI9327 max TFT height

#define ILI9327_NOP 0x00     ///< No-op register
#define ILI9327_SWRESET 0x01 ///< Software reset register
#define ILI9327_RDDID 0x04   ///< Read display identification information
#define ILI9327_RDDST 0x09   ///< Read Display Status

#define ILI9327_SLPIN 0x10  ///< Enter Sleep Mode
#define ILI9327_SLPOUT 0x11 ///< Sleep Out
#define ILI9327_PTLON 0x12  ///< Partial Mode ON
#define ILI9327_NORON 0x13  ///< Normal Display Mode ON

#define ILI9327_RDMODE 0x0A     ///< Read Display Power Mode
#define ILI9327_RDMADCTL 0x0B   ///< Read Display MADCTL
#define ILI9327_RDPIXFMT 0x0C   ///< Read Display Pixel Format
#define ILI9327_RDIMGFMT 0x0D   ///< Read Display Image Format
#define ILI9327_RDSELFDIAG 0x0F ///< Read Display Self-Diagnostic Result

#define ILI9327_INVOFF 0x20   ///< Display Inversion OFF
#define ILI9327_INVON 0x21    ///< Display Inversion ON
#define ILI9327_GAMMASET 0x26 ///< Gamma Set
#define ILI9327_DISPOFF 0x28  ///< Display OFF
#define ILI9327_DISPON 0x29   ///< Display ON

#define ILI9327_SET_COLUMN_ADDRESS 0x2A  ///< Column Address Set
#define ILI9327_SET_PAGE_ADDRESS 0x2B    ///< Page Address Set
#define ILI9327_WRIGHT_MEMORY_START 0x2C ///< Memory Write
#define ILI9327_RAMRD 0x2E               ///< Memory Read

#define ILI9327_PTLAR 0x30    ///< Partial Area
#define ILI9327_VSCRDEF 0x33  ///< Vertical Scrolling Definition
#define ILI9327_MADCTL 0x36   ///< Memory Access Control
#define ILI9327_VSCRSADD 0x37 ///< Vertical Scrolling Start Address
#define ILI9327_PIXFMT 0x3A   ///< COLMOD: Pixel Format Set

#define ILI9327_FRMCTR1 \
  0xB1                       ///< Frame Rate Control (In Normal Mode/Full Colors)
#define ILI9327_FRMCTR2 0xB2 ///< Frame Rate Control (In Idle Mode/8 colors)
#define ILI9327_FRMCTR3 \
  0xB3                       ///< Frame Rate control (In Partial Mode/Full Colors)
#define ILI9327_INVCTR 0xB4  ///< Display Inversion Control
#define ILI9327_DFUNCTR 0xB6 ///< Display Function Control

#define ILI9327_PWCTR1 0xC0 ///< Power Control 1
#define ILI9327_PWCTR2 0xC1 ///< Power Control 2
#define ILI9327_PWCTR3 0xC2 ///< Power Control 3
#define ILI9327_PWCTR4 0xC3 ///< Power Control 4
#define ILI9327_PWCTR5 0xC4 ///< Power Control 5
#define ILI9327_VMCTR1 0xC5 ///< VCOM Control 1
#define ILI9327_VMCTR2 0xC7 ///< VCOM Control 2

#define ILI9327_RDID1 0xDA ///< Read ID 1
#define ILI9327_RDID2 0xDB ///< Read ID 2
#define ILI9327_RDID3 0xDC ///< Read ID 3
#define ILI9327_RDID4 0xDD ///< Read ID 4

#define ILI9327_GMCTRP1 0xE0 ///< Positive Gamma Correction
#define ILI9327_GMCTRN1 0xE1 ///< Negative Gamma Correction
// #define ILI9327_PWCTR6     0xFC

// Color definitions
#define ILI9327_BLACK 0x0000       ///<   0,   0,   0
#define ILI9327_NAVY 0x000F        ///<   0,   0, 123
#define ILI9327_DARKGREEN 0x03E0   ///<   0, 125,   0
#define ILI9327_DARKCYAN 0x03EF    ///<   0, 125, 123
#define ILI9327_MAROON 0x7800      ///< 123,   0,   0
#define ILI9327_PURPLE 0x780F      ///< 123,   0, 123
#define ILI9327_OLIVE 0x7BE0       ///< 123, 125,   0
#define ILI9327_LIGHTGREY 0xC618   ///< 198, 195, 198
#define ILI9327_DARKGREY 0x7BEF    ///< 123, 125, 123
#define ILI9327_BLUE 0x001F        ///<   0,   0, 255
#define ILI9327_GREEN 0x07E0       ///<   0, 255,   0
#define ILI9327_CYAN 0x07FF        ///<   0, 255, 255
#define ILI9327_RED 0xF800         ///< 255,   0,   0
#define ILI9327_MAGENTA 0xF81F     ///< 255,   0, 255
#define ILI9327_YELLOW 0xFFE0      ///< 255, 255,   0
#define ILI9327_WHITE 0xFFFF       ///< 255, 255, 255
#define ILI9327_ORANGE 0xFD20      ///< 255, 165,   0
#define ILI9327_GREENYELLOW 0xAFE5 ///< 173, 255,  41
#define ILI9327_PINK 0xFC18        ///< 255, 130, 198

class ILI9327 : public Adafruit_SPITFT_DBI_TYPE_C_MODIFIED {
 private:
 public:
  ILI9327(int8_t cs, int8_t dc, int8_t mosi, int8_t scl, int8_t rst, int8_t miso);
  ILI9327(int8_t cs, int8_t dc, SPIClass *spi, int8_t rst);
  void begin(uint32_t freq = 0) override;
  void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
  ~ILI9327();
};

#endif // _ILI9327_H_
