#define _DEBUG_;

#include <Wire.h>    // I2C library
#include "ccs811.h"  // CCS811 library

// Wiring for ESP8266 NodeMCU boards: VDD to 3V3, GND to GND, SDA to D2, SCL to D1, nWAKE to D3 (or GND)
CCS811 ccs811(D3); // nWAKE on D3

//
// FiWare
//
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "SSID";
const char* password = "password";

// FiWare Config UL IoTAgent
const String host = "Stack FiWare Server UL";
const String port = "Stack FiWare Port UL";
const String apik = "Api Key FiWare Device Group";
const String deid = "Provision Device.id";

// FiWare POST URL
String urlpost = "http://"+host+":"+port+"/iot/d?k="+apik+"&i="+deid+"&getCmd=0";

// Peticiones WEB
HTTPClient http;

unsigned long getDataTimer = 0;
unsigned long DataTimer = 2 * 1000;
unsigned long getDataPost = 0;
unsigned long DataPost = 300 * 1000;

void setup()
{
    Serial.begin(115200);  // Device to serial monitor feedback
    
    #ifdef _DEBUG_
      Serial.println("");
      Serial.println("setup: Starting CCS811 basic demo");
      Serial.print("setup: ccs811 lib  version: "); 
      Serial.println(CCS811_VERSION); 
    #endif 
         
    // Enable I2C
    Wire.begin(); 

    // Enable CCS811
    ccs811.set_i2cdelay(50); // Needed for ESP8266 because it doesn't handle I2C clock stretch correctly
    bool ok= ccs811.begin();
    if( !ok ) Serial.println("setup: CCS811 begin FAILED");  

    // Print CCS811 versions
    #ifdef _DEBUG_
      Serial.print("setup: hardware    version: "); Serial.println(ccs811.hardware_version(),HEX);
      Serial.print("setup: bootloader  version: "); Serial.println(ccs811.bootloader_version(),HEX);
      Serial.print("setup: application version: "); Serial.println(ccs811.application_version(),HEX); 
    #endif 
     
    // Start measuring
    ok= ccs811.start(CCS811_MODE_1SEC);
    if( !ok ) Serial.println("setup: CCS811 start FAILED");

   // Conectar WiFi
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) 
      delay(500); 


}

void loop()
{
    if (millis() - getDataTimer >= DataTimer)
    {

        // Read
        uint16_t eco2, etvoc, errstat, raw;
        ccs811.read(&eco2,&etvoc,&errstat,&raw); 

        // Print measurement results based on status
        if( errstat==CCS811_ERRSTAT_OK ) { 
          Serial.print("CCS811: ");
          Serial.print("eco2=");  Serial.print(eco2);     Serial.print(" ppm  ");
          Serial.print("etvoc="); Serial.print(etvoc);    Serial.print(" ppb  ");
          Serial.print("mA=");  Serial.print(raw/1024); Serial.print(" uuA  "); 
          Serial.print("adc="); Serial.print(raw%1024); Serial.print(" ADC  ");
          Serial.print("ohm="); Serial.print((1650*1000L/1023)*(raw%1024)/(raw/1024)); Serial.print(" ohm");
          Serial.println();
        } else if( errstat==CCS811_ERRSTAT_OK_NODATA ) {
          Serial.println("CCS811: waiting for (new) data");
        } else if( errstat & CCS811_ERRSTAT_I2CFAIL ) { 
          Serial.println("CCS811: I2C error");
        } else {
          Serial.print("CCS811: errstat="); Serial.print(errstat,HEX); 
          Serial.print("="); Serial.println( ccs811.errstat_str(errstat) ); 
        }
                                  
        getDataTimer = millis();

        // POST Fiware
        if (millis() - getDataPost >= DataPost){

          if(WiFi.status()== WL_CONNECTED){

            http.begin(urlpost);                                            //Specify request destination    
            http.addHeader("Content-Type", "text/plain");                   //Specify content-type header
            http.addHeader("Fiware-Service", "<FiWare-Service>");           //Specify content-type header
            http.addHeader("Fiware-ServicePath", "<FiWare-Servicepath>");   //Specify content-type header
            int httpCode = http.POST("eco2|"+String(eco2)+"|etvoc|"+String(etvoc)+"|mcramp|"+String(raw/1024)+"|adc|"+String(raw%1024)+"|ohm|"+String((1650*1000L/1023)*(raw%1024)/(raw/1024)));         //Send the request
            String payload = http.getString();
            http.end();
  
            // DEBUG
            #ifdef _DEBUG_
              Serial.print("POST Fiware: ");
              Serial.println(urlpost);
              Serial.print("HTTP POST Code: ");
              Serial.println(httpCode);
            #endif 
            
          }
                    
          //
          getDataPost = millis();
        }


        
    }
}
