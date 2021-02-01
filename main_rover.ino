#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>   

WiFiManager wifiManager;

WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets

DynamicJsonDocument sensor_data(1024);
DynamicJsonDocument telemetry(1024);

int motorA1 = 13;
int motorA2 = 15;

int motorB1 = 12;
int motorB2 = 14;

#define LED_ON LOW
#define LED_OFF HIGH


// Counicate the last two digits of the ip using led --> 192.168.1.43 --> 1.43
void comunicateIp(String ip) {

  Serial.println("Printing ip using led");

  // Splitting the ip by "."
  int lenght = ip.length() + 1;
  char ip_to_split[lenght];
  ip.toCharArray(ip_to_split, lenght);

  int i = 0;
  char *p = strtok (ip_to_split, ".");
  char *array[4];

  while (p != NULL)
  {
    array[i++] = p;
    p = strtok (NULL, ".");
  }



  // Blick fast --> start comunication
  for (int j = 0 ; j < 20; j++) {
    digitalWrite(LED_BUILTIN, LED_ON);
    delay(100);
    digitalWrite(LED_BUILTIN, LED_OFF);
    delay(100);
  }
  delay(3000);

  // Taking the last two numbers of the ip address --> 192.168.1.43 --> array[2] = 1 array[3] = 43

  for (int j = 0; j < String(array[2]).length(); j++) {
    //Needed for double numbers --> 1 --> blick 1 --> delay 3
    for ( int k = 0; k < (int)array[2][j] - '0'; k++) {
      digitalWrite(LED_BUILTIN, LED_ON);
      delay(500);
      digitalWrite(LED_BUILTIN, LED_OFF);
      delay(500);
    }
    delay(1000);
  }

  // Blick fast 10 times --> emulating a .
  for (int j = 0 ; j < 10; j++) {
    digitalWrite(LED_BUILTIN, LED_ON);
    delay(100);
    digitalWrite(LED_BUILTIN, LED_OFF);
    delay(100);
  }
  delay(2000);


  for (int j = 0; j < String(array[3]).length(); j++) {
    //Needed for double numbers --> 43 --> blick 4 --> delay --> delay 3
    for ( int k = 0; k < (int)array[3][j] - '0'; k++) {
      digitalWrite(LED_BUILTIN, LED_ON);
      delay(500);
      digitalWrite(LED_BUILTIN, LED_OFF);
      delay(500);
    }
    delay(1000);
  }

  // Blick fast --> end comunication
  for (int j = 0 ; j < 20; j++) {
    digitalWrite(LED_BUILTIN, LED_ON);
    delay(100);
    digitalWrite(LED_BUILTIN, LED_OFF);
    delay(100);
  }
  delay(3000);

}

void setup()
{
  
  Serial.begin(115200);
  Serial.println();
  wifiManager.autoConnect("Wemos-Rover");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LED_OFF);
  comunicateIp(WiFi.localIP().toString());

  //Setting up motors
  pinMode(motorA1, OUTPUT); // Inizializzazione del pin direzione del motore collegato al canale A - D8 - motorA1
  pinMode(motorA2, OUTPUT); // Inizializzazione del pin direzione del motore collegato al canale A - D7 - motorA2
  digitalWrite(motorA1, LOW);  // Impostazione della direzione “avanti” del motore collegato al canale A
  digitalWrite(motorA2, LOW);  // Impostazione della direzione “avanti” del motore collegato al canale A

  pinMode(motorB1, OUTPUT); // Inizializzazione del pin direzione del motore collegato al canale B - D6 - motorB1
  pinMode(motorB2, OUTPUT); // Inizializzazione del pin direzione del motore collegato al canale B - D5 - motorB2
  digitalWrite(motorB1, LOW);  // Impostazione della direzione “avanti” del motore collegato al canale B
  digitalWrite(motorB2, LOW);  // Impostazione della direzione “avanti” del motore collegato al canale B

  // Resetting telemetry
  telemetry["motors"][0]["motor"] = "A";
  telemetry["motors"][0]["direction"] = "-";
  telemetry["motors"][0]["power"] = 0;
  telemetry["motors"][1]["motor"] = "B";
  telemetry["motors"][1]["direction"] = "-";
  telemetry["motors"][1]["power"] = 0;
  telemetry["network"]["server"]["ip"] = WiFi.localIP().toString();
  telemetry["network"]["server"]["port"] = localUdpPort;
}


