#include "FastLED.h"
#include <WebSocketsServer.h>

WebSocketsServer webSocket = WebSocketsServer(81);

const int NUM_LEDS = 300;
const int PIN = 1;
int TOP_INDEX = int(NUM_LEDS / 2);
int EVENODD = NUM_LEDS % 2;
int BOTTOM_INDEX = 0;
int idex = 0;  
int bouncedirection = 0;
int isat = 0; 
int ival = 0;
byte counter = 0;
uint16_t newmode = 0;


uint16_t step = 1;
uint16_t brightness = 100;
uint16_t thishue = 149;
uint16_t thissat = 255;
uint16_t thisval = 255;
uint16_t thisdelay = 10;
uint16_t mode = 0;



CRGB leds[NUM_LEDS];

void setup_led(){
  FastLED.addLeds<WS2812B, PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(brightness);
}




void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  if(type == WStype_TEXT){
    if(payload[0] == 'b' && payload[1] == 'r'){
      brightness = (uint16_t) strtol((const char *) &payload[2], NULL, 10);
      Serial.print("brightness= ");
      Serial.println(brightness);
    }
    else if (payload[0] == 'm' && payload[1] == 'd')
    {
      mode = (uint16_t) strtol((const char *) &payload[2], NULL, 10);
      Serial.print("mode= ");
      Serial.println(mode);
    }
    else if (payload[0] == 's' && payload[1] == 'p')
    {
      uint16_t speed;
      speed = (uint16_t) strtol((const char *) &payload[2], NULL, 10);
      thisdelay = 41 - speed;
      Serial.print("thisdelay= ");
      Serial.println(thisdelay);
    }
    else if (payload[0] == 'h' && payload[1] == 'h')
    {
      thishue = (uint16_t) strtol((const char *) &payload[2], NULL, 10);
      Serial.print("thishue= ");
      Serial.println(thishue);
    }
    else if (payload[0] == 's' && payload[1] == 's')
    {
      thissat = (uint16_t) strtol((const char *) &payload[2], NULL, 10);
      Serial.print("thissat= ");
      Serial.println(thissat);
    }
    else if (payload[0] == 'v' && payload[1] == 'v')
    {
      thisval = (uint16_t) strtol((const char *) &payload[2], NULL, 10);
      Serial.print("thisval= ");
      Serial.println(thisval);
    }
    else
    {
      for(int i = 0; i < length; i++)
        Serial.print((char) payload[i]);
        Serial.println();
    }
  }
}

void webSocketStart(){
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

int horizontal_index(int i) {
  //-ONLY WORKS WITH INDEX < TOPINDEX
  if (i == BOTTOM_INDEX) {
    return BOTTOM_INDEX;
  }
  if (i == TOP_INDEX && EVENODD == 1) {
    return TOP_INDEX + 1;
  }
  if (i == TOP_INDEX && EVENODD == 0) {
    return TOP_INDEX;
  }
  return NUM_LEDS - i;
}


int adjacent_cw(int i) {
  int r;
  if (i < NUM_LEDS - 1) {
    r = i + 1;
  }
  else {
    r = 0;
  }
  return r;
}

//---FIND ADJACENT INDEX COUNTER-CLOCKWISE
int adjacent_ccw(int i) {
  int r;
  if (i > 0) {
    r = i - 1;
  }
  else {
    r = NUM_LEDS - 1;
  }
  return r;
}

/*----------------------------------------------
  ------------------Effects---------------------*/

void one_color_all(int h, int s, int v) {       //-SET ALL LEDS TO ONE COLOR
  for (int i = 0 ; i < NUM_LEDS; i++ ) {
    leds[i].setHSV( h, s, v);
  }
}

void pulse_one_color_all() {           //-m11-PULSE val ON ALL LEDS TO ONE COLOR
  if (bouncedirection == 0) {
    ival+=2;
    if (ival >= 255) {
      bouncedirection = 1;
    }
  }
  if (bouncedirection == 1) {
    ival = ival - 2;
    if (ival <= 1) {
      bouncedirection = 0;
    }
  }
  for (int idex = 0 ; idex < NUM_LEDS; idex++ ) {
    leds[idex] = CHSV(thishue, thissat, ival);
  }
  FastLED.show();
  FastLED.delay(thisdelay);
}


void color_bounce() {                        //-m5-BOUNCE COLOR (SINGLE LED)
  if (bouncedirection == 0) {
    idex = idex + 1;
    if (idex == NUM_LEDS) {
      bouncedirection = 1;
      idex = idex - 1;
    }
  }
  if (bouncedirection == 1) {
    idex = idex - 1;
    if (idex == 0) {
      bouncedirection = 0;
    }
  }
  for (int i = 0; i < NUM_LEDS; i++ ) {
    if (i == idex) {
      leds[i] = CHSV(thishue, thissat, 255);
    }
    else {
      leds[i] = CHSV(0, 0, 0);
    }
  }
  FastLED.show();
  FastLED.delay(thisdelay);
}


void rainbow(){
    for (int i = 0; i < NUM_LEDS; i++ ) {         // от 0 до первой трети
        leds[i] = CHSV(counter + i * step, 255, 255);  // HSV. Увеличивать HUE (цвет)// умножение i уменьшает шаг радуги
    }
    counter++;        // counter меняется от 0 до 255 (тип данных byte)
    FastLED.show();
    FastLED.delay(thisdelay);         // скорость движения радуги
}

void color_bounceFADE() {      
  if (bouncedirection == 0) {
    idex = idex + 1;
    if (idex == NUM_LEDS) {
      bouncedirection = 1;
      idex = idex - 1;
    }
  }
  if (bouncedirection == 1) {
    idex = idex - 1;
    if (idex == 0) {
      bouncedirection = 0;
    }
  }
  int iL1 = adjacent_cw(idex);
  int iL2 = adjacent_cw(iL1);
  int iL3 = adjacent_cw(iL2);
  int iR1 = adjacent_ccw(idex);
  int iR2 = adjacent_ccw(iR1);
  int iR3 = adjacent_ccw(iR2);
  for (int i = 0; i < NUM_LEDS; i++ ) {
    if (i == idex) {
      leds[i] = CHSV(thishue, thissat, 255);
    }
    else if (i == iL1) {
      leds[i] = CHSV(thishue, thissat, 150);
    }
    else if (i == iL2) {
      leds[i] = CHSV(thishue, thissat, 80);
    }
    else if (i == iL3) {
      leds[i] = CHSV(thishue, thissat, 20);
    }
    else if (i == iR1) {
      leds[i] = CHSV(thishue, thissat, 150);
    }
    else if (i == iR2) {
      leds[i] = CHSV(thishue, thissat, 80);
    }
    else if (i == iR3) {
      leds[i] = CHSV(thishue, thissat, 20);
    }
    else {
      leds[i] = CHSV(0, 0, 0);
    }
  }
  FastLED.show();
  FastLED.delay(thisdelay);
}

void change_mode(){
  switch (mode) {
    case 999: break;                           // пазуа
    case 0: rainbow(); break;
    case 1: color_bounce(); break;
    case 2: color_bounceFADE(); break;
    case 3: one_color_all(thishue, thissat, thisval); FastLED.show(); break;
    case 4: pulse_one_color_all(); break;
  }
  if (mode != newmode){
    one_color_all(0, 0, 0);
    ival = 0;
    isat = 0;
    idex = 0;
    bouncedirection = 0;
    newmode = mode;
  }
}

void led_mainloop(){
  FastLED.setBrightness(brightness);
  change_mode();
}