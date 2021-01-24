#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

const char* ssid = "ssid";
const char* password = "password";

WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  replyPacket[] = "Hi there! Got the message :-)";  // a reply string to send back

DynamicJsonDocument sensor_data(1024);
DynamicJsonDocument telemetry(1024);

// Potenza dello sterzo
int motorB_pwm = 2; //D4
int motorB_pwm_power = 990;

void setup()
{
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

  //Setting up motors
  pinMode(13, OUTPUT); // Inizializzazione del pin direzione del motore collegato al canale A - D8
  pinMode(15, OUTPUT); // Inizializzazione del pin direzione del motore collegato al canale A - D7
  digitalWrite(13, LOW);  // Impostazione della direzione “avanti” del motore collegato al canale A 
  digitalWrite(15, LOW);  // Impostazione della direzione “avanti” del motore collegato al canale A 
  
  pinMode(12, OUTPUT); // Inizializzazione del pin direzione del motore collegato al canale B - D6
  pinMode(14, OUTPUT); // Inizializzazione del pin direzione del motore collegato al canale B - D5
  pinMode(2, OUTPUT); // Inizializzazione del pin direzione del motore collegato al canale B - D5
  digitalWrite(12, LOW);  // Impostazione della direzione “avanti” del motore collegato al canale B
  digitalWrite(14, LOW);  // Impostazione della direzione “avanti” del motore collegato al canale B 
  analogWrite(2,0);

  // Resetting telemetry
  telemetry["motors"][0]["motor"]= "A";
  telemetry["motors"][0]["direction"]= "-";
  telemetry["motors"][0]["power"]= 0;
  telemetry["motors"][1]["motor"]= "B";
  telemetry["motors"][1]["direction"]= "-";
  telemetry["motors"][1]["power"]= 0;
  telemetry["network"]["server"]["ip"] = WiFi.localIP().toString();
  telemetry["network"]["server"]["port"] = localUdpPort;
}


void loop()
{
  // Getting the packet
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    telemetry["network"]["client"]["ip"] = Udp.remoteIP().toString();
    telemetry["network"]["client"]["port"] = Udp.remotePort();
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    // Deserialising the json string
    String incomingPacket_string(incomingPacket);
    deserializeJson(sensor_data, incomingPacket_string);
    int code = sensor_data["code"];
    int value = sensor_data["value"];

    Serial.printf("Code: %d  Value: %d\n",code,value);

    if(code == 2 && value < 100 ){
      Serial.printf("D\n");
      digitalWrite(13, LOW); 
      digitalWrite(15, HIGH);
      // Set telemetry data
      telemetry["motors"][0]["motor"]= "A";
      telemetry["motors"][0]["direction"]= "D";
      telemetry["motors"][0]["power"]= 1023;
    }else if(code == 2 && value > 140 ){
      Serial.printf("R\n");
      digitalWrite(13, HIGH); 
      digitalWrite(15, LOW); 
      // Set telemetry data
      telemetry["motors"][0]["motor"]= "A";
      telemetry["motors"][0]["direction"]= "R";
      telemetry["motors"][0]["power"]= 1023;
    }else if (code == 2){
      Serial.printf("Stop motor D/R\n");
      digitalWrite(13, LOW); 
      digitalWrite(15, LOW); 
      // Set telemetry data
      telemetry["motors"][0]["motor"]= "A";
      telemetry["motors"][0]["direction"]= "-";
      telemetry["motors"][0]["power"]= 0;
    }

    if(code == 0 && value < 480 ){
      Serial.printf("LEFT\n");
      analogWrite(2,motorB_pwm_power);
      digitalWrite(12, LOW); 
      digitalWrite(14, HIGH); 
      // Set telemetry data
      telemetry["motors"][1]["motor"] = "B";
      telemetry["motors"][1]["direction"] = "L";
      telemetry["motors"][1]["power"] = motorB_pwm_power;
    }else if(code == 0 && value > 530 ){
      Serial.printf("RIGHT\n");
      analogWrite(2,motorB_pwm_power);
      digitalWrite(12, HIGH); 
      digitalWrite(14, LOW); 
      // Set telemetry data
      telemetry["motors"][1]["motor"]= "B";
      telemetry["motors"][1]["direction"] = "R";
      telemetry["motors"][1]["power"] = motorB_pwm_power;
    }else if (code == 0){
      Serial.printf("Stop motor LEFT/RIGHT\n");
      analogWrite(2,0);
      digitalWrite(12, LOW); 
      digitalWrite(14, LOW);
      // Set telemetry data
      telemetry["motors"][1]["motor"] = "B";
      telemetry["motors"][1]["direction"] = "-";
      telemetry["motors"][1]["power"] = 0;
    }
    
    Serial.printf("UDP packet contents: %s\n", incomingPacket);

    char replyPacket[1023];
    serializeJson(telemetry,replyPacket);
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(replyPacket);
    Udp.endPacket();
  }
}
