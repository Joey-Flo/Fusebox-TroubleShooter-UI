
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <esp_task_wdt.h>
#define CONFIG_ARDUINO_LOOP_STACK_SIZE 37268
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#include <esp_task_wdt.h>
#include <driver/timer.h>

#define WDT_TIMEOUT 3

void disableTGWDT() {
  // Disable Timer Group 0 Watchdog Timer
  TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;  // Write enable
  TIMERG0.wdt_config0.en = 0;                  // Disable TG0 WDT
  TIMERG0.wdt_wprotect = 0;                    // Write protect

  // Disable Timer Group 1 Watchdog Timer
  TIMERG1.wdt_wprotect = TIMG_WDT_WKEY_VALUE;  // Write enable
  TIMERG1.wdt_config0.en = 0;                  // Disable TG1 WDT
  TIMERG1.wdt_wprotect = 0;                    // Write protect
}

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define FRAME_DELAY (42)
#define FRAME_WIDTH (32)
#define FRAME_HEIGHT (32)
#define FRAME_COUNT (sizeof(frames) / sizeof(frames[0]))
int GeneralCounter = 0;
int CommonGround = 0;
float DisplayChoice = 0;
float AmpBell = 0;
int DifferenceValue = 0;
int PreviousAutoState = 0;
int TimeElapsedState = 0;
int TimeSinceTrip = 0;
int y_valRST = 0;
int y_valDLY = 5;
int SecondsValue = 30;
int AutoResetValue = 0;
int position_y_RST = 0;
int position_y_DLY = 5;
float FuseValue = 20.00;
bool previousButtonState = LOW;
bool PreviousRun = LOW;
bool previousUpState = LOW;
bool previousDownState = LOW;
int counter = 0;
int overcurrent_counter = 0;
bool FuseState = true;
int RunCounter = 0;
int PreviousRunCounter = 0;
int DisplayOption = 0;
int AvgFuseCounter = 0;
int AutoResetState = 0;
float AvgFuseValTotal = 0;
char Fuse[6]; // Increase the size to accommodate the null terminator '\0'

  // Serial.println(Fuse[6]);
int frame = 0;
const int digitalPin1 = 13;
const int digitalPin2 = 12;
const int digitalPin3 = 27;
const int digitalPin4 = 26;
const int buttonpin5 = 25;
const int buttonpin6 =17;
const int digitalPin8 = 5;
const int BeepPin = 4;
const int pwmPin = 16;  // The pin you want to use for PWM
const int pwmChannel = 0;
const int pwmFrequency = 50;  // Frequency in Hertz
const int pwmResolution = 8; 

