#include "Arduino.h"
#include "src/camera/buffered/BufferedCameraOV7670.h"
#include "src/camera/buffered/stm32_72mhz/BufferedCameraOV7670_QQVGA_30hz.h"
#include "src/camera/buffered/stm32_72mhz/BufferedCameraOV7670_QQVGA.h"
#include "src/camera/buffered/stm32_72mhz/BufferedCameraOV7670_QVGA.h"
#include <string.h>

#include "src/my_SSD1306.h"
#include <SPI.h>

#define OLED_DC PA1
#define OLED_CS PA2
#define OLED_RESET PA4
SSD1306 oled(OLED_DC, OLED_RESET, OLED_CS);
uint8_t* buf;

// OV7670 pinmap
// A8 - MCLK
// PB4 - PCLK
// PB5 - vsync
// PB6 - i2c Clock
// PB7 - i2c data
// PB8..PB15 D0-D7



BufferedCameraOV7670_QQVGA camera(CameraOV7670::PIXEL_YUV422, BufferedCameraOV7670_QQVGA::FPS_15_Hz);
//BufferedCameraOV7670_QVGA camera(CameraOV7670::PIXEL_RGB565, BufferedCameraOV7670_QVGA::FPS_7p5_Hz);

#define WIDTH 128
#define HEIGHT 64
#define THRESHOLD 80
//static uint8_t savebuf[WIDTH*HEIGHT*2];
void inline copy(uint8_t *dst, const uint8_t *src, uint16_t num)
{
  while (num--)
    *(dst++) = *(src++);
}

void inline copy2(void *dst, const void *src, uint16_t num)
{
  int nchunks = num / sizeof(int); /*按CPU位宽拷贝*/
  int slice =   num % sizeof(int); /*剩余的按字节拷贝*/

  unsigned int * s = (unsigned int *)src;
  unsigned int * d = (unsigned int *)dst;

  while (nchunks--)
    *d++ = *s++;

  while (slice--)
    *((char *)d++) = *((char *)s++);
}

static const uint8_t* pixel_buffer;
static uint16_t buffer_length;

void setup() {
  SPI.begin();
  oled.ssd1306_init(SSD1306_SWITCHCAPVCC);
  oled.display(); // show splashscreen
  buf = oled.getBuffer();
  delay(2000);
  Serial.println(F("Start"));
  camera.init();
  Serial.println(F("Init"));
  pixel_buffer = camera.getPixelBuffer();
  buffer_length = camera.getPixelBufferLength();
  systick_disable();
}


void loop() {
  memset(buf, 0, WIDTH*HEIGHT/8);
  camera.waitForVsync();
  noInterrupts();
  uint16_t cur = 0;

  /*while(cur < (120-HEIGHT)/2){
    cur++;
    camera.readLine();
  }*/
  uint16_t nextX = 0;
  uint16_t nextY = 0;
  uint8_t cX = 0;
  uint8_t cY = 0;
  for (uint16_t j = 0; j < camera.getLineCount(); j++) {
    camera.readLine();
    if(j == nextY){
      for(int i=0; i<160; i++){
        if(i == nextX){
          uint8_t intensity = pixel_buffer[i*2];
          if(intensity > THRESHOLD){
            buf[cX+cY/8*WIDTH] = buf[cX+cY/8*WIDTH] | (1<<(cY%8));
          } else {
            buf[cX+cY/8*WIDTH] = buf[cX+cY/8*WIDTH] & ~(1<<(cY%8));
          }
          cX++;
          uint16_t tmp = 160 * cX;
          nextX = tmp / WIDTH + (tmp%WIDTH > WIDTH/2? 1:0);
        }
      }
      cX = 0;
      nextX = 0;
      cY++;
      uint16_t tmp = 120 * cY;
      nextY = tmp / HEIGHT + (tmp%HEIGHT > HEIGHT/2? 1:0);
    }
  }
  interrupts();
  oled.display();
}
