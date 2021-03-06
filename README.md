# co2ATDFiware
Prueba de Sensores CO2. Integración Stack FiWare

En el Aula de Transformación Digital FiWare de la Universidad de Córdoba se han instalado diferentes tipos de sensores para la calidad del aire, concretamente dispositivos industriales listos para su configuración e instalación. Estos dispositivos tienen un coste determinado por unidad y unas caracteristicas de comuicación concretas.

La idea de este repositorio es probar diferentes tipos de sensores de bajo coste y realizar estudios de comparación con los semejantes industriales de los que disponemos utilizando otros tipos de tecnologías de comunicación con el fin de ser más accesibles por el usuario final y poder ser instalados en nuestra propia estancia, despacho o casa.

Toda la información sobre los sensores del aula se puede encontrar en https://www.uco.es/atdfiware/open-data donde se describe los datos capturados por el sensor, integración con FiWare, el sistema de comunicación y protocolo de acceso así como enlaces a la página del producto.  Los datos del Stack FiWare se pueden consultar a través de sus protocolos (NGSI/V2 ó NGSI TSDB) y su consumo, también es posible, a través de herramientas de graficación histórica como la representada en la imagen a continuación por Grafana.

![Alt text](images/PanelSensor.png?raw=true "Dashboard Integración Sensores CO2 FiWare")

Además estos mismos datos se ofrecen en abierto para que puedan ser consultados desde cualquier aplicación con simples peticiones HTTP y deserialización de información en formato JSON.

Por lo tanto, teniendo el sistema de gestión de datos que registra información sensórica y la ofrece para su consumo, sólo nos queda ir probando los diferentes tipos de sensores ambientales CO2 que vayamos encontrando y comprobar su funcionamiento a través de la representación de los datos en las posibilidades anteriormente descritas.

A continuación destacaremos la instalación e integración con FiWare de los sensore de bajo coste:

# 1.- Sensor MHZ19 con NodeMCU V3.0.0.

El sensor:

![Alt text](images/MH-Z19.jpg?raw=true "Sensor CO2 MH-Z19")

El esquema de conexión a NodeMCU: 

![Alt text](images/schema01.jpeg?raw=true "Conexión Sensor CO2 MH-Z19 con NodeMCU V3")

Resultado final con el envase :-)

![Alt text](images/case.jpg?raw=true "Sensor CO2 Envase")

# 2.- Sensor CCS811 con NodeMCU Amica ESP8266-E

El sensor:

![Alt text](images/ccs811.png?raw=true "Sensor CO2 CCS811")

El esquema de conexión a NodeMCU según las indicación y uso de librería de Maarten Pennigs https://github.com/maarten-pennings/CCS811:

![Alt text](images/schema03.jpg?raw=true "Conexión Sensor CO2 CCS811 con NodeMCU Amica ESP8266-E")

El resultado final, antes de "envasar":

# 3.- Sensor MHZ14 con NodeMCU V3.0.0.

El sensor MH-Z14a y la placa NodeMCU:

![Alt text](images/mhz14a.jpeg?raw=true "Sensor CO2 MH-Z14")


# 4.- Códigos de programación.

Los códigos correspondientes a la conexión WiFi de los ESP8266 (NodeMCU), la recogida de los datos del sensor y la subida de los mismos al Stack FiWare del Aula de Transformación Digital FiWare de la Universidad de Córdoba es el que describimos a continuación dependiendo del tipo de MCU:

# * BasicUsageMHZ19.ino (for NodeMCU V3)

Es el programa de .ino encargado de realizar la operación descrita anteriormente por el sensor MHZ19 para poder recuperar los datos y inyectartlos en el Stack de FiWare para que puedan ser consumidos por protocolo o por datos abiertos.

# * BasicUsageCCS811.ino (for NodeMCU Amica ESP8266-E)

Es el programa de .ino encargado de realizar la operación descrita anteriormente por el sensor CCS811 para poder recuperar los datos y inyectartlos en el Stack de FiWare para que puedan ser consumidos por protocolo o por datos abiertos.

# * BasicUsageMHZ14A.ino (for NodeMCU V3)

Es el programa de .ino encargado de realizar la operación descrita anteriormente por el sensor MHZ14a para poder recuperar los datos y inyectartlos en el Stack de FiWare para que puedan ser consumidos por protocolo o por datos abiertos.

# 5.- Representación de datos.

# BasicUsageM5Stack.ino

Se trata de un un programa desarrollado para M5Stack Fire que consulta los datos abiertos ofrecidos por la página https://www.uco.es/atdfiware/open-data de diferentes sensores gestionados por un stack FiWare. Los sensores cuyos datos se representan son sensores del Aula de Transformación Digital FiWare de la Universidad de Córdoba relativos a la gestión ambiental y concretamente en el ámbito del control del Dióxido de Carbono en estancias con cobertura de comunicación WiFi y/o LoRaWan.

M5Stack es un ESP8266 que se conecta a la WiFi indicada hardcodeada en el código y consulta los datos de sensores/CO2 industriales o desarrollados por ATDFiWare para su estudio y comparación con diferente electrónica y comunicaciones. Se ha desarrollado un programa de representación basado en los ejemplos de M5Stack para su representación, consultado los datos OPEN de ATDFiWare, deserializándolos y representandolos en el LCD del dispositivo.

Con la pulsación del botón A podemos pasar de sensor y con la pulsación del botón B apagamos el dispositivo. En la pantalla se representa un mapeo de los valores "ppm" de cada sensor en una escala del 1 a 100%, así como los valores correspondiente de cada sensor, temperatura y el time en que ha registrado la última lectura en formato GMT.

Haz click en la imagen para ver video con una prueba:

[![Ver VIDEO](images/M5Stack.jpeg?raw=true)](https://youtu.be/c4iqZGf_Hb8)

# 6.- ATDFiWareSonda.ino

Versión mejorada de "BasicUsageMHZ19.ino (for MHZ19)" donde se incorpora portal cautivo para la configuración de la WiFi y pantalla OLED.




