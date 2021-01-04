#include <M5Stack.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

//
// Wifi
//
WiFiClientSecure client;

//
// Red WiFi
//
const char* ssid       = "SSID";   // WiFi SSID
const char* password   = "password";         // WiFi PW

//
// Variables para ciclos
//
unsigned long getDataTimer = 0;
unsigned long DataTimer = 5 * 1000;
int pantalla = 1;
int nSensores = 3;
int val = 0;
double temp = 0.0;

//
// Define meter size as 1 for M5.Lcd.rotation(0) or 1.3333 for M5.Lcd.rotation(1)
//
#define M_SIZE 1.3333
#define TFT_GREY 0x5AEB

float ltx = 0;                                // Saved x coord of bottom of needle
uint16_t osx = M_SIZE*120, osy = M_SIZE*120;  // Saved x & y coords
uint32_t updateTime = 0;                      // time for next update
int old_analog =  -999;                       // Value last displayed

//
// Peticiones WEB
//
HTTPClient http;

//
// Configuración
//
void setup() {
  
  M5.begin();
  M5.Power.begin();
  // Serial.begin(57600); // For debug
  M5.Lcd.fillScreen(TFT_BLACK);

  if (wifiConnection()){
    analogMeter(); 
    cuadrosMeter();
    M5.Lcd.drawCentreString("(C) ATDFiWare 2021", 155, 180, 4); // Units at bottom right  
  }

}

//
// Main program
//
void loop() {

  M5.update();

  // Testigo Cambio de Sensor con Botón A
  if(M5.BtnA.wasPressed()) {
    // M5.Power.powerOFF();
    pantalla = pantalla + 1;
    if (pantalla > nSensores){
      pantalla = 1;
    }
  }

  // Apagado con Boton C
  if(M5.BtnC.wasPressed()) {
    M5.Power.powerOFF();
  }
  
  // Get valores ATDFiWare Sensors OPEN-DATA por cada DateTimer
  if (millis() - getDataTimer >= DataTimer){

    // am102
    if (pantalla == 1){
        String consulta = GetValue("https://atd.innolivar.es", "/opendata/am100");
        String am102_co2 = deserializa(consulta, "co2", pantalla);
        String am102_temp = deserializa(consulta, "temperature", pantalla);
        String am102_time = deserializa(consulta, "TimeInstant", pantalla);
        val = map(am102_co2.toInt(), 0, 1300, 0, 100);
        plotNeedle(val, 0, am102_co2.toInt(),"AM102",am102_temp.toFloat(),am102_time);
        // delay(5000);
    }

    // em500
    if (pantalla == 2){
      String consulta = GetValue("https://atd.innolivar.es", "/opendata/em500");
      String em500_co2 = deserializa(consulta, "co2", pantalla);
      String em500_temp = deserializa(consulta, "temperature", pantalla);
      String em500_time = deserializa(consulta, "TimeInstant", pantalla);
      val = map(em500_co2.toInt(), 0, 1300, 0, 100);
      plotNeedle(val, 0, em500_co2.toInt(), "EM500",em500_temp.toFloat(),em500_time);
      // delay(5000);
    }

    // mhz19
    if (pantalla == 3){
      String consulta = GetValue("https://atd.innolivar.es", "/opendata/mhz19");
      String mhz19_co2 = deserializa(consulta, "ppm", pantalla);
      String mhz19_temp = deserializa(consulta, "temp", pantalla);
      String mhz19_time = deserializa(consulta, "TimeInstant", pantalla);
      val = map(mhz19_co2.toInt(), 0, 1300, 0, 100);
      plotNeedle(val, 0, mhz19_co2.toInt(), "MHZ19",mhz19_temp.toFloat(),mhz19_time);
      //delay(5000); 
    }   

    getDataTimer = millis();
    
  }
  
}

//
// Cuadro inferior Clear.
//
void cuadrosMeter(){
    // Meter outline
    M5.Lcd.drawRoundRect(5, 163, M_SIZE*230, 60, 2, WHITE);
}

