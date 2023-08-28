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
#define EEPROM_SIZE 4000 // how many bytes to use of the EEPROM space

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

int numbers = 0;

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
  .jpeg_quality = 13, // quality
  .fb_count = 2
  //.fb_location = CAMERA_FB_IN_PSRAM,
  //.grab_mode = CAMERA_GRAB_WHEN_EMPTY
};

// initialize camera with our configuration above
esp_err_t camera_init(){
  return esp_camera_init(&config);  
}

// we take bool
void TakePicture(bool video){
  camera_fb_t *fb = NULL;
  fb = esp_camera_fb_get();
  if(!fb){
    Serial.println("ERROR:Frame buffer could not be acquierded!"); 
    return; 
  }
  Serial.println("Success!, frame buffer acquired!");

  // get pictureNumber from EEPROM and set path for the photo
  //EEPROM.begin(EEPROM_SIZE);
  //pictureNumber = EEPROM.read(0) + 1;
  String path = "";
  
  File indexFile = SD_MMC.open("/index.txt", FILE_READ);
  if(!indexFile){
    Serial.println("Failed to open index file");
    return;
    }

    while (indexFile.available()) {
      pictureNumber = indexFile.read();
    }
    
  indexFile.close();
  Serial.println(String(pictureNumber));
  
  path = "/picture" + String(pictureNumber) +".jpg";
  
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
    //EEPROM.write(0, pictureNumber);
    //EEPROM.commit();

        File writeIndex = SD_MMC.open("/index.txt", FILE_WRITE);
     
    if (!writeIndex) {
      Serial.println("Opening file to write failed");
      return;
    }

    writeIndex.write(pictureNumber+1);
    writeIndex.close();
  }
  file.close();
  //EEPROM.write(0, pictureNumber);
  //EEPROM.commit();
  
  // return frame buffer back for reuse
  esp_camera_fb_return(fb);
}

void cameraSettings(){
    sensor_t * s = esp_camera_sensor_get();
    s->set_brightness(s, 0);     // -2 to 2
    s->set_contrast(s, 0);       // -2 to 2
    s->set_saturation(s, 0);     // -2 to 2
    s->set_special_effect(s, 0); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
    s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
    s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
    s->set_wb_mode(s, 0);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
    s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
    s->set_aec2(s, 0);           // 0 = disable , 1 = enable
    s->set_ae_level(s, 0);       // -2 to 2
    s->set_aec_value(s, 300);    // 0 to 1200
    s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
    s->set_agc_gain(s, 0);       // 0 to 30
    s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
    s->set_bpc(s, 0);            // 0 = disable , 1 = enable
    s->set_wpc(s, 1);            // 0 = disable , 1 = enable
    s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
    s->set_lenc(s, 1);           // 0 = disable , 1 = enable
    s->set_hmirror(s, 0);        // 0 = disable , 1 = enable
    s->set_vflip(s, 0);          // 0 = disable , 1 = enable
    s->set_dcw(s, 1);            // 0 = disable , 1 = enable
    s->set_colorbar(s, 0);       // 0 = disable , 1 = enable
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

  cameraSettings();
  
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
  
  //digitalWrite(ledPin, HIGH);
  TakePicture(false);
  delay(2000);

  //esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);
  //Serial.println("Entering sleep mode");
  //delay(1000); // 1sec
  //digitalWrite(ledPin, LOW);
  //esp_deep_sleep_start();
  
  // Wi-Fi connection
  //WiFi.begin(ssid, password);
  //while (WiFi.status() != WL_CONNECTED) {
  //  delay(500);
  //  Serial.print(".");
  //}
  //Serial.println("");
  //Serial.println("WiFi connected");
  
  //Serial.print("Camera Stream Ready! Go to: http://");
  //Serial.print(WiFi.localIP());
  
  // Start streaming web server
  //startCameraServer();
}

void loop() {
  delay(250); 
  //numbers++;
  //Serial.println(String(numbers));
  TakePicture(true);
}