// 'Amp', 20x18px
const byte PROGMEM frames[][128] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,129,128,0,0,135,192,0,0,132,32,0,0,140,48,0,0,136,24,0,0,152,8,0,0,144,12,0,0,176,4,0,0,176,4,0,0,160,6,0,0,160,2,0,0,224,3,0,0,255,249,63,255,255,249,63,255,128,1,128,4,128,0,128,4,128,0,128,12,128,0,192,12,128,0,64,8,128,0,96,24,128,0,32,16,128,0,48,48,128,0,16,32,128,0,24,96,128,0,15,192,128,0,3,128,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,129,128,0,0,135,192,0,0,132,32,0,0,140,48,0,0,136,24,0,0,152,8,0,0,144,12,0,0,176,4,0,0,176,4,0,0,160,6,0,0,160,2,0,0,224,3,0,0,255,249,63,255,255,249,63,255,128,1,128,4,128,0,128,4,128,0,128,12,128,0,192,12,128,0,64,8,128,0,96,24,128,0,32,16,128,0,48,48,128,0,16,32,128,0,24,96,128,0,15,192,128,0,3,128,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,129,128,0,0,135,192,0,0,132,32,0,0,140,48,0,0,136,24,0,0,152,8,0,0,144,12,0,0,176,4,0,0,176,4,0,0,160,6,0,0,160,2,0,0,224,3,0,0,255,249,63,255,255,249,63,255,128,1,128,4,128,0,128,4,128,0,128,12,128,0,192,12,128,0,64,8,128,0,96,24,128,0,32,16,128,0,48,48,128,0,16,32,128,0,24,96,128,0,15,192,128,0,3,128,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,129,128,0,0,135,192,0,0,132,32,0,0,140,48,0,0,136,24,0,0,152,8,0,0,144,12,0,0,176,4,0,0,176,4,0,0,160,6,0,0,160,2,0,0,224,3,0,0,255,249,63,255,255,249,63,255,128,1,128,4,128,0,128,4,128,0,128,12,128,0,192,12,128,0,64,8,128,0,96,24,128,0,32,16,128,0,48,48,128,0,16,32,128,0,24,96,128,0,15,192,128,0,3,128,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,129,128,0,0,135,224,0,0,132,32,0,0,140,48,0,0,136,24,0,0,152,8,0,0,144,12,0,0,144,4,0,0,176,6,0,0,160,6,0,0,160,2,0,0,224,3,0,0,255,249,63,255,255,249,63,255,128,1,128,4,128,0,128,4,128,0,128,12,128,0,192,12,128,0,64,8,128,0,96,24,128,0,32,16,128,0,48,48,128,0,16,32,128,0,24,96,128,0,15,192,128,0,3,128,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,129,128,0,0,131,192,0,0,132,32,0,0,140,16,0,0,136,16,0,0,152,8,0,0,144,12,0,0,144,4,0,0,128,4,0,0,128,6,0,0,128,2,0,0,128,3,0,0,255,249,63,255,255,249,63,255,128,1,128,4,128,0,128,4,128,0,128,12,128,0,192,12,128,0,64,8,128,0,96,24,128,0,32,16,128,0,48,48,128,0,16,32,128,0,24,96,128,0,15,192,128,0,3,128,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,129,128,0,0,131,224,0,0,128,32,0,0,128,16,0,0,128,24,0,0,128,8,0,0,128,12,0,0,128,4,0,0,128,6,0,0,128,6,0,0,128,2,0,0,128,3,0,0,255,249,63,255,255,249,63,255,128,1,128,4,128,0,128,4,128,0,128,12,128,0,192,12,128,0,64,8,128,0,96,24,128,0,32,16,128,0,48,48,128,0,16,32,128,0,24,96,128,0,15,192,128,0,3,128,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,4,0,0,128,4,0,0,128,6,0,0,128,2,0,0,128,3,0,0,255,249,63,255,255,249,63,255,128,1,128,4,128,0,128,4,128,0,128,12,128,0,192,12,128,0,64,8,128,0,96,24,128,0,32,16,128,0,48,48,128,0,16,32,128,0,24,96,128,0,15,192,128,0,3,128,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,255,255,255,255,255,255,255,255,128,0,0,4,128,0,0,4,128,0,0,12,128,0,0,12,128,0,0,8,128,0,96,24,128,0,32,16,128,0,48,48,128,0,16,32,128,0,8,96,128,0,15,192,128,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,255,255,255,255,255,255,255,255,128,0,0,4,128,0,0,4,128,0,0,12,128,0,0,12,128,0,0,8,128,0,0,24,128,0,0,16,128,0,0,48,128,0,0,32,128,0,0,0,128,0,0,0,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,255,255,255,255,255,255,255,255,128,0,0,4,128,0,0,4,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,255,255,255,255,255,255,255,255,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,255,255,255,255,255,255,255,255,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,255,255,255,255,255,255,255,255,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,255,255,255,255,255,255,255,255,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,255,255,255,255,255,255,255,255,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,255,255,255,255,255,255,255,255,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,255,255,255,255,255,255,255,255,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,160,0,0,0,224,0,0,0,255,255,255,255,255,255,255,255,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,128,0,0,0,128,0,0,0,132,0,0,0,140,0,0,0,136,0,0,0,152,0,0,0,144,0,0,0,144,0,0,0,176,0,0,0,160,0,0,0,160,0,0,0,224,0,0,0,255,255,255,255,255,255,255,255,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,129,128,0,0,131,224,0,0,132,32,0,0,140,16,0,0,136,24,0,0,152,8,0,0,144,12,0,0,144,4,0,0,176,0,0,0,160,0,0,0,160,0,0,0,224,0,0,0,255,255,255,255,255,255,255,255,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,129,128,0,0,135,192,0,0,132,32,0,0,140,48,0,0,136,24,0,0,152,8,0,0,144,12,0,0,176,4,0,0,176,4,0,0,160,6,0,0,160,2,0,0,224,3,0,0,255,249,63,255,255,249,63,255,128,1,128,0,128,0,128,0,128,0,128,0,128,0,192,0,128,0,64,0,128,0,96,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,129,128,0,0,135,192,0,0,132,32,0,0,140,48,0,0,136,24,0,0,152,8,0,0,144,12,0,0,176,4,0,0,176,4,0,0,160,6,0,0,160,2,0,0,224,3,0,0,255,249,63,255,255,249,63,255,128,1,128,0,128,0,128,0,128,0,128,0,128,0,192,0,128,0,64,0,128,0,96,0,128,0,32,0,128,0,48,0,128,0,16,0,128,0,8,0,128,0,15,192,128,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,129,128,0,0,135,192,0,0,132,32,0,0,140,48,0,0,136,24,0,0,152,8,0,0,144,12,0,0,176,4,0,0,176,4,0,0,160,6,0,0,160,2,0,0,224,3,0,0,255,249,63,255,255,249,63,255,128,1,128,0,128,0,128,0,128,0,128,0,128,0,192,0,128,0,64,8,128,0,96,24,128,0,32,16,128,0,48,48,128,0,16,32,128,0,8,96,128,0,15,192,128,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,129,128,0,0,135,192,0,0,132,32,0,0,140,48,0,0,136,24,0,0,152,8,0,0,144,12,0,0,176,4,0,0,176,4,0,0,160,6,0,0,160,2,0,0,224,3,0,0,255,249,63,255,255,249,63,255,128,1,128,4,128,0,128,4,128,0,128,12,128,0,192,8,128,0,64,8,128,0,96,24,128,0,32,16,128,0,48,48,128,0,16,32,128,0,24,96,128,0,15,192,128,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,129,128,0,0,135,192,0,0,132,32,0,0,140,48,0,0,136,24,0,0,152,8,0,0,144,12,0,0,176,4,0,0,176,4,0,0,160,6,0,0,160,2,0,0,224,3,0,0,255,249,63,255,255,249,63,255,128,1,128,4,128,0,128,4,128,0,128,12,128,0,192,12,128,0,64,8,128,0,96,24,128,0,32,16,128,0,48,48,128,0,16,32,128,0,24,96,128,0,15,192,128,0,3,128,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,129,128,0,0,135,192,0,0,132,32,0,0,140,48,0,0,136,24,0,0,152,8,0,0,144,12,0,0,176,4,0,0,176,4,0,0,160,6,0,0,160,2,0,0,224,3,0,0,255,249,63,255,255,249,63,255,128,1,128,4,128,0,128,4,128,0,128,12,128,0,192,12,128,0,64,8,128,0,96,24,128,0,32,16,128,0,48,48,128,0,16,32,128,0,24,96,128,0,15,192,128,0,3,128,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,129,128,0,0,135,192,0,0,132,32,0,0,140,48,0,0,136,24,0,0,152,8,0,0,144,12,0,0,176,4,0,0,176,4,0,0,160,6,0,0,160,2,0,0,224,3,0,0,255,249,63,255,255,249,63,255,128,1,128,4,128,0,128,4,128,0,128,12,128,0,192,12,128,0,64,8,128,0,96,24,128,0,32,16,128,0,48,48,128,0,16,32,128,0,24,96,128,0,15,192,128,0,3,128,0,0,0,0,0,0,0,0,0,0,0,0}
};