//
// Limpiar Pantalla
//
void clearScreen() {
  M5.Lcd.clear(BLACK);
  M5.Lcd.setCursor(0,0);
}

//
// Deserializar JSON petición WEB
//
String deserializa(String respuesta, String identificador, int pantalla){

      //
      // capacity de https://arduinojson.org/v6/assistant/
      //
      size_t capacity = JSON_ARRAY_SIZE(3) +
                              2 * JSON_OBJECT_SIZE(1) +
                              JSON_OBJECT_SIZE(2) +
                              3 * JSON_OBJECT_SIZE(4) +
                              JSON_OBJECT_SIZE(5) +
                              JSON_OBJECT_SIZE(6) +
                              JSON_OBJECT_SIZE(12) +
                              304;  

      //
      // modify object capacity depends on sensor from https://arduinojson.org/v6/assistant/
      //
      if (pantalla == 1){
        // AM102
        capacity = 2048;
      }
      if (pantalla == 2){
        // EM500
        capacity = 1536;
      }
      if (pantalla == 3){
        // MHZ19
        capacity = 1024;
      }

      DynamicJsonDocument doc(capacity);
 
      // Parsear objeto JSON
      DeserializationError error = deserializeJson(doc, respuesta);
      if (error) {
        // Si hay error no se continua
          Serial.print("Fallo al parsear JSON. Error: ");
          Serial.println(error.c_str());
      } 

      // Return value of identificador
      return doc[0][identificador]["value"];
  
}

//
// Conexión WiFi
//
boolean wifiConnection() {
  WiFi.begin(ssid,password);
  int count = 0;
  M5.Lcd.print("Waiting Wi-Fi connection");
  while ( count < 20 ) {
    if (WiFi.status() == WL_CONNECTED) {
      M5.Lcd.println();
      M5.Lcd.println("Connected!");
      return (true);
    }
    delay(500);
    M5.Lcd.print(".");
    count++;
  }
  M5.Lcd.println("Timed out.");
  return false;
}

//
// Función petición WEB obtención datos.
//
String GetValue(String host, String url){

  http.begin(host+url);
  int codigoHttp = http.GET();

  Serial.print("Http Code: ");
  Serial.println(codigoHttp);

  if (codigoHttp > 0){
    // Response OK
    if (codigoHttp == HTTP_CODE_OK){
       return http.getString();        
    }
  } else {
    Serial.println("Error al recibir petición");
  }
  
}

