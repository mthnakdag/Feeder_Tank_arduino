#include <SoftwareSerial.h>
#include <Servo.h>
#include <SPI.h>
#include <Ethernet.h>

//--------------------------------------------------------------------------
//             *     ,MMM8&&&.            *                                 |
//                  MMMM88&&&&&    .                                        |
//                 MMMM88&&&&&&&                                            | 
//     *           MMM88&&&&&&&&                                            |       
//                 MMM88&&&&&&&&                                            |                                    
//                 'MMM88&&&&&&'                                            |        
//                   'MMM8&&&'      *                                       |    
//          |\___/|                                                         |                       
//          )     (             .              '                            |   
//         =\     /=                                                        |        
//           )===(       *                                                  |     
//          /     \                                                         |                       
//          |     |                                                         |   
//         /       \                                                        |    
//         \       /                                                        |    
//  _/\_/\_/\__  _/_/\_/\_/\_/\_/\_/\_/\_/\_/\_                             |  
//  |  |  |  |( (  |  |  |  |  |  |  |  |  |  |                             |   
//  |  |  |  | ) ) |  |  |  |  |  |  |  |  |  |                             |  
//  |  |  |  |(_(  |  |  |  |  |  |  |  |  |  |                             |     
//  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |                             |   
//  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |                             |          
//---------------------------------------------------------------------------

//ARDUNIO CONFIGURATION FOR Local Network Settings
byte mac[]     = { 0xD4, 0xA8, 0xE2, 0xFE, 0xA0, 0xA4 }; // Must be unique on local network


/*
 * MOTOR CLASS CODE
 */
class ServoMotor{
  public:
    Servo thisServo;
    int pos=0;
    int pin;
    int increment=1;
    String idnum;
    boolean isOpen=false;
  public:
    ServoMotor(int pin,String idnum){
      this->pin=pin;
      this->idnum=idnum;
    }
    void attachpin(){
    thisServo.attach(pin);
}
    void opengap(){
      this->pos=0;
      this->increment=1;
      for(int pos=this->pos ; pos<=90; pos+=increment){
        thisServo.write(pos);
        delay(50);
        this->pos=pos;
      }
      if(this->pos==90)//change to status
      increment=increment*(-1);   
      }
      
    void closegap(){
      this->pos=90;
      for(int pos=this->pos;pos>=0;pos+=increment){
          thisServo.write(pos);
          delay(50);
          this->pos=pos;
        }
       if(this->pos == 0 ) //change to status
      increment=increment*(-1);
    }  
    ~ServoMotor(){
      thisServo.detach();
      }  
};



/*
 * SENSOR CLASS CODE
 */
class HCsensor{
  public:
    String idnum;
    int trigPin,echoPin;
    HCsensor(int trig,int echo,String idnum){
    this->trigPin=trig;
    this->echoPin=echo;
    this->idnum=idnum;
    pinMode(trigPin,OUTPUT);
    pinMode(echoPin,INPUT);
    }
    int updateDistance(){
      long cm,duration;
      digitalWrite(trigPin,LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin,HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin,LOW);
      duration=pulseIn(echoPin,HIGH);
      cm=duration/29/2;
      return cm;
    }
};

//Sensors
HCsensor sensor1(2,3,"001");
HCsensor sensor2(2,4,"002");
HCsensor sensor3(2,5,"003");
HCsensor sensor4(2,6,"004");

//Motors
ServoMotor servo1(7,"001");
ServoMotor servo2(8,"002");
ServoMotor servo3(9,"003");
ServoMotor servo4(10,"004");


//SERVER SETTINGS
char server[] = "192.168.1.22"; //CONTEXT BROKER FOR MY LOCAL AREA
int port=1026; //CONTEXT BROKER PORT
int failedCounter=0;
String getresponse=""; //For Config ARdunion get response
EthernetClient client;//client







/*
 * ETHERNET PARTS
 */