const unsigned char epd_bitmap_Amp [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x01, 0xf8, 0x00, 0x01, 0xf8, 0x00, 0x01, 
	0xfc, 0x00, 0x03, 0xfc, 0x00, 0x03, 0x9c, 0x00, 0x07, 0x9e, 0x00, 0x07, 0x9e, 0x00, 0x07, 0xfe, 
	0x00, 0x0f, 0xff, 0x00, 0x0f, 0xff, 0x00, 0x0f, 0x0f, 0x00, 0x1f, 0x07, 0x80, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 80)
const int epd_bitmap_allArray_LEN = 1;
const unsigned char* epd_bitmap_allArray[1] = {
	epd_bitmap_Amp
};

// 'FLOW#', 103x70px
// 'FLOW#', 30x30px
// 'FLOW2', 28x19px
const unsigned char epd_bitmap_FLOW_ [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 
	0xc0, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x70, 0x00, 
	0x00, 0x00, 0x07, 0x00, 0x60, 0x00, 0x00, 0x00, 0x03, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x03, 0x00, 
	0xe0, 0x00, 0x00, 0x00, 0x03, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x03, 0x00, 0xe0, 0x00, 0x00, 0x00, 
	0x03, 0x00, 0xe3, 0xc8, 0x1c, 0x46, 0x23, 0x00, 0xe3, 0x0c, 0x76, 0x46, 0x23, 0x00, 0xe3, 0x0c, 
	0x63, 0x66, 0x63, 0x00, 0xe3, 0x0c, 0xc3, 0x6f, 0x63, 0x00, 0xe3, 0xcc, 0xc3, 0x2f, 0x43, 0x00, 
	0xe3, 0x0c, 0x43, 0x39, 0xc3, 0x00, 0xe3, 0x0c, 0x62, 0x39, 0xc3, 0x00, 0xe3, 0x0f, 0x3e, 0x39, 
	0x83, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x03, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x03, 0x00, 0xe0, 0x00, 
	0x00, 0x00, 0x03, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x03, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x03, 0x00, 
	0xe0, 0x00, 0x00, 0x00, 0x03, 0x00, 0x70, 0x00, 0x00, 0x00, 0x07, 0x00, 0x3f, 0xff, 0xff, 0xff, 
	0xfe, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x03, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};