// 
// Draw the analogue meter on the screen. Modified by J(C)heca from Examples Arduino IDE for M5Stack
// 
void analogMeter()
{

  // Meter outline
  M5.Lcd.fillRect(0, 0, M_SIZE*239, M_SIZE*126+60, TFT_GREY);
  M5.Lcd.fillRect(5, 3, M_SIZE*230, M_SIZE*119, TFT_WHITE);

  M5.Lcd.setTextColor(TFT_BLACK);  // Text colour

  // Draw ticks every 5 degrees from -50 to +50 degrees (100 deg. FSD swing)
  for (int i = -50; i < 51; i += 5) {
    // Long scale tick length
    int tl = 15;

    // Coodinates of tick to draw
    float sx = cos((i - 90) * 0.0174532925);
    float sy = sin((i - 90) * 0.0174532925);
    uint16_t x0 = sx * (M_SIZE*100 + tl) + M_SIZE*120;
    uint16_t y0 = sy * (M_SIZE*100 + tl) + M_SIZE*140;
    uint16_t x1 = sx * M_SIZE*100 + M_SIZE*120;
    uint16_t y1 = sy * M_SIZE*100 + M_SIZE*140;

    // Coordinates of next tick for zone fill
    float sx2 = cos((i + 5 - 90) * 0.0174532925);
    float sy2 = sin((i + 5 - 90) * 0.0174532925);
    int x2 = sx2 * (M_SIZE*100 + tl) + M_SIZE*120;
    int y2 = sy2 * (M_SIZE*100 + tl) + M_SIZE*140;
    int x3 = sx2 * M_SIZE*100 + M_SIZE*120;
    int y3 = sy2 * M_SIZE*100 + M_SIZE*140;

    // Green zone limits
    if (i >= -50 && i < 0) {
      M5.Lcd.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_GREEN);
      M5.Lcd.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_GREEN);
    }

    // Yellow zone limits
    if (i >= 0 && i < 25) {
      M5.Lcd.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_YELLOW);
      M5.Lcd.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_YELLOW);
    }

    // Orange zone limits
    if (i >= 25 && i < 50) {
      M5.Lcd.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_RED);
      M5.Lcd.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_RED);
    }

    // Short scale tick length
    if (i % 25 != 0) tl = 8;

    // Recalculate coords incase tick lenght changed
    x0 = sx * (M_SIZE*100 + tl) + M_SIZE*120;
    y0 = sy * (M_SIZE*100 + tl) + M_SIZE*140;
    x1 = sx * M_SIZE*100 + M_SIZE*120;
    y1 = sy * M_SIZE*100 + M_SIZE*140;

    // Draw tick
    M5.Lcd.drawLine(x0, y0, x1, y1, TFT_BLACK);

    // Check if labels should be drawn, with position tweaks
    if (i % 25 == 0) {
      // Calculate label positions
      x0 = sx * (M_SIZE*100 + tl + 10) + M_SIZE*120;
      y0 = sy * (M_SIZE*100 + tl + 10) + M_SIZE*140;
      switch (i / 25) {
        case -2: M5.Lcd.drawCentreString("0", x0, y0 - 12, 2); break;
        case -1: M5.Lcd.drawCentreString("25", x0, y0 - 9, 2); break;
        case 0: M5.Lcd.drawCentreString("50", x0, y0 - 7, 2); break;
        case 1: M5.Lcd.drawCentreString("75", x0, y0 - 9, 2); break;
        case 2: M5.Lcd.drawCentreString("100", x0, y0 - 12, 2); break;
      }
    }

    // Now draw the arc of the scale
    sx = cos((i + 5 - 90) * 0.0174532925);
    sy = sin((i + 5 - 90) * 0.0174532925);
    x0 = sx * M_SIZE*100 + M_SIZE*120;
    y0 = sy * M_SIZE*100 + M_SIZE*140;
    // Draw scale arc, don't draw the last part
    if (i < 50) M5.Lcd.drawLine(x0, y0, x1, y1, TFT_BLACK);
  }

  M5.Lcd.drawString("AM102", M_SIZE*(5 + 220 - 40), M_SIZE*(119 - 20), 2); // Units at bottom right
  M5.Lcd.drawCentreString("CO2", M_SIZE*120, M_SIZE*70, 4); // Comment out to avoid font 4
  M5.Lcd.drawRect(5, 3, M_SIZE*230, M_SIZE*119, TFT_BLACK); // Draw bezel line

  plotNeedle(0, 0, 0,"",0.0,""); // Put meter needle at 0
}

