#include "Arduino.h" // general funcionality
#include "esp_camera.h" // camera thingy https://github.com/espressif/esp32-camera
#include "esp_timer.h"
#include "FS.h" // file system
//brownout thing
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include "FS.h" // SD Card ESP32
#include "SD_MMC.h" // SD Card ESP32
#include <EEPROM.h> // read and write from flash memory


// define the number of bytes you want to access
#define EEPROM_SIZE 1

// Photo File Name to save in SPIFFS
#define FILE_PHOTO "/photo.jpg"

#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

/*
Notes:
pin 0 = button
pin 4 = front flashlight LED
pin 33 = back red LED
*/

// pin numbers
const int buttonPin = 0;  // pin number of the button
const int ledPin =  4;    // pin number of the Flashlight LED

int buttonState = 0;

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
  .pixel_format = PIXFORMAT_JPEG, 
  .frame_size = FRAMESIZE_VGA,
  .jpeg_quality = 30,
  .fb_count = 2
  //.fb_location = CAMERA_FB_IN_PSRAM,
  //.grab_mode = CAMERA_GRAB_WHEN_EMPTY
};

esp_err_t camera_init(){
  return esp_camera_init(&config);  
}

void TakePicture(){
  camera_fb_t *fb = NULL;
  fb = esp_camera_fb_get();
  if(!fb){
    Serial.println("Frame buffer could not be acquierded!"); 
    return; 
  }
  Serial.println("Success");

  EEPROM.begin(EEPROM_SIZE);
  photoCount = EEPROM.read(0) + 1;
  
  String path = "/picture/myEpicPhoto.jpg";
  fs::FS &fs = SD_MMC;
  File file = fs.open(path.c_str(), FILE_WRITE);
  if(!file){
    Serial.println("Ooopsie, failed to open file in writing mode");  
  }

  file.write(fb->buf, fb->len);
  EEPROM.write(0, photoCount);
    EEPROM.commit();
  file.close();
  
  esp_camera_fb_return(fb);
}

void setup() {
  Serial.begin(115200);  
  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); ////// TOCHECK!
  esp_err_t camera;
  camera = camera_init();

  if(camera != ESP_OK){
    Serial.println("Camera init failed");
    return;
  }
  
  Serial.println("Reset");
  // initialize the pushbutton pin as an input
  /// pinMode(buttonPin, INPUT);
  // initialize the LED pin as an output
  /// pinMode(ledPin, OUTPUT);

  TakePicture();
}

void loop() {
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






  
