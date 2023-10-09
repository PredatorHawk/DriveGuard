#include <SoftwareSerial.h>

#define RX 7
#define TX 6
#define irSensor 3
#define buzzerPin 9
#define ledPin 5

SoftwareSerial esp8266(RX, TX);

const char* ssid = "Omswar";
const char* password = "00193057";

unsigned long startTime = 0;
bool monitoringStarted = false;
bool drowsyAlertSent = false;

void setup() {
  Serial.begin(9600);
  esp8266.begin(115200);
  pinMode(irSensor, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  connectToWiFi();
  
  sendAlert("Alert !! Monitoring Started Now.\nVehicle No: MH 12 PZ 7021");
  monitoringStarted = true;
}

void loop() {
  int valSensor = digitalRead(irSensor);

  if (valSensor == 1) {
    if (!drowsyAlertSent) {
      startTime = millis();
      drowsyAlertSent = true;
    }
    else if (millis() - startTime >= 3000) {
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(ledPin, HIGH);
      sendAlert("Danger !! Driver is drowsing !!!\nVehicle No: MH 12 PZ 7021");
      drowsyAlertSent = false;
    }
  }
  else {
    drowsyAlertSent = false;
    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);
  }

  delay(1000); // Adjust the delay time as needed
}

void connectToWiFi() {
  esp8266.println("AT+CWJAP=\"" + String(ssid) + "\",\"" + String(password) + "\"");
  delay(2000);
  while (!esp8266.find("OK")) {
    Serial.println("Failed to connect to WiFi. Retrying...");
    delay(2000);
    esp8266.println("AT+CWJAP=\"" + String(ssid) + "\",\"" + String(password) + "\"");
    delay(2000);
  }
  Serial.println("Connected to WiFi!");
   // Beep twice when connected to WiFi
  digitalWrite(buzzerPin, HIGH);
  delay(1000);
  digitalWrite(buzzerPin, LOW);
  delay(1000);
  digitalWrite(buzzerPin, HIGH);
  delay(1000);
  digitalWrite(buzzerPin, LOW);
}

void sendAlert(const char* message) {
  Serial.println("Sending Alert: ");
  Serial.println(message);

  esp8266.println("AT+CIPSTART=\"TCP\",\"117.248.110.41\",9999");
  delay(2000);
  esp8266.println("AT+CIPSEND=" + String(strlen(message)));
  delay(2000);
  esp8266.println(message);
  delay(2000);
  esp8266.println("AT+CIPCLOSE");
}