// 'trip', 41x31px
const unsigned char epd_bitmap_trip [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 
	0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xe0, 0x18, 0x3e, 0x60, 0x7c, 0xe0, 0x18, 
	0x0c, 0x60, 0x3c, 0xfc, 0xf9, 0xcc, 0x67, 0x3c, 0xfc, 0xf9, 0xcc, 0x67, 0x1c, 0xfc, 0xf9, 0x8c, 
	0x67, 0x3c, 0xfc, 0xf8, 0x1c, 0x66, 0x3c, 0xfc, 0xf8, 0x1c, 0x60, 0x7c, 0xfc, 0xf9, 0x8c, 0x67, 
	0xfc, 0xfc, 0xf9, 0xcc, 0x67, 0xfc, 0xfc, 0xf9, 0xcc, 0x67, 0xfc, 0xfc, 0xf9, 0xc4, 0x67, 0xfc, 
	0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 
	0xff, 0xff, 0xff, 0xfc

};

// 'open switch', 42x20px
const unsigned char epd_bitmap_open_switch [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x18, 
	0x00, 0x0e, 0x00, 0x00, 0x7f, 0xfc, 0x00, 0x0f, 0xff, 0x00, 0x3f, 0xfc, 0x00, 0x0f, 0xff, 0x00, 
	0x00, 0x08, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'Flo2', 128x58px
const unsigned char epd_bitmap_Flo2 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0f, 0xff, 0xff, 0xff, 0xf8, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xfe, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xf8, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xfe, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xe0, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xc0, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xe0, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xe0, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xc0, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0x80, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x1f, 0xfe, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x07, 0xff, 0x00, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x03, 0xff, 0x80, 0x03, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xfe, 0x01, 0xff, 0xff, 0xff, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xfe, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 
	0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xfc, 0x00, 0x7f, 0xff, 0xff, 0xff, 0x00, 
	0x00, 0x1f, 0xf0, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xf8, 0x00, 0x3f, 0xff, 0xff, 0xfe, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 944)
const int epd_bitmap_allArray_LEN2 = 1;
const unsigned char* epd_bitmap_allArray2[1] = {
	epd_bitmap_Flo2
};


 int AmpMode() {
   if ((analogRead(15) * .00086) <=  4){
       for (int i = 0; i < 750; i++){
         AmpBell = ((((analogRead(15) * .00086)) * 3));
       }
   }

   if ((analogRead(15) * .00086) > 4){
     for (int i = 0; i < 500; i++){
       AmpBell = ((   ( .00086 * analogRead(15)) - (   ((analogRead(0) * .00086) * 0.5))) * 15.15) - 3.4;
     }
   }

     if (AmpBell < 0.15){
       AmpBell = 0;
     }
  return 0;
 }