void loop()
{

  telemetry["motors"][0]["motor"] = "A";
  telemetry["motors"][0]["direction"] = "-";
  telemetry["motors"][0]["power"] = 0;
  telemetry["motors"][1]["motor"] = "B";
  telemetry["motors"][1]["direction"] = "-";
  telemetry["motors"][1]["power"] = 0;

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

    Serial.printf("Code: %d  Value: %d\n", code, value);

    Serial.printf("Stop motor LEFT/RIGHT\n");
    digitalWrite(motorB1, LOW);
    digitalWrite(motorB2, LOW);
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, LOW);

    if (code == 1 && value < 480 ) {
      Serial.printf("D\n");
      digitalWrite(motorA1, HIGH);
      digitalWrite(motorA2, LOW);
      digitalWrite(motorB1, HIGH);
      digitalWrite(motorB2, LOW);
      // Set telemetry data
      telemetry["motors"][0]["motor"] = "A";
      telemetry["motors"][0]["direction"] = "D";
      telemetry["motors"][0]["power"] = 1023;
    } else if (code == 1 && value > 550 ) {
      Serial.printf("R\n");
      digitalWrite(motorA1, LOW);
      digitalWrite(motorA2, HIGH);
      digitalWrite(motorB1, LOW);
      digitalWrite(motorB2, HIGH);
      // Set telemetry data
      telemetry["motors"][0]["motor"] = "A";
      telemetry["motors"][0]["direction"] = "R";
      telemetry["motors"][0]["power"] = 1023;
    }

    if (code == 0 && value < 480 ) {
      Serial.printf("LEFT\n");
      digitalWrite(motorB1, LOW);
      digitalWrite(motorB2, HIGH);
      digitalWrite(motorA1, HIGH);
      //digitalWrite(motorA1, LOW);
      digitalWrite(motorA2, LOW);
      // Set telemetry data
      telemetry["motors"][1]["motor"] = "B";
      telemetry["motors"][1]["direction"] = "L";
      telemetry["motors"][1]["power"] = 1023;
    } else if (code == 0 && value > 530 ) {
      Serial.printf("RIGHT\n");
      digitalWrite(motorB1, HIGH);
      //digitalWrite(motorB1, LOW);
      digitalWrite(motorB2, LOW);
      digitalWrite(motorA1, LOW);
      digitalWrite(motorA2, HIGH);
      // Set telemetry data
      telemetry["motors"][1]["motor"] = "B";
      telemetry["motors"][1]["direction"] = "R";
      telemetry["motors"][1]["power"] = 1023;
    }

    if (code == 1000 && value == 0) {
      digitalWrite(motorA1, HIGH);
      digitalWrite(motorA2, LOW);
      digitalWrite(motorB1, LOW);
      digitalWrite(motorB2, LOW);
      delay(1000);
      digitalWrite(motorA1, LOW);
      digitalWrite(motorA2, LOW);
      digitalWrite(motorB1, LOW);
      digitalWrite(motorB2, LOW);
    }
    if (code == 1000 && value == 1000) {
      digitalWrite(motorA1, LOW);
      digitalWrite(motorA2, LOW);
      digitalWrite(motorB1, LOW);
      digitalWrite(motorB2, LOW);
      delay(1000);
      digitalWrite(motorA1, LOW);
      digitalWrite(motorA2, LOW);
      digitalWrite(motorB1, HIGH);
      digitalWrite(motorB2, LOW);
    }

    Serial.printf("UDP packet contents: %s\n", incomingPacket);

    char replyPacket[1023];
    serializeJson(telemetry, replyPacket);
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(replyPacket);
    Udp.endPacket();
  }
}