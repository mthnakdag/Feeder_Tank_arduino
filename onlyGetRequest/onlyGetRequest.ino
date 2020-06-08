#include <SPI.h>
#include <Ethernet.h>


 // Local Network Settings
byte mac[]     = { 0xD4, 0xA8, 0xE2, 0xFE, 0xA0, 0xA4 }; // Must be unique on local network

String total="";
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
 Ethernet.begin(mac);
  client.stop();
  delay(1000);
  if(Ethernet.begin(mac) == 0){
    Serial.println("Please Reset Arduino");
  }
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
  // Disconnect from 
  if (!client.connected() && lastConnected)
  {
    Serial.println();
    Serial.println("...disconnected.");
    Serial.println();
    
    client.stop();
     
  }
  
   if(!client.connected() && (millis() - lastConnectionTime > updateInterval))
  {
    
    updateserver( ); 
     
  }
  
  lastConnected = client.connected();
}

void updateserver()
{
  if (client.connect(server, 1026))
  { 
    
    client.print("GET /v2/entities/urn:ngsi-ld:Sensor:unit003/attrs/distance/value HTTP/1.1\n");
    client.print("Host: 192.168.72.127 \n");
    client.print("Connection: close \n");
    client.print("\n\n");
     lastConnectionTime = millis();
     
    if (client.connected())
    {
      
      Serial.println("Connecting to Server...");
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