int AvgFuseValue() {

  AvgFuseValTotal = (AmpBell);
  if (AvgFuseValTotal < 0.17){
    AvgFuseValTotal = 0;
  }

  AvgFuseCounter = 0;
  return static_cast<int>(AvgFuseValTotal);
}
 int BatteryLife() {
   display.fillRect(95, 0, 127 , 15, BLACK);
   int CurrentBattery = (analogRead(35) * 0.006);
   display.drawRect(105,1,20,10,WHITE);
   display.drawRect(104,0,22,12,WHITE);
   display.drawLine(105,4,105,7,BLACK);
   display.drawLine(104,4,104,7,BLACK);
   display.drawLine(102,3,102,8,WHITE);
   display.drawLine(103,3,103,8,WHITE);
   display.fillRect((123-CurrentBattery), 3, CurrentBattery , 6, WHITE);
   display.drawLine(111,3,111,8,BLACK);
   display.drawLine(115,3,115,8,BLACK);
   display.drawLine(119,3,119,8,BLACK);
   return 0;
 }
 int WattOption(){
   CommonGround = 0;
   byte CommonCheck1 = 0;
   byte CommonCheck2 = 0;
   byte CommonCheck3 = 0;
   int Check1 = analogRead(7);
   delay(7);
   int Check2 = analogRead(7);
   if ((((Check1 * 0.00086) - (Check2 * 0.00086)) < 0.3) and (((Check1 * 0.00086) - (Check2 * 0.00086)) > -0.3)){
     CommonCheck1 = 1;
   }
   Check1 = analogRead(7);
   delay(7);
   Check2 = analogRead(7);
   if ((((Check1 * 0.00086) - (Check2 * 0.00086)) < 0.3) and (((Check1 * 0.00086) - (Check2 * 0.00086)) > -0.3)){
     CommonCheck2 = 1;
   }
   Check1 = analogRead(7);
   delay(7);
   Check2 = analogRead(7);
   if ((((Check1 * 0.00086) - (Check2 * 0.00086)) < 0.3) and (((Check1 * 0.00086) - (Check2 * 0.00086)) > -0.3)){
     CommonCheck3 = 1;
   }
   if (((CommonCheck1 == 1) and (CommonCheck2 == 1)) and (CommonCheck3 == 1)){
     CommonGround = 1;
   }
   
       ButtonStates();
       if ((DisplayOption  == 0) or (CommonGround = 0)){
         DisplayChoice = AvgFuseValTotal;
       }
       if ((DisplayOption == 1) and (CommonGround = 1)){
         DisplayChoice = AvgFuseValTotal * ((analogRead(7) *  .00086) * 5);
       }
       if ((DisplayOption == 3) and (CommonGround = 1)){
         DisplayChoice = (((analogRead(7) *  .00086) * 5) / AvgFuseValTotal);
       }


   
  return 0;
 }
 int DisplayAmp(){
   int AmpExtent = 0;
   if (((AvgFuseValTotal)) > 9.99){
     AmpExtent = AmpExtent + 8;
   }

     if (((AvgFuseValTotal)) > 99.99){
     AmpExtent = AmpExtent + 16;
   }
   display.setTextSize(2.5);          // text size
   display.setTextColor(WHITE);     // text color
   display.setCursor(0, 30);        // position to display
   display.println((DisplayChoice)); // text to display      // show on display
  
   if (DisplayOption == 0){
     display.drawBitmap((52 + AmpExtent), 29, epd_bitmap_Amp, 20, 18, 1);
   }
  return 0;
 }
 int FlowTrip(){
   display.fillRect(90,12,40,80,BLACK);
   display.fillRect(0,45,80,80,BLACK);
   display.drawBitmap(88, 11, epd_bitmap_FLOW_, 41, 31, WHITE);
   display.fillRect(90,12,40,4,BLACK);
   display.fillRect(87,14,2,80,BLACK);
   display.fillRect(124,14,3,40,BLACK);
   display.fillRect(0,16,127,1,WHITE);
   display.fillRect(89,16,1,60,WHITE);
   display.fillRect(89,34,40,1,WHITE);
   if ((RunCounter % 2) == 1){
     digitalWrite(digitalPin8,LOW);
     AmpMode();
     display.fillRect(88, 35, 65, 32, BLACK);
     display.drawBitmap(91, 35, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
     if ((AmpBell) > (FuseValue)){
       AvgFuseValTotal = (AmpBell);
       TimeSinceTrip = millis();
       RunCounter ++;
     }

   }
   if ((RunCounter % 2) == 0){
     AutoReset();
     if (digitalRead(digitalPin2) == HIGH){
       AvgFuseValTotal = 0;
     }
     digitalWrite(digitalPin8,HIGH);
     if (PreviousRunCounter == 1){
       digitalWrite(BeepPin,HIGH);
       delay(50);
       digitalWrite(BeepPin,LOW);
       delay(50);
       digitalWrite(BeepPin,HIGH);
       delay(50);
       digitalWrite(BeepPin,LOW);
     }
     display.fillRect(91,17,50,60,BLACK);
     display.drawBitmap(90, 15, epd_bitmap_trip, 38, 20, WHITE);
     display.fillRect(90,42,40,80,BLACK);
     display.fillRect(5,50,80,80,BLACK);
     display.drawBitmap(90, 40, epd_bitmap_open_switch, 42, 20, WHITE);
     display.drawRoundRect(91, 17, 36, 16, 2, BLACK);
     if (AvgFuseValTotal > FuseValue){
       if (overcurrent_counter < 5 ){
          
           display.setTextColor(WHITE);     // text color
           display.setCursor(5, 50);  
           display.setTextSize(1);         // position to display
           display.println("OVERCURRENT"); // text to display      // show on display/'

       }
     }
    
   }
   PreviousRunCounter = (RunCounter % 2);
   display.fillRect(89,34,3,80,WHITE);
  return 0;
  }
 int ButtonStates(){
     int buttonState = digitalRead(buttonpin5);
   if (buttonState == HIGH && previousButtonState == LOW) {
     counter++;
   }
     int FuseState = digitalRead(buttonpin6);
   if (FuseState == HIGH && PreviousRun == LOW){
     RunCounter++;
     digitalWrite(BeepPin,HIGH);
     delay(50);
     digitalWrite(BeepPin,LOW);
   }
  
     int UpState = digitalRead(digitalPin3);
   if (UpState == HIGH && previousUpState == LOW) {
     DisplayOption++;
     if (DisplayOption == 4){
       DisplayOption = 0;
     }
   }
       int DownState = digitalRead(digitalPin4);
   if (DownState == HIGH && previousDownState == LOW) {
     DisplayOption--;
     if (DisplayOption == -1){
       DisplayOption = 3;
     }
   }
 

   if (counter == 1){
     display.clearDisplay();
     previousButtonState = buttonState;
     fuseDeploy();
   }
   previousButtonState = buttonState;
   PreviousRun = FuseState;
   previousUpState = UpState;
   previousDownState = DownState;
   if ((RunCounter % 2) == 0){
     FuseState = false;

   }
   if ((RunCounter % 2) == 1){
     FuseState = true;
   }
return 0;
 }
  int DipslayFuseValue() {
    display.drawRoundRect(0, 0, 90, 15, 4, WHITE);
    display.setTextSize(1.5);          // text size
    display.setTextColor(WHITE);     // text color
    display.setCursor(5, 4);        // position to display
    display.println("FUSE:"); // text to display      // show on display
    display.setTextSize(1.5);          // text size    // text color
    display.setCursor(40, 4);        // position to display
    display.println(FuseValue); // text to display      // show on display
    display.setTextSize(1.75);          // text size    // text color
    display.setCursor(75, 4); 
    display.println("A");
  return 0;
  }
 int curserPlacement(){
   int SpaceValue = 0;
   int position = 0;
   int Number = 0;


   bool loop = true;
   while (loop == true){

     bool Right = digitalRead(digitalPin1) == HIGH;
     bool Left = digitalRead(digitalPin2) == HIGH;
     bool Up = digitalRead(digitalPin3) == HIGH;
     bool Down = digitalRead(digitalPin4) == HIGH;

     display.fillRect(30, 21, 65, 8, BLACK);
     display.fillRect(30, 47, 65, 8, BLACK);
     display.fillRect(0, 45, 10, 20, BLACK);
     display.fillRect(30, 30, 70, 18, BLACK);


     if (Right == 1){
       position = position +1;
     }

     if (Left == 1) {
       position = position -1;
     }

     if (position < 0) {
       position = 3;
     }

     if (position > 3) {
       position = 0;
     }

     if (position < 2) {
        Number = Fuse[position] - '0';
     }
     if (position >= 2){
         Number = Fuse[position + 1] - '0';
     }
     if (Up == 1){
       Number = (Number +1);
     }

     if (Down == 1) {
       Number = (Number -1);
     }


     if (Number < 0) {
       Number = 9;
     }

     if (Number > 9) {
       Number = 0;

     }
     if (position == 0) {
       SpaceValue = 0;
     }

     if (position == 1) {
       SpaceValue = 13;
     }

     if (position == 2) {
       SpaceValue = 35;
     }

     if (position == 3) {
       SpaceValue = 48;
     }

     if (position < 2){
       Fuse[position] = Number + '0';
     }
     if (position >= 2){
       Fuse[position + 1] = Number + '0';
    
     }

     int buttonState = digitalRead(buttonpin5);
     if (buttonState == HIGH && previousButtonState == LOW) {
     counter++;
     }


     previousButtonState = buttonState;
     display.fillTriangle((31 + SpaceValue) ,21,(34 + SpaceValue),25,(37 + SpaceValue),21,WHITE);
     display.fillTriangle((31 + SpaceValue),51,(34 + SpaceValue),47,(37 + SpaceValue),51,WHITE);

     display.drawBitmap(95, 29, epd_bitmap_Amp, 20, 18, WHITE);
     display.setTextSize(2);          // text size
     display.setTextColor(WHITE);     // text color
     display.setCursor(30, 30);        // position to display
     display.println(Fuse); // text to display      // show on display
     BatteryLife();
     display.display();

    
     if (counter == 2){
       loop = false;
       FuseValue = atof(Fuse);
       display.clearDisplay();
       Automode();
     }

 
     delay(175);
   }
   return 0;
 }
  int fuseDeploy(){
   int curservalue = 0;
   char Fuse[6]; // Increase the size to accommodate the null terminator '\0'
   char SecondFuse[10];
   // Serial.println(Fuse[6]);
   dtostrf(FuseValue, 4, 2, Fuse); // Convert float to string with 4 digits, 2 decimal places
   if (FuseValue < 10){
     Fuse[5] = Fuse[4];
     Fuse[4] = Fuse[3];
     Fuse[3] = Fuse[2];
     Fuse[2] = Fuse[1];
     Fuse[1] = Fuse[0];
     Fuse[0] = '0';
   }
   display.clearDisplay();
   display.setTextSize(2);          // text size
   display.setTextColor(WHITE);     // text color
   display.setCursor(30, 30);        // position to display
   display.println(Fuse); // text to display      // show on display
   display.setTextSize(1.75);          // text size
   display.setTextColor(WHITE);     // text color
   display.setCursor(0, 2);        // position to display
   display.println("FUSE VALUE:"); // text to display      // show on display
   curserPlacement();
  return 0;
 }
 int AutoReset(){
   if (AutoResetState == HIGH){
     if ((TimeElapsedState == 1) and (PreviousAutoState != TimeElapsedState)){
       RunCounter = 1;
     }
     PreviousAutoState = TimeElapsedState;
   }
   return 0;
 }
 int Automode(){
   int AutoModeBool = 0;
   int SpaceValue = 0;
   int x_val = 0;
   int position_x = 0;
   while (counter == 2){

     if ((digitalRead(digitalPin2) or digitalRead(digitalPin1)) == HIGH){
       x_val++;
     }

     if (((digitalRead(digitalPin3) or digitalRead(digitalPin4)) == HIGH) and (position_x % 2 ==0)){
       y_valRST++;
     }

     if ((digitalRead(digitalPin3) == HIGH) and (position_x % 2 == 1)){
       y_valDLY++;
     }

     if ((digitalRead(digitalPin4) == HIGH) and (position_x % 2 == 1)){
       y_valDLY--;
     }
    
     position_x = x_val / 9;
     if (position_x % 2 == 0){
       position_y_RST = y_valRST / 9;
     }
     if (position_x % 2 == 1){
       position_y_DLY = y_valDLY / 9;
     }
     ButtonStates();
     BatteryLife();
     if (position_x % 2 == 0){
       SpaceValue = 0;
     }
     if (position_x % 2 ==1){
       SpaceValue = 61;
     }

     if ((position_x % 2 == 0) and (position_y_RST % 2 == 0)){
       AutoModeBool = 0;
     }

     if ((position_x % 2 == 0) and (position_y_RST % 2 == 1)){
       AutoModeBool = 1;
     }

     if (position_x % 2 == 1){
       SecondsValue = position_y_DLY;
     }

     if (counter == 3){
       counter = 0;
     }
     display.setTextSize(2);          // text size    // text color 
     if (AutoModeBool == 1){
       AutoResetState = AutoModeBool;
       DifferenceValue = 0;

     }

     if (AutoModeBool == 0){
       AutoResetState = AutoModeBool;
       DifferenceValue = 0;
     }
         if (AutoResetState == 1){
       display.setCursor(19, 38);
       display.println("ON");

     }
         if (AutoResetState == 0){
       display.setCursor(14, 38);
       display.println("OFF");

     }
     if (SecondsValue > 9){
       display.setCursor(83, 38);
       display.println(SecondsValue);
     }
     if (SecondsValue < 10){
       display.setCursor(86, 38);
       display.println(SecondsValue);
     }

     if (y_valDLY > 277){
       y_valDLY = 47;
     }
     if (y_valDLY < 45){
       y_valDLY = 276;
     }
     display.setTextSize(1);          // text size    // text color
     display.setCursor(0, 0);        // position to display
     display.println("AUTO SETTINGS"); 
     display.setCursor(2, 20); 
     display.println("AUTORESET  RESETDELAY");
     display.setCursor(107, 37); 
     display.setTextSize(2);
     display.println("s");
     display.fillRect(63,16,2,60,WHITE);
     display.fillRect(0,16,128,1,WHITE);
     display.fillTriangle((26 + SpaceValue) ,30,(29 + SpaceValue),34,(32 + SpaceValue),30,WHITE);
     display.fillTriangle((26 + SpaceValue),60,(29 + SpaceValue),56,(32 + SpaceValue),60,WHITE);
     display.display();
     display.fillRect(0,0,128,64,BLACK);
   }
   display.clearDisplay();
   DisplayOption = 0;
   return 0;
 }

void setup() {
    Serial.begin(115200);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);analogSetAttenuation(ADC_11db);
  esp_task_wdt_deinit();
  disableTGWDT();
  dtostrf(FuseValue, 4, 2, Fuse); // Convert float to string with 4 digits, 2 decimal places
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  pinMode(digitalPin1, INPUT);
  pinMode(digitalPin2, INPUT);
  pinMode(digitalPin3, INPUT);
  pinMode(digitalPin4, INPUT);
  pinMode(buttonpin5, INPUT);
  pinMode(buttonpin6, INPUT);
  pinMode(digitalPin8,OUTPUT);
  pinMode(pwmPin, OUTPUT);
  pinMode(BeepPin, OUTPUT);
  ledcSetup(pwmChannel, pwmFrequency, pwmResolution);
  ledcAttachPin(pwmPin, pwmChannel);
  int dutyCycle = 120; // 50% of 255 for 8-bit resolution
  ledcWrite(pwmChannel, dutyCycle);
    display.clearDisplay();
    display.drawBitmap(0, 10, epd_bitmap_Flo2, 128, 58, WHITE);
    display.display();
    delay(3000);
    display.clearDisplay();

}

void loop() {
  // put your main code here, to run repeatedly:
  esp_task_wdt_reset();
  AmpMode();
  WattOption();
  overcurrent_counter++;
  if (overcurrent_counter == 10){
    overcurrent_counter = 0;

  }
  DifferenceValue = ((millis() - TimeSinceTrip));
  if ((RunCounter % 2) == 1){
    AvgFuseValue();
    float CurrentValue = AvgFuseValTotal;
  }
  if (((DifferenceValue / (SecondsValue * 1000)) % 1000) >= 1){// Allow This guy to aound 0 and not exactly 0
    TimeElapsedState = HIGH;
  }
  if (((DifferenceValue / (SecondsValue * 1000)) % 1000) < 1){ 
    TimeElapsedState = LOW;
  }
  ButtonStates();

  frame = (frame + 1) % FRAME_COUNT;
  
  DipslayFuseValue();
  BatteryLife();
  display.fillRect(0,30,100,14,BLACK);
  FlowTrip();
  DisplayAmp();
  display.drawLine(84, 17, 84, 80, BLACK);
  display.fillRect(87,14,80,2,BLACK);
  display.fillRect(0,16,127,1,WHITE);
  display.fillRect(89,16,1,60,WHITE);
  display.display();
  display.clearDisplay();
  GeneralCounter++;
}
