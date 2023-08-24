/*
Notes:
  Pins:
  pin 0 = button
  pin 4 = front flashlight LED
  pin 33 = back red LED
  SD card:
  must be formatted to FAT32 
  uses up to 4GB of space 
  tho larger cards can be used but only 4GB will be used
*/

#include "Arduino.h" // general funcionality
#include "esp_camera.h" // camera thingy https://github.com/espressif/esp32-camera
#include "FS.h" // file system
#include "SD_MMC.h" // SD Card for ESP32

#include <EEPROM.h> // read and write from flash memory on the ESP32 https://en.wikipedia.org/wiki/EEPROM
#define EEPROM_SIZE 1 // how many bytes to use of the EEPROM space

// pinout definition for configuration
#define CAMERA_MODEL_AI_THINKER // camera model
#include "camera_pins.h" // pin definitions here

// disable brownout detector thingy (errors when low power or voltage or something like that)
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// pin numbers
const int buttonPin = 0;  // pin number of the button
const int ledPin =  4;    // pin number of the Flashlight LED

// variables
int buttonState = 0;
int pictureNumber = 0; // saved in EEPROM

// camera config duhh
static camera_config_t config = {
  .pin_pwdn = PWDN_GPIO_NUM,
  .pin_reset = RESET_GPIO_NUM,
  .pin_xclk = XCLK_GPIO_NUM,
  .pin_sccb_sda = SIOD_GPIO_NUM,
  .pin_sccb_scl = SIOC_GPIO_NUM,
  .pin_d7 = Y9_GPIO_NUM,
  .pin_d6 = Y8_GPIO_NUM,
  .pin_d5 = Y7_GPIO_NUM,
  .pin_d4 = Y6_GPIO_NUM,
  .pin_d3 = Y5_GPIO_NUM,
  .pin_d2 = Y4_GPIO_NUM,
  .pin_d1 = Y3_GPIO_NUM,
  .pin_d0 = Y2_GPIO_NUM,
  .pin_vsync = VSYNC_GPIO_NUM,
  .pin_href = HREF_GPIO_NUM,
  .pin_pclk = PCLK_GPIO_NUM,
  .xclk_freq_hz = 20000000,
  .ledc_timer = LEDC_TIMER_0,
  .ledc_channel = LEDC_CHANNEL_0,
  .pixel_format = PIXFORMAT_JPEG, // format
  .frame_size = FRAMESIZE_UXGA, // resolution
  .jpeg_quality = 12, // quality
  .fb_count = 2
  //.fb_location = CAMERA_FB_IN_PSRAM,
  //.grab_mode = CAMERA_GRAB_WHEN_EMPTY
};

// initialize camera with our configuration above
esp_err_t camera_init(){
  return esp_camera_init(&config);  
}

void TakePicture(){
  camera_fb_t *fb = NULL;
  fb = esp_camera_fb_get();
  if(!fb){
    Serial.println("ERROR:Frame buffer could not be acquierded!"); 
    return; 
  }
  Serial.println("Success!, frame buffer acquired!");

  // get pictureNumber from EEPROM and set path for the photo
  EEPROM.begin(EEPROM_SIZE);
  pictureNumber = EEPROM.read(0) + 1;
  String path = "/picture" + String(pictureNumber) +".jpg";

  // save picture to microSD card
  fs::FS &fs = SD_MMC; 
  File file = fs.open(path.c_str(), FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file in writing mode");
  } 
  else {
    // write frame buffer to the file
    file.write(fb->buf, fb->len); // framebuffer, framebuffer length
    Serial.printf("Saved file to path: %s\n", path.c_str());
    EEPROM.write(0, pictureNumber);
    EEPROM.commit();
  }
  file.close();
  EEPROM.write(0, pictureNumber);
  EEPROM.commit();
  
  // return frame buffer back for reuse
  esp_camera_fb_return(fb);
}

void setup() {
  // disable brownout detector
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
  
  Serial.begin(115200); 
  Serial.println("Reset"); 

  // initialize camera
  esp_err_t camera;
  camera = camera_init();
  if(camera != ESP_OK){
    Serial.println("Camera init failed");
    return;
  }

  // mount SD card
  if(!SD_MMC.begin()){
    Serial.println("SD Card Mount Failed");
    return;
  }

  // check if SD card inserted
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD Card attached");
    return;
  }

  // BUTTON TODO
  // initialize the pushbutton pin as an input
  /// pinMode(buttonPin, INPUT);
  // initialize the LED pin as an output
  /// pinMode(ledPin, OUTPUT);

  TakePicture();
}

void loop() {
  // TODO
  
  // read the state of the pushbutton value
  ///buttonState = digitalRead(buttonPin);
  //Serial.println(buttonState);
  
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH
  ///if (buttonState == HIGH) {
    // turn LED off
    ///digitalWrite(ledPin, LOW);
  ///} else {
    // turn LED on
    ///digitalWrite(ledPin, HIGH);
  ///}
}






  
