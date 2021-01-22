#define _DEBUG_;

#include "MHZ19.h"                                        
#include <SoftwareSerial.h> 
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiManager.h>                                              
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// Portal Cautivo
WiFiManager wm;
//WiFiManagerParameter custom_sonda_id("sonda", "Sonda ID", "", 40);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// 'atd', 128x32px
const unsigned char myBitmap [] PROGMEM = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xe0, 0x3f, 0xfc, 0x00, 0x00, 0x7f, 0xf0, 0x1c, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0x80, 0x0f, 0xf8, 0x00, 0x00, 0x3f, 0xc0, 0x04, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0x00, 0x03, 0xf8, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xfc, 0x00, 0x01, 0xf8, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xf9, 0xcf, 0xff, 0xff, 
  0xfc, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x1f, 0xff, 0xff, 0x39, 0xc0, 0xff, 0xff, 
  0xf8, 0x00, 0x00, 0xfe, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x1f, 0xff, 0xff, 0x31, 0xc0, 0x1f, 0xff, 
  0xf0, 0x0f, 0xc0, 0x7f, 0xf8, 0x3f, 0xf8, 0x0f, 0xc0, 0x1f, 0xff, 0xfe, 0x71, 0xc0, 0x0f, 0xff, 
  0xf0, 0x3f, 0xe0, 0x7f, 0xf8, 0x3f, 0xf8, 0x1f, 0xf0, 0x1f, 0xff, 0xfe, 0x71, 0xf0, 0x1f, 0xff, 
  0xf0, 0x7f, 0xf0, 0x3f, 0xf8, 0x3f, 0xf0, 0x3f, 0xf8, 0x1f, 0xff, 0xfc, 0x71, 0xff, 0xfc, 0xff, 
  0xe0, 0x7f, 0xf0, 0x3f, 0xf8, 0x3f, 0xf0, 0x3f, 0xf8, 0x1f, 0xff, 0xfc, 0x70, 0xff, 0xf0, 0xff, 
  0xe0, 0x7f, 0xf8, 0x3f, 0xf8, 0x3f, 0xf0, 0x7f, 0xf8, 0x1f, 0xff, 0xfc, 0x70, 0x3f, 0xc1, 0xff, 
  0xe0, 0x7f, 0xf8, 0x3f, 0xf8, 0x3f, 0xf0, 0x7f, 0xf8, 0x1f, 0xff, 0xfc, 0x78, 0x00, 0x03, 0xdf, 
  0xe0, 0x00, 0x00, 0x3f, 0xf8, 0x3f, 0xf0, 0x7f, 0xf8, 0x1f, 0xff, 0xfc, 0x7c, 0x00, 0x07, 0x8f, 
  0xe0, 0x00, 0x00, 0x3f, 0xf8, 0x3f, 0xf0, 0x7f, 0xf8, 0x1f, 0xff, 0xfc, 0x3e, 0x00, 0x1f, 0x1f, 
  0xe0, 0x00, 0x00, 0x3f, 0xf8, 0x3f, 0xf0, 0x3f, 0xf8, 0x1f, 0xff, 0xfc, 0x1f, 0xe1, 0xfc, 0x3f, 
  0xe0, 0x00, 0x00, 0x3f, 0xf8, 0x3f, 0xf0, 0x1f, 0xf0, 0x3f, 0xff, 0xfe, 0x0f, 0xff, 0xf8, 0x7f, 
  0xe0, 0x00, 0x00, 0x3f, 0xf8, 0x3f, 0xf8, 0x0f, 0xe0, 0x3f, 0xff, 0xfe, 0x07, 0xff, 0xe0, 0xff, 
  0xe0, 0x0f, 0xf8, 0x3f, 0xf8, 0x3f, 0xf8, 0x07, 0xc0, 0x7f, 0x81, 0xff, 0x00, 0xff, 0x01, 0xff, 
  0xe0, 0x3f, 0xf8, 0x3f, 0xf8, 0x3f, 0xfc, 0x00, 0x00, 0x7f, 0x81, 0xff, 0x80, 0x00, 0x03, 0xff, 
  0xe0, 0x7f, 0xf8, 0x3f, 0xf8, 0x3f, 0xfe, 0x00, 0x00, 0xff, 0x81, 0xff, 0xc0, 0x00, 0x0f, 0xff, 
  0xe0, 0x7f, 0xf8, 0x3f, 0xf8, 0x3f, 0xff, 0x00, 0x01, 0xff, 0x81, 0xff, 0xf8, 0x00, 0x7f, 0xff, 
  0xe0, 0xff, 0xf8, 0x3f, 0xf8, 0x3f, 0xff, 0x80, 0x07, 0xff, 0x81, 0xff, 0xff, 0xdf, 0xff, 0xff, 
  0xf0, 0xff, 0xfc, 0x7f, 0xfc, 0x7f, 0xff, 0xe0, 0x1f, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// FiWare Config UL IoTAgent
const String host = "<FiWare Ultralight Server>";
const String port = "<Ultralight Server Port>";
const String apik = "<FiWare Group Tocken>";
const String deid = "<FiWare Device ID>";

// Preheating time
const unsigned long MHZ19_PREHEATING_TIME = 2L * 60L * 1000L;

// FiWare POST URL
String urlpost = "http://"+host+":"+port+"/iot/d?k="+apik+"&i="+deid+"&getCmd=0";

// Peticiones WEB
HTTPClient http;

//
// Conexion Pantalla
//
#define RX_PIN 14                                          // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 12                                          // Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)

MHZ19 myMHZ19;                                             // Constructor for library
SoftwareSerial mySerial(RX_PIN, TX_PIN);                   // (Uno example) create device to MH-Z19 serial