// 
// Update needle position. Modified by J(C)heca from Examples Arduino IDE for M5Stack
// 
void plotNeedle(int value, byte ms_delay, int co2, String sensor, float temp, String momento)
{
  char buf[8]; dtostrf(co2, 4, 0, buf);

  M5.Lcd.drawRightString("ATDFiWare", M_SIZE*60, M_SIZE*(30 - 20), 2);

  // Temperature. Upper Right Corner
  M5.Lcd.fillRect(M_SIZE*60+175, M_SIZE*(30 - 21),50,20, WHITE);    
  M5.Lcd.drawFloat(temp, 1, M_SIZE*60+180, M_SIZE*(30 - 20), 2);
  M5.Lcd.drawRightString("C", M_SIZE*60+222, M_SIZE*(30 - 20), 2);

  M5.Lcd.fillRect(M_SIZE*15, M_SIZE*(119 - 22),50,20, WHITE);    
  M5.Lcd.drawRightString(buf, M_SIZE*40, M_SIZE*(119 - 20), 2);
  M5.Lcd.drawRightString("ppm", M_SIZE*70, M_SIZE*(119 - 20), 2);

  if (value < -10) value = -10; // Limit value to emulate needle end stops
  if (value > 110) value = 110;

  // Move the needle until new value reached
  while (!(value == old_analog)) {
    if (old_analog < value) old_analog++;
    else old_analog--;

    if (ms_delay == 0) old_analog = value; // Update immediately if delay is 0

    float sdeg = map(old_analog, -10, 110, -150, -30); // Map value to angle
    // Calcualte tip of needle coords
    float sx = cos(sdeg * 0.0174532925);
    float sy = sin(sdeg * 0.0174532925);

    // Calculate x delta of needle start (does not start at pivot point)
    float tx = tan((sdeg + 90) * 0.0174532925);

    // Erase old needle image
    M5.Lcd.drawLine(M_SIZE*(120 + 20 * ltx - 1), M_SIZE*(140 - 20), osx - 1, osy, TFT_WHITE);
    M5.Lcd.drawLine(M_SIZE*(120 + 20 * ltx), M_SIZE*(140 - 20), osx, osy, TFT_WHITE);
    M5.Lcd.drawLine(M_SIZE*(120 + 20 * ltx + 1), M_SIZE*(140 - 20), osx + 1, osy, TFT_WHITE);

    // Re-plot text under needle
    M5.Lcd.setTextColor(TFT_BLACK);
    M5.Lcd.drawCentreString("CO2", M_SIZE*120, M_SIZE*70, 4);
    // Re-plot Sensor Name. Down-Right corner
    M5.Lcd.fillRect(M_SIZE*(5 + 220 - 40), M_SIZE*(119 - 20),50,20, WHITE);    
    M5.Lcd.drawString(sensor, M_SIZE*(5 + 220 - 40), M_SIZE*(119 - 20), 2);
    // En cuadro inferior clear
    M5.Lcd.fillRect(115,180,85,25, TFT_GREY);  

    // Draw panic color in bottom square.GREEN
    if (co2 < 800){
       M5.Lcd.fillRect(5,163,M_SIZE*230,60, GREEN);  
    }
    // Draw panic color in bottom square.YELLOW
    if (co2 > 800 and co2 < 1000){
       M5.Lcd.fillRect(5,163,M_SIZE*230,60, YELLOW);   
    }    
    // Draw panic color in bottom square.RED
    if (co2 > 1000){
       M5.Lcd.fillRect(5,163,M_SIZE*230,60, RED);  
    } 

    M5.Lcd.setTextColor(TFT_BLACK);  
    M5.Lcd.drawCentreString(sensor, 155, 180, 4); 
    M5.Lcd.drawCentreString(momento,155,205,2); 

    // Set Default Color for AnalogScreen    
    M5.Lcd.setTextColor(TFT_BLACK);

    // Store new needle end coords for next erase
    ltx = tx;
    osx = M_SIZE*(sx * 98 + 120);
    osy = M_SIZE*(sy * 98 + 140);

    // Draw the needle in the new postion, magenta makes needle a bit bolder
    // draws 3 lines to thicken needle
    M5.Lcd.drawLine(M_SIZE*(120 + 20 * ltx - 1), M_SIZE*(140 - 20), osx - 1, osy, TFT_RED);
    M5.Lcd.drawLine(M_SIZE*(120 + 20 * ltx), M_SIZE*(140 - 20), osx, osy, TFT_MAGENTA);
    M5.Lcd.drawLine(M_SIZE*(120 + 20 * ltx + 1), M_SIZE*(140 - 20), osx + 1, osy, TFT_RED);

    // Slow needle down slightly as it approaches new postion
    if (abs(old_analog - value) < 10) ms_delay += ms_delay / 5;

    // Wait before next update
    delay(ms_delay);
  }
}
