#include <ESP8266FtpServer.h>             // Библиотека для работы с SPIFFS по FTP
#include <WebServer/WebServer.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <LED_effects/Led_effects.h>



FtpServer ftpSrv;                         // Создаём объект для работы с FTP

extern const char* ssid ;            // Указываем имя WiFi-сети, к которой будет подключаться ESP8266 для доступа в Интернет
extern const char* password;        // Указываем пароль для подключения к WiFi-сети

extern WebSocketsServer webSocket;



void start_system() {
  Serial.begin(9600);
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready OTA");
  ftpSrv.begin("login","password");       // Инициализируем FTP-сервер (на 21-й порт)
  startWebServer();
  webSocketStart();
  SPIFFS.begin();                         // Инициализируем работу с файловой системой
}



void setup() {
  start_system();
  setup_led();
}



void loop() {
  
  led_mainloop();
  ArduinoOTA.handle();
  handleClient();
  webSocket.loop();
  ftpSrv.handleFTP();   
  /*if (WiFi.status() == WL_CONNECTED){
    
  }                  // Отслеживаем обращения по FTP 
*/
}
