# WemosComPVter
repository contenente codice presentato nel corso "we mos" :)
13 giugno 2017: Wemos, arduino IDE e MQTT

ESP_MQTT_DHT11_CPVWM_1.ino   -> Sketch per wemos D1mini. Legge dato sentore DHT11 e lo pubblica via MQTT. 
ESP_MQTT_BUTTON_CPVWM_02.ino -> Sketch per wemos D1mini. Manda azioni pulsante via MQTT
ESP_MQTT_Relay_CPVWM_03.ino  -> Sketch per wemos D1mini. Si sottoscrive al topic "actions" e attende "{trigger: double}" per attivare il relay