# ESP32_MQTT_Mindsphere
Test project on the development of an IoT application to monitor temperature of a device on a cloud environment.
  <li> Simple smart instrument developed with an ESP32 microcontroller (for connectivity) and a LM35 temperature sensor
  <li> Edge device developed at an Ubuntu 18.0.4 virtual machine running on Windows 11 host, holding: Mosquitto MQTT Broker, node-red
  <li> Edge device suitable to be applied on development board such as Raspberry-Pi or some industrial IoT Gateway (ex: Siemens SIMATIC 2040)
  <li> Cloud data processing made at Siemens MindSphere
  <li> Cloud-based dashboard made at Siemens MindSphere
  <li> Data transfer between instrument and edge device via MQTT

Files and descriptions:
  <li> tempMonMQTT.ino - Arduino IDE source code, to be downloaded to ESP 32
  <li> tempMonEdge.json - Node-red flow, to be used on node-red instance at the Edge Device
  <li> tempMonCloud.json - Node-red flow, to be used on node-red (called visual flow creator) instance at Siemens MindSphere
  <li> tempMonArch.jpg - Image of the architecture of the entire application
    
Details of development and demonstration can be found on youtube video (on brazilian Portuguese):
