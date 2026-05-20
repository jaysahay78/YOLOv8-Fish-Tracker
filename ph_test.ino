#define BLYNK_TEMPLATE_ID "TMPL3RlJP_Ot3"
#define BLYNK_TEMPLATE_NAME "Aquarium Monitor"
#define BLYNK_AUTH_TOKEN "5btDQHQY9houBheOCanB_vDngvnqZ_rt"

#include <WiFi.h>
#include <WebServer.h>
#include <BlynkSimpleEsp32.h>

#define TDS_PIN 34

HardwareSerial sensorSerial(2);

char ssid[] = "Jay's Wifi";
char pass[] = "02072003";

WebServer server(80);

String incomingLine = "";

float phValue = 0.0;
float temp = 0.0;
int tds = 0;

void handleData() {

  String json = "{";
  json += "\"ph\":" + String(phValue) + ",";
  json += "\"temp\":" + String(temp) + ",";
  json += "\"tds\":" + String(tds);
  json += "}";

  server.send(200, "application/json", json);
}

void setup() {

  Serial.begin(115200);

  sensorSerial.begin(9600, SERIAL_8N1, 16, 17);

  Serial.println("Sensor Started");

  WiFi.begin(ssid, pass);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  server.on("/data", handleData);

  server.begin();

  Serial.println("Web server started");
}

void loop() {

  while(sensorSerial.available()) {

    char c = sensorSerial.read();

    Serial.write(c);

    if(c == '\n' || c == '\r') {

      // Parse PH
      int phIndex = incomingLine.indexOf("PH:");
      if(phIndex != -1) {

        int commaIndex = incomingLine.indexOf(",", phIndex);

        String phString = incomingLine.substring(phIndex + 3, commaIndex);

        phValue = phString.toFloat();
      }

      // Parse Temperature
      int tempIndex = incomingLine.indexOf("T:");
      if(tempIndex != -1) {

        String tempString = incomingLine.substring(tempIndex + 2);

        temp = tempString.toFloat();
      }

      incomingLine = "";
    }
    else {

  if(c >= 32 && c <= 126) {
    incomingLine += c;
  }

  if(incomingLine.length() > 80) {
    incomingLine = "";
  }
}
  }

  static unsigned long lastRead = 0;

  if(millis() - lastRead > 1000) {

    tds = analogRead(TDS_PIN);

    Serial.print(" | Raw TDS Value: ");
    Serial.println(tds);

    // Send REAL values to Blynk
    Blynk.virtualWrite(V0, phValue);
    Blynk.virtualWrite(V1, tds);
    Blynk.virtualWrite(V2, temp);

    Serial.print("PH Value: ");
    Serial.println(phValue);

    Serial.print("Temperature: ");
    Serial.println(temp);

    lastRead = millis();
  }

  Blynk.run();

  server.handleClient();
}