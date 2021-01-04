#define _DEBUG_1;

#include <Arduino.h>
#include "MHZ19.h"                                        
#include <SoftwareSerial.h>                                // Remove if using HardwareSerial

//
// FiWare
//
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "SSID";
const char* password = "password";

// Datos OpenWeatherMap
String hostOpenWeatherMap = "http://api.openweathermap.org/data/2.5/weather";
String idCiudad = "2519240"; // Sustituye por el id de tu ciudad
String apiKey = "<apikey>"; // Sustituye por tu API Key

// FiWare Config UL IoTAgent
const String host = "Stack FiWare Server UL";
const String port = "Stack FiWare Port UL";
const String apik = "Api Key FiWare Device Group";
const String deid = "Provision Device.id";

// FiWare POST URL
String urlpost = "http://"+host+":"+port+"/iot/d?k="+apik+"&i="+deid+"&getCmd=0";

// Peticiones WEB
HTTPClient http;

//
//
//

#define RX_PIN 14                                          // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 12                                          // Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)

MHZ19 myMHZ19;                                             // Constructor for library
SoftwareSerial mySerial(RX_PIN, TX_PIN);                   // (Uno example) create device to MH-Z19 serial

unsigned long getDataTimer = 0;
unsigned long DataTimer = 2 * 1000;
unsigned long getDataPost = 0;
unsigned long DataPost = 300 * 1000;

void setup()
{
    Serial.begin(9600);                                     // Device to serial monitor feedback

    mySerial.begin(BAUDRATE);                               // (Uno example) device to MH-Z19 serial start   
    myMHZ19.begin(mySerial);                                // *Serial(Stream) refence must be passed to library begin(). 

    myMHZ19.autoCalibration();                              // Turn auto calibration ON (OFF autoCalibration(false))

   // Conectar WiFi
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) 
      delay(500); 


}

void loop()
{
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
        Temp_1 = myMHZ19.getTemperature(true);           // Request Temperature (as Celsius)
    
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

            http.begin(urlpost);                                  //Specify request destination    
            http.addHeader("Content-Type", "text/plain");         //Specify content-type header
            http.addHeader("Fiware-Service", "<FiWare-Service>");      //Specify content-type header
            http.addHeader("Fiware-ServicePath", "<FiWare-Servicepath>");   //Specify content-type header
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




void obtenerDatosOpenWeather() {
  
  // Crear URL para hacer la pretición
  String url = hostOpenWeatherMap;
  url += "?id=";
  url += idCiudad;
  url += "&appid=";
  url += apiKey;
 
  #ifdef _DEBUG_
    Serial.print("URL petición HTTP: ");
    Serial.println(url);
  #endif
 
  // Conexión con el servidor y configuración de la petición
  http.begin(url);
 
  // Envío de petición HTTP al servidor
  int codigoHttp = http.GET();
 
  #ifdef _DEBUG_
    Serial.print("Codigo HTTP: ");
    Serial.println(codigoHttp);
  #endif
 
  // Si todo ha ido bien devolverá un número positivo mayor que cero
  if (codigoHttp > 0) {
    // Si ha encontrado el recurso en el servidor responde un código 200
    if (codigoHttp == HTTP_CODE_OK) {
      #ifdef _DEBUG_
        Serial.print("Archivo JSON: ");
        Serial.println(http.getString());
      #endif
 
      // Parsear archivo JSON
      // Para obtener tamaño del buffer vistiar https://arduinojson.org/v6/assistant/
      const size_t capacity = JSON_ARRAY_SIZE(3) +
                              2 * JSON_OBJECT_SIZE(1) +
                              JSON_OBJECT_SIZE(2) +
                              3 * JSON_OBJECT_SIZE(4) +
                              JSON_OBJECT_SIZE(5) +
                              JSON_OBJECT_SIZE(6) +
                              JSON_OBJECT_SIZE(12) +
                              304;
      DynamicJsonDocument doc(capacity);
 
      // Parsear objeto JSON
      DeserializationError error = deserializeJson(doc, http.getString());
      if (error) {
        // Si hay error no se continua
        #ifdef _DEBUG_
          Serial.print("Fallo al parsear JSON. Error: ");
          Serial.println(error.c_str());
        #endif
        return;
      }
 
      // Temperatura
      float tempF = doc["main"]["temp"];
      tempF = tempF - 273.15; // A grados Celsius
      char temp[7];
      snprintf(temp, 7, "%.0f C", tempF);
 
      // Humedad
      String humedadS = String(int(doc["main"]["humidity"])) + " %";
      char humedad[7];
      humedadS.toCharArray(humedad, 7);
 
      // Temperatura mínima
      float tempMinF = doc["main"]["temp_min"];
      tempMinF = tempMinF - 273.15; // A grados Celsius
      char tempMin[7];
      snprintf(tempMin, 7, "%.0f C", tempMinF);
 
      // Temperatura máxima
      float tempMaxF = doc["main"]["temp_max"];
      tempMaxF = tempMaxF - 273.15; // A grados Celsius
      char tempMax[7];
      snprintf(tempMax, 7, "%.0f C", tempMaxF);
 
      // Viento
      float vientoF = doc["wind"]["speed"];
      char viento[8];
      snprintf(viento, 8, "%.0f m/S", vientoF);
 
      // Ciudad
      const char* ciudad = doc["name"];
     
      #ifdef _DEBUG_
        Serial.println("Datos OpenWeatherMap");
        Serial.print("Temperatura: ");
        Serial.println(temp);
        Serial.print("Humedad: ");
        Serial.println(humedad);
        Serial.print("Temp. Min: ");
        Serial.println(tempMin);
        Serial.print("Temp. Max: ");
        Serial.println(tempMax);
        Serial.print("Viento: ");
        Serial.println(viento);

        Serial.print("Ciudad: ");
        Serial.println(ciudad);
      #endif
    } else {
      #ifdef _DEBUG_
        Serial.println("Error al recibir petición.");
      #endif
    }
  }
}
