#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

const char* ssid = "ssid";
const char* password = "password";

WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets

DynamicJsonDocument sensor_data(1024);
DynamicJsonDocument telemetry(1024);

int motorA1 = 13;
int motorA2 = 15;

int motorB1 = 12;
int motorB2 = 14;

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
