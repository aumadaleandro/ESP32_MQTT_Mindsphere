#include "esp_adc_cal.h"
#include <WiFi.h>
#include <PubSubClient.h>

#define LM35_Sensor1 5
#define DEBUG_LED 20

// WiFi Credentials
const char* ssid = "you_network";
const char* password =  "your_password";

// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 
char *mqttServer = "mqtt_broker_address";
int mqttPort = mqtt_broker_gate;


//LM35 variable declarations
int LM35_Raw_Sensor1 = 0;
float temperature = 0.0;
char tempString[8];
float Voltage = 0.0;

//Variable declarations
long last_time = 0;
bool tempChange = false;

//Timer declaration
hw_timer_t * timer = NULL;

/* Setup Function for WiFi Connection */
void connectToWiFi() {
  Serial.print("Ligação a");
 
  WiFi.begin(ssid, password);
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("Conectado.");
}

/* Setup Function for MQTT Communication */
void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  // define a função de retorno de chamada
  mqttClient.setCallback(callback);
}

/* Setup Function for HW Timer */
void startTimer(){
    timer = timerBegin(0, 80, true); //Selecionado counter 1, timer clock = 80Mhz/80 = 1Mhz
    /*conecta à interrupção do timer
     - timer é a instância do hw_timer
     - endereço da função a ser chamada pelo timer
     - edge=true gera uma interrupção
    */
    timerAttachInterrupt(timer, &cb_timer, true);
    timerAlarmWrite(timer, 5000000, true); //Interrupção a cada 5.000.000 contagens (5 segundo)

    //ativa o alarme
    timerAlarmEnable(timer);
}

/* Stop Function for HW Timer */
void stopTimer(){
    timerEnd(timer);
    timer = NULL; 
}

/* Setup function, runs once */
void setup() {
  // Start serial communication
  Serial.begin(115200);

  //Timer setup
  startTimer();

  //Comms setup
  connectToWiFi();
  setupMQTT();

  //Debug LED
  pinMode(DEBUG_LED, OUTPUT);
  digitalWrite(DEBUG_LED, LOW);
}

/* MQTT Reconnect Function */
void reconnect() {
  Serial.println("Conectando ao MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconectando ao MQTT Broker..");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
        // inscreva-se no tópico
        mqttClient.subscribe("ESP32/debug");
      }
  }
}

/* MQTT Callback (Subscriber) Function */
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Callback - ");
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
}

/* ISR - Called after each timer interrupt */
void cb_timer(){
    static unsigned int counter = 1;
    Serial.println("cb_timer(): ");
    Serial.print(counter);
    Serial.print(": ");
    Serial.print(millis()/1000);
    Serial.println(" segundos");
    
    /* Temperature Reading Logic */
    // Read LM35_Sensor1 ADC Pin
    LM35_Raw_Sensor1 = analogRead(LM35_Sensor1);
    tempChange = true;
    counter++;
}

/* Calibrates de ADC reading */
uint32_t readADC_Cal(int ADC_Raw)
{
  esp_adc_cal_characteristics_t adc_chars;
  
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  return(esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars));
}

void loop() {

  /* Temperature Reading Processing Logic */
  if(tempChange){
    // Calibrate ADC & Get Voltage (in mV)
    Voltage = readADC_Cal(LM35_Raw_Sensor1); 
    // TempC = Voltage(mV) / 10
    temperature = Voltage / 10;
    // Print The Readings
    Serial.printf("Temperature = %f °C \n", temperature);
    tempChange = false;
  }

  /* MQTT Data Send Logic */
  if (!mqttClient.connected())
    reconnect();
  mqttClient.loop();
  long now = millis();
  if (now - last_time > 20000) {
    // Publicando dados através do MQTT
    dtostrf(temperature, 1, 2, tempString);
    Serial.print("----------------\n");
    Serial.print("Sent Value\n");
    Serial.printf("Temperature = %f °C \n", temperature);
    Serial.print("----------------\n");
    mqttClient.publish("ESP32/temperature", tempString);
    last_time = now;
    digitalWrite(DEBUG_LED, HIGH);
    Serial.print("LED Blink\n");
    delay(1000);
    digitalWrite(DEBUG_LED, LOW);
  }

  /* 100ms delay for safety */
  delay(100);
}
