#include "Arduino.h"
#include "main.h"
#include "interface_SPI_Master.h"
#include "serviceBaseDeTemps.h"
#include "service_Protocole_SPI.h"
#include <stdio.h>
#include "processus_Serveur.h"
#include "processus_Communication.h"


#include "esp_http_server.h"
#include "img_converters.h"
#include <WiFi.h>

#include "esp_camera.h"
#include "img_converters.h"
#include "fb_gfx.h"
#include "soc/soc.h" //disable brownout problems
#include "soc/rtc_cntl_reg.h"  //disable brownout problems


#define PART_BOUNDARY "123456789000000000000987654321"

void startCameraServer();

const char* ssid = "SM-G930W87541";
const char* password = "franky66";

static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t stream_httpd = NULL;

PROCESSUS_SERVEUR Processus_Serveur_Struct;


int Processus_Serveur_initialise(void)
{
    // Wi-Fi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  delay(3000);
  Serial.println("");
  Serial.println("WiFi connected");
  
  Serial.print("Camera Stream Ready! Go to: http://");
  Serial.print(WiFi.localIP());
  
  // Start streaming web server
  startCameraServer();

  return 0;
}

static esp_err_t stream_handler(httpd_req_t *req){
  
  esp_err_t res = ESP_OK;
  char * part_buf[64];

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if(res != ESP_OK){
    return res;
  }

  while(true){
    

        
    if(res == ESP_OK){
      
      size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, Processus_Serveur_Struct.Buffersize);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if(res == ESP_OK){
      
      res = httpd_resp_send_chunk(req, (const char *)Processus_Serveur_Struct.CameraBuffer, Processus_Serveur_Struct.Buffersize);
    }
    if(res == ESP_OK){
      
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
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