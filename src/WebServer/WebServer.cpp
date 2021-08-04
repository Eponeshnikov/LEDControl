#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>                  // Библиотека для создания Wi-Fi подключения (клиент или точка доступа)
#include <FS.h>                           // Библиотека для работы с файловой системой
#include <CONSTANTS.h>

char* ssid = SSIDConsts::ssid;            // Указываем имя WiFi-сети, к которой будет подключаться ESP8266 для доступа в Интернет
char* password = SSIDConsts::password;        // Указываем пароль для подключения к WiFi-сети
char* mySsid = MySSIDConsts::mySsid;


IPAddress ip(192,168,10,4);
IPAddress gateway(192,168,10,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server;

String getContentType(String filename){                                 // Функция, возвращающая необходимый заголовок типа содержимого в зависимости от расширения файла
  if (filename.endsWith(".html")) return "text/html";                   // Если файл заканчивается на ".html", то возвращаем заголовок "text/html" и завершаем выполнение функции
  else if (filename.endsWith(".css")) return "text/css";                // Если файл заканчивается на ".css", то возвращаем заголовок "text/css" и завершаем выполнение функции
  else if (filename.endsWith(".js")) return "application/javascript";   // Если файл заканчивается на ".js", то возвращаем заголовок "application/javascript" и завершаем выполнение функции
  else if (filename.endsWith(".png")) return "image/png";               // Если файл заканчивается на ".png", то возвращаем заголовок "image/png" и завершаем выполнение функции
  else if (filename.endsWith(".jpg")) return "image/jpg";               // Если файл заканчивается на ".jpg", то возвращаем заголовок "image/jpg" и завершаем выполнение функции
  else if (filename.endsWith(".gif")) return "image/gif";               // Если файл заканчивается на ".gif", то возвращаем заголовок "image/gif" и завершаем выполнение функции
  else if (filename.endsWith(".ico")) return "image/x-icon";            // Если файл заканчивается на ".ico", то возвращаем заголовок "image/x-icon" и завершаем выполнение функции
  return "text/plain";                                                  // Если ни один из типов файла не совпал, то считаем что содержимое файла текстовое, отдаем соответствующий заголовок и завершаем выполнение функции
}

bool handleFileRead(String path){                                       // Функция работы с файловой системой
  if(path.endsWith("/")) path += "index.html";                          // Если устройство вызывается по корневому адресу, то должен вызываться файл index.html (добавляем его в конец адреса)
  String contentType = getContentType(path);                            // С помощью функции getContentType (описана ниже) определяем по типу файла (в адресе обращения) какой заголовок необходимо возвращать по его вызову
  if(SPIFFS.exists(path)){                                              // Если в файловой системе существует файл по адресу обращения
    File file = SPIFFS.open(path, "r");                                 //  Открываем файл для чтения
    size_t sent = server.streamFile(file, contentType);                   //  Выводим содержимое файла по HTTP, указывая заголовок типа содержимого contentType
    file.close();                                                       //  Закрываем файл
    return true;                                                        //  Завершаем выполнение функции, возвращая результатом ее исполнения true (истина)
  }
  return false;                                                         // Завершаем выполнение функции, возвращая результатом ее исполнения false (если не обработалось предыдущее условие)
}


void startWebServer() {
    system_update_cpu_freq(SYS_CPU_160MHZ);
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(ssid, password);             // Соединяемся с WiFi-сетью
    while (WiFi.status() != WL_CONNECTED)   // Пока соединение не установлено
      delay(500);                           //  делаем задержку в пол секунды, пока соединение не установится

    Serial.print("\n\nLocal IP: ");         // Выводим на монитор серийного порта сообщение о том, что сейчас будем выводить локальный IP
    Serial.println(WiFi.localIP());         // Выводим локальный IP-адрес ESP8266


    server.onNotFound([](){                                                 // Описываем действия при событии "Не найдено"
    if(!handleFileRead(server.uri()))                                       // Если функция handleFileRead (описана ниже) возвращает значение false в ответ на поиск файла в файловой системе
        server.send(404, "text/plain", "Not Found");                        // возвращаем на запрос текстовое сообщение "File isn't found" с кодом 404 (не найдено)
    });

    server.begin();

    WiFi.softAPConfig(ip, gateway, subnet);
    WiFi.softAP(mySsid, password);
}

void handleClient() {
    //Serial.println(WiFi.RSSI());
    server.handleClient();
}