int HttptoInt(){  //GET request also must be converting Int
  getresponse="";
  while (client.available())
  {
     char c = client.read();
     getresponse+=c;  
  }
  if(getresponse.length()>2)
  {
    getresponse=getresponse.substring(getresponse.length()-2);
    return getresponse.toInt();
  }
}

int readDistance(String idnum)  //Distance from Fiware
{
  if (client.connect(server, port))
  { 
    client.print("GET /v2/entities/urn:ngsi-ld:Sensor:unit"+idnum+"/attrs/distance/value HTTP/1.1\n");
    client.print("Host:192.168.1.22 \n");
    client.print("Connection: close \n");
    client.print("\n\n");  
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
  }
  return HttptoInt();
}

void writeDistance(String idnum,String sData)  //Distance Write to Fiware
{ 
  if (client.connect(server, 1026))
  { 
    client.print("PUT  /v2/entities/urn:ngsi-ld:Sensor:unit"+idnum+"/attrs/distance/value HTTP/1.1\n");
    client.print("Host: 192.168.1.22 \n");
    client.print("Connection: close  \n");
    client.print("Content-Type: text/plain \n");
    client.print("Content-Length: ");
    client.print(String(sData.length()));
    client.print("\n\n");
    client.print(sData.toInt());
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
  }
}




void setup() {
    
  Serial.begin(9600);
  servo1.attachpin();
  servo2.attachpin();
  servo3.attachpin();
  Ethernet.begin(mac);
  client.stop();
  delay(1000);
  if(Ethernet.begin(mac) == 0){
    Serial.println("Please Reset Arduino");
  }
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
   
}


void loop() {

  //FOR OPEN SEGMENTATION
  //open food tank   
  if(readDistance(sensor1.idnum)>=40 && !servo1.isOpen){
    servo1.opengap();
    delay(70);
    servo1.isOpen=true;
    client.stop();
  }
  delay(70);
  //open water tank
  if(readDistance(sensor2.idnum)>=40 && !servo2.isOpen){
    delay(70);
    servo2.opengap();
    servo2.isOpen=true;
    client.stop();
  }
  delay(70);
  //open food feeder
  if(readDistance(sensor3.idnum)>=55 && !servo3.isOpen){
    servo3.opengap();
    servo3.isOpen=true;
    client.stop();
  }
  //open Water feeder
  if(readDistance(sensor4.idnum)>=55 && !servo4.isOpen){
    delay(70);
    servo4.opengap();
    servo4.isOpen=true;
    client.stop();
  }
  delay(70);
  //FOR CLOSE SEGMENTATION
  //close food tank   
  if(readDistance(sensor1.idnum)<=15 && servo1.isOpen){
    delay(70);
    servo1.closegap();
    servo1.isOpen=false;
    client.stop();
  }
  delay(70);
  //close water tank
  if(readDistance(sensor2.idnum)<=15 && servo2.isOpen){
    delay(70);
    servo2.closegap();
    servo2.isOpen=false;
    client.stop();
  }
  delay(70);
  //close food feeder
  if(readDistance(sensor3.idnum)<=50 && servo3.isOpen){
    delay(70);
    servo3.closegap();
    servo3.isOpen=false;
    client.stop();
  }
  delay(70);
  //close food feeder
  if(readDistance(sensor4.idnum)<=50 && servo4.isOpen){
    delay(70);
    servo4.closegap();
    servo4.isOpen=false;
    client.stop();
  }
  delay(70);

  //UPDATE DATA'S PART
  delay(600);
  writeDistance(sensor1.idnum,String(sensor1.updateDistance()));
  delay(600);
  writeDistance(sensor2.idnum,String(sensor2.updateDistance()));
  delay(600);
  writeDistance(sensor3.idnum,String(sensor3.updateDistance()));
  delay(600);
  writeDistance(sensor4.idnum,String(sensor4.updateDistance()));
  //delay(600000);
}
