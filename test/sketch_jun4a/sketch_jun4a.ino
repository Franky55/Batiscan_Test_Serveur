/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-cam-video-streaming-web-server-camera-home-assistant/
  
  IMPORTANT!!! 
   - Select Board "AI Thinker ESP32-CAM"
   - GPIO 0 must be connected to GND to upload a sketch
   - After connecting GPIO 0 to GND, press the ESP32-CAM on-board RESET button to put your board in flashing mode
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include "esp_camera.h"
#include <WiFi.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "soc/soc.h" //disable brownout problems
#include "soc/rtc_cntl_reg.h"  //disable brownout problems
#include "esp_http_server.h"
#include "ESP32SPISlave.h"



#define PART_BOUNDARY "123456789000000000000987654321"

const char* ssid = "SM-G930W87541";
const char* password = "franky66";

uint8_t image_buffer[160*120*2];

static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

#define SPI_MISO 12
#define SPI_MOSI 13
#define SPI_CLK 14
#define INTERFACE_SPI_CS1 15

ESP32SPISlave slave;


#define SPI_BUFFER_SIZE 64

unsigned char RAW_RX_buf[SPI_BUFFER_SIZE] {0};
unsigned char RAW_TX_buf[SPI_BUFFER_SIZE] = {'A','B','C','D','E','F','G','H','I','J',
'A','B','C','D','E','F','G','H','I','J',
'A','B','C','D','E','F','G','H','I','J',
'A','B','C','D','E','F','G','H','I','J',
'A','B','C','D','E','F','G','H','I','J',
'A','B','C','D','E','F','G','H','I','J',
'A','B','C','D'
};

void interface_SPI_ReadData();
void interface_SPI_Queue_Transaction();

httpd_handle_t stream_httpd = NULL;


void send_image_buffer(uint8_t* buffer, httpd_req_t *req) {
  
  // uint8_t* jpg_buf = NULL;
  // size_t jpg_buf_len = 0;
  // char * part_buf[64];

  // fb = esp_camera_fb_get();
  // if(!fb) {
  //   Serial.println("Camera capture failed");
  //   return;
  // }
  // size_t buffer_len = fb->len; // Assuming 16-bit color depth (RGB565 format)

  // //bool jpeg_converted = fmt2jpg(buffer, buffer_len, 96, 96, PIXFORMAT_GRAYSCALE, 80, &jpg_buf, &jpg_buf_len);
  // bool jpeg_converted = fmt2jpg(buffer, fb->len, fb->width, fb->height, fb->format, 80, &jpg_buf, &jpg_buf_len);
  // if (!jpeg_converted) {
  //   Serial.println("JPEG conversion failed");
  //   return;
  // }
  
    
  // size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, jpg_buf_len);
  // httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
  // httpd_resp_send_chunk(req, (const char*)jpg_buf, jpg_buf_len);
  // httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));

  // esp_camera_fb_return(fb);
  // fb = NULL;
  // jpg_buf = NULL;
  // free(jpg_buf);
  // jpg_buf = NULL;

}



static esp_err_t stream_handler(httpd_req_t *req){
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t * _jpg_buf = NULL;
  char * part_buf[64];

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if(res != ESP_OK){
    return res;
  }

  while(true){
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
    } else {
      if(fb->width > 95){
        
        Serial.print("Size: ");
        Serial.println(fb->len);
        for(int i = 0; i < fb->len; i++)
        {
          image_buffer[i] = fb->buf[i];
          Serial.print(image_buffer[i]);
          Serial.print(", ");
        }
        Serial.println("\n");

        if(fb->format != PIXFORMAT_JPEG){
          //bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
          Serial.println("Convert JPEG");
          bool jpeg_converted = fmt2jpg(image_buffer, fb->len, fb->width, fb->height, fb->format, 80, &_jpg_buf, &_jpg_buf_len);
          esp_camera_fb_return(fb);
          fb = NULL;
          if(!jpeg_converted){
            Serial.println("JPEG compression failed");
            res = ESP_FAIL;
          }
        } else {
          Serial.println("Skip");
          _jpg_buf_len = fb->len;
          _jpg_buf = fb->buf;
        }
      }
    }
    if(res == ESP_OK){
      
      size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if(res == ESP_OK){
      
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
    }
    if(res == ESP_OK){
      
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }
    if(fb){
      
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else if(_jpg_buf){
      free(_jpg_buf);
      _jpg_buf = NULL;
    }
    if(res != ESP_OK){
      break;
    }
    //Serial.printf("MJPG: %uB\n",(uint32_t)(_jpg_buf_len));
  }
  return res;
}







void startCameraServer(){
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  //config.max_resp_headers = 5;
  config.server_port = 80;

  httpd_uri_t index_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = stream_handler,
    .user_ctx  = NULL
    
  };
  
  //Serial.printf("Starting web server on port: '%d'\n", config.server_port);
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &index_uri);
  }
}


void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
 
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  //config.pixel_format = PIXFORMAT_GRAYSCALE;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QQVGA;
  config.jpeg_quality = 10;
  config.fb_count = 2;
  
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }


  // fb = esp_camera_fb_get();
  // if(!fb) {
  //   Serial.println("Camera capture failed");
  //   return;
  // }

  // Serial.println("Camera capture succes: ");
  // Serial.print("fb->len: ");
  // Serial.println(fb->len);
  // Serial.print("fb->width: ");
  // Serial.println(fb->width);
  // Serial.print("fb->height: ");
  // Serial.println(fb->height);
  // Serial.print("fb->format: ");
  // Serial.println(fb->format);

  // delay(3000);
  // for(int i = 0; i < fb->len; i++)
  // {
  //   image_buffer[i] = fb->buf[i];
  //   Serial.print(fb->buf[i], DEC);
  //   Serial.print(", ");
  // }

  // Serial.println("");
  // Serial.println("Finished");


  

// Wi-Fi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  Serial.print("Camera Stream Ready! Go to: http://");
  Serial.print(WiFi.localIP());
  
  // Start streaming web server
  startCameraServer();

  //esp_camera_fb_return(fb);

  // slave.setDataMode(SPI_MODE0);
  // slave.begin(SPI2_HOST, SPI_CLK, SPI_MISO, SPI_MOSI, INTERFACE_SPI_CS1);
  // slave.setQueueSize(1);
  // slave.queue(RAW_RX_buf, RAW_TX_buf, SPI_BUFFER_SIZE);
  
}

void loop() {
  delay(1);
  // if(slave.available())
  // {
  //   interface_SPI_ReadData();
  // }
}


void interface_SPI_ReadData()
{
    
    int size = (unsigned char)slave.size();

    
    // Serial.print("Data Received ");
    // Serial.print(size);
    // Serial.print(" : ");

    for(int i = 0; i < size; i++)
    {
        //Serial.print((char)RAW_RX_buf[i]);
        //interface_SPI_Struct.spi_slave_rx_buf[i] = RAW_RX_buf[i];
    }
    //Serial.println("");
    memset(RAW_RX_buf, 0, SPI_BUFFER_SIZE);

    // you should pop the received packet
    slave.pop();

    while(slave.available())
    {
        slave.pop();
    }

    interface_SPI_Queue_Transaction();
}


void interface_SPI_Queue_Transaction()
{
  slave.end();
    slave.setDataMode(SPI_MODE0);
    slave.begin(SPI2_HOST, SPI_CLK, SPI_MISO, SPI_MOSI, INTERFACE_SPI_CS1);
    slave.setQueueSize(1);

    slave.queue(RAW_RX_buf, RAW_TX_buf, SPI_BUFFER_SIZE);
}