// Data Timer
unsigned long getDataTimer = 0;
unsigned long DataTimer = 2 * 1000;
// Data Post FiWare
unsigned long getDataPost = 0;
unsigned long DataPost = 300 * 1000;
int val = 0;

void setup() {

    Serial.begin(9600);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }

    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP        
    //reset settings - wipe credentials for testing
    //wm.resetSettings();

    wm.setAPCallback(configModeCallback);
    //automatically connect using saved credentials if they exist
    //If connection fails it starts an access point with the specified name
    if(wm.autoConnect("ATDFiWareAP")){

        #ifdef _DEBUG_
          Serial.println("Connected...:)");
        #endif
        
        // Logo
        testdrawbitmap();
        delay(2000);
        display.display();

        // Sensores
        mySerial.begin(BAUDRATE);                               // (Uno example) device to MH-Z19 serial start   
        myMHZ19.begin(mySerial);                                // *Serial(Stream) refence must be passed to library begin(). 
        myMHZ19.autoCalibration();                              // Turn auto calibration ON (OFF autoCalibration(false))       

        // PreHeating
        if (isPreHeating()) {
          #ifdef _DEBUG_
            Serial.print("Preheating");
          #endif  
          boolean color = false;
          while (isPreHeating()) {
            #ifdef _DEBUG_
              Serial.print(".");
            #endif
            color = !color;
            display.invertDisplay(color);
            display.display();
            delay(1000);            
          }
          Serial.println();
         } 

         //
         // Set invertDisplay FALSE
         //
         display.invertDisplay(false);
 
    }
    else {
        Serial.println("Configportal running");
    }
    
}

void loop() {

    // Process WiFiManager
    wm.process();

    // Read Data Sensor in DataTimer
    if (millis() - getDataTimer >= DataTimer)
    {
        int CO2; 
        double Temp;
        double Temp_1; 

        /* note: getCO2() default is command "CO2 Unlimited". This returns the correct CO2 reading even 
        if below background CO2 levels or above range (useful to validate sensor). You can use the 
        usual documented command with getCO2(false) */

        CO2 = myMHZ19.getCO2();                             // Request CO2 (as ppm)                               
        Temp = myMHZ19.getTemperature(true,true);           // Request Temperature (as Celsius)
        Temp_1 = myMHZ19.getTemperature(true);              // Request Temperature (as Celsius)

        // Display Frame
        display.clearDisplay();
        display.drawRoundRect(0,0,128,32,1,SSD1306_WHITE);
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        // Display CO2 value + sufix
        display.setCursor(20,3);
        display.print(CO2);
        display.println(" PPm");
        // Display TEMP value + sufix
        display.setTextSize(1);
        display.setCursor(67,22);
        display.print(Temp_1);
        display.print(" ");
        display.write(167);
        display.print("C");

        // Draw Signal Blocks (bottom left border)
        val = map(CO2, 400, 1500, 1, 5);
        for(int16_t i=1; i<=5; i+=1) {
           if (val <= i){
              // display.drawRect(5*i,25-i,4,5+i,WHITE);     
              display.drawRect(5*i,25,4,5,WHITE);                
           } else {
              // display.fillRect(5*i,25-i,4,5+i,WHITE);
              display.fillRect(5*i,25,4,5,WHITE);
           }
        }
        
        // ReDraw. Send to OLED
        display.display();
     
        #ifdef _DEBUG_
           Serial.print("CO2 (ppm): ");
           Serial.println(CO2);
           Serial.print("Temperature (Cº): ");
           Serial.println(Temp_1);
        #endif 
                                  
        getDataTimer = millis();

        // POST Fiware
        if (millis() - getDataPost >= DataPost){

          if(WiFi.status()== WL_CONNECTED){

            http.begin(urlpost);                                                          //Specify request destination    
            http.addHeader("Content-Type", "text/plain");                                 //Specify content-type header
            http.addHeader("Fiware-Service", "<FiWare-Service>");                         //Specify content-type header
            http.addHeader("Fiware-ServicePath", "<FiWare-ServicePath>");                 //Specify content-type header
            int httpCode = http.POST("ppm|"+String(CO2)+"|temp|"+String(Temp_1));         //Send the request
            String payload = http.getString();
            http.end();
  
            // DEBUG
            #ifdef _DEBUG_
              Serial.print("POST Fiware: ");
              Serial.println(urlpost);
              Serial.print("HTTP POST Code: ");
              Serial.println(httpCode);
            #endif 

            // Obtener datos Meteorológicos
            // obtenerDatosOpenWeather();            
            
          }
                    
          //
          getDataPost = millis();
        }
        
    }
    
}

// Save extra config parameters
void saveParamsCallback () {
  Serial.println("Get Params:");
//Serial.print(custom_sonda_id.getID());
  Serial.print(" : ");
//Serial.println(custom_sonda_id.getValue());
}

// Draw Logo
void testdrawbitmap(void) {
  display.clearDisplay();
  display.drawBitmap(0,0,myBitmap, 128, 32, 1);
  display.display();
  delay(1000);
}

// Preheating Sensor
boolean isPreHeating() {  
    return millis() < (MHZ19_PREHEATING_TIME);
}

// Config Mode to Display
void configModeCallback (WiFiManager *wm) {

  #ifdef _DEBUG_
    Serial.println("Entered config mode.");
    Serial.println(WiFi.softAPIP());
    Serial.println(wm->getConfigPortalSSID());
  #endif

  // OLED Display Portal warning
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println("Portal Running...");
  display.println("");
  display.print(wm->getConfigPortalSSID());
  display.display();
  
}
