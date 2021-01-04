# co2ATDFiware
Prueba de Sensores CO2. Integración Stack FiWare

# BasicUsageM5Stack.ino

Se trata de un un programa desarrollado para M5Stack Fire consulta los datos abiertos ofrecidos por la página https://www.uco.es/atdfiware/open-data de diferentes sensores gestionados por un stack FiWare. Los sensores cuyos datos se representan son sensores del Aula de Transformación Digital FiWare de la Universidad de Córdoba relativos a la gestión ambiental y concretamente en el ámbito del control del Dióxido de Carbono en estancias con cobertura de comunicación WiFi y/o LoRaWan.

M5Stack es un ESP8266 que se conecta a la WiFi indicada hardcodeada en el código y consulta los datos de sensores/CO2 industriales o desarrollados por ATDFiWare para su estudio y comparación con diferente electrónica y comunicaciones. Se ha desarrollado un programa de representación basado en los ejemplos de M5Stack para su representación, consultado los datos OPEN de ATDFiWare, deserializándolos y representandolos en el LCD del dispositivo.

Con la pulsación del botón A podemos pasar de sensor y con la pulsación del botón B apagamos el dispositivo. En la pantalla se representa un mapeo de los valores "ppm" de cada sensor en una escala del 1 a 100%, así como los valores correspondiente de cada sensor, temperatura y el time en que ha registrado la última lectura en formato GMT.

![Alt text](images/M5Stack.jpeg?raw=true "M5Stack ATDFiWare CO2 Gauge"){:height="24px" width="48px"}
