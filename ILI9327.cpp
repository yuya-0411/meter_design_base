#include "Adafruit_GFX.h"
#include "Arduino.h"
#include "Print.h"
#include "Adafruit_SPITFT_DBI_TYPE_C_MODIFIED.h"
#include <SPI.h>
#include <limits.h>

#include "ILI9327.h"

// 通信速度
#if defined(ARDUINO_ARCH_ARC32) || defined(ARDUINO_MAXIM)
#define SPI_DEFAULT_FREQ 16000000
// Teensy 3.0, 3.1/3.2, 3.5, 3.6
#elif defined(__MK20DX128__) || defined(__MK20DX256__) || \
    defined(__MK64FX512__) || defined(__MK66FX1M0__)
#define SPI_DEFAULT_FREQ 40000000
#elif defined(__AVR__) || defined(TEENSYDUINO)
#define SPI_DEFAULT_FREQ 8000000
#elif defined(ESP8266) || defined(ESP32)
#define SPI_DEFAULT_FREQ 40000000
#elif defined(RASPI)
#define SPI_DEFAULT_FREQ 80000000
#elif defined(ARDUINO_ARCH_STM32F1)
#define SPI_DEFAULT_FREQ 36000000
#else
#define SPI_DEFAULT_FREQ 24000000 ///< Default SPI data clock frequency
#endif

ILI9327::ILI9327(int8_t cs, int8_t dc, int8_t mosi, int8_t scl, int8_t rst = -1, int8_t miso = -1)
    : Adafruit_SPITFT_DBI_TYPE_C_MODIFIED(ILI9327_TFTWIDTH, ILI9327_TFTHEIGHT, cs, dc, mosi, scl, rst, miso)
{
}

ILI9327::ILI9327(int8_t cs, int8_t dc, SPIClass *spi, int8_t rst = -1)
    : Adafruit_SPITFT_DBI_TYPE_C_MODIFIED(ILI9327_TFTWIDTH, ILI9327_TFTHEIGHT, spi, cs, dc, rst)
{
}

// clang-format off
static const uint8_t PROGMEM initcmd[] = {
  ILI9327_MADCTL  , 1, 0x20,             // Memory Access Control
  ILI9327_VSCRSADD, 1, 0x00,             // Vertical scroll zero
  ILI9327_PIXFMT  , 1, 0x06,
  ILI9327_SLPOUT  , 0,                // Exit Sleep
  ILI9327_DISPON  , 0,                // Display on
  0x00                                   // End of list
};
// clang-format on

void ILI9327::begin(uint32_t freq)
{
  if (!freq)
    freq = SPI_DEFAULT_FREQ;
  initSPI(freq, SPI_MODE3);

  if (_rst < 0) {                 // If no hardware reset pin...
    sendCommand(ILI9327_SWRESET); // Engage software reset
    delay(150);
  }

  uint8_t cmd, x, numArgs;
  const uint8_t *addr = initcmd;
  while ((cmd = pgm_read_byte(addr++)) > 0) {
    x = pgm_read_byte(addr++);
    numArgs = x & 0x7F;
    sendCommand(cmd, addr, numArgs);
    addr += numArgs;
    if (x & 0x80)
      delay(150);
  }

  _width = ILI9327_TFTWIDTH;
  _height = ILI9327_TFTHEIGHT;
}

void ILI9327::setAddrWindow(uint16_t x1, uint16_t y1, uint16_t w,
                                     uint16_t h) {
  static uint16_t old_x1 = 0xffff, old_x2 = 0xffff;
  static uint16_t old_y1 = 0xffff, old_y2 = 0xffff;

  uint16_t x2 = (x1 + w - 1), y2 = (y1 + h - 1);
  if (x1 != old_x1 || x2 != old_x2) {
    writeCommand(ILI9327_SET_COLUMN_ADDRESS); // Column address set
    SPI_WRITE16(x1);
    SPI_WRITE16(x2);
    old_x1 = x1;
    old_x2 = x2;
  }
  if (y1 != old_y1 || y2 != old_y2) {
    writeCommand(ILI9327_SET_PAGE_ADDRESS); // Row address set
    SPI_WRITE16(y1);
    SPI_WRITE16(y2);
    old_y1 = y1;
    old_y2 = y2;
  }
  writeCommand(ILI9327_WRIGHT_MEMORY_START); // Write to RAM
}

ILI9327::~ILI9327()
{
}
