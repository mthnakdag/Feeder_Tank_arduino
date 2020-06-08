#include <SPI.h>
#include <Ethernet.h>


 // Local Network Settings
byte mac[]     = { 0xD4, 0xA8, 0xE2, 0xFE, 0xA0, 0xA4 }; // Must be unique on local network

 char server[] = "192.168.72.127";
const int updateInterval = 10000;        // Time interval in milliseconds to update server   

// Variable Setup
long lastConnectionTime = 0; 
boolean lastConnected = false;
int failedCounter = 0;

// Initialize Arduino Ethernet Client
EthernetClient client;

void setup()
{

  Serial.begin(9600);
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    } else if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // no point in carrying on, so do nothing forevermore:
    while (true) {
      delay(1);
    }
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
}
 
void loop()
{
  
   // Print Update Response to Serial Monitor
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }
  
  // Disconnect from Server
  if (!client.connected() && lastConnected)
  {
    Serial.println();
    Serial.println("...disconnected.");
    Serial.println();
    
    client.stop();
     
  }
  
   if(!client.connected() && (millis() - lastConnectionTime > updateInterval))
  {
    updateserver("27"); 
     
  }
  
  lastConnected = client.connected();
}

void updateserver(String  sData)
{
  if (client.connect(server, 1026))
  { 
    client.print("PUT  /v2/entities/urn:ngsi-ld:Sensor:unit003/attrs/distance/value HTTP/1.1\n");
    client.print("Host: 192.168.72.127 \n");
    client.print("Connection: close  \n");
    client.print("Content-Type: text/plain \n");
    client.print("Content-Length: ");
    client.print(String(sData.length()));
    client.print("\n\n");
    client.print(27);
    lastConnectionTime = millis();
    
    if (client.connected())
    {
      Serial.println("Connecting to server...");
      Serial.println();
      
      failedCounter = 0;
    }
    else
    {
      failedCounter++;
  
      Serial.println("Connection to server failed ("+String(failedCounter, DEC)+")");   
      Serial.println();
    }
    
  }
  else
  {
    failedCounter++;
    
    Serial.println("Connection to server Failed ("+String(failedCounter, DEC)+")");   
    Serial.println();
    
    lastConnectionTime = millis(); 
  }
}
void startEthernet()
{
  
  
}
 
