#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_AM2320.h>

#define ssid "WLAN"
#define password "WLAN_PASSWORT!"
#define mqtt_server "MQTT_SERVER"
#define mqtt_user "MQTT_USER" /* NULL für kein USER*/
#define mqtt_password "dustu7" /* NULL für kein Passwort*/
#define mqtt_client "zimmer_1"
#define topic_temp "sensors/zimmer_1/temp"
#define topic_hum "sensors/zimmer_1/hum"

Adafruit_AM2320 am2320;
WiFiClient espClient; //WiFiClientSecure falls TLS gewünscht ist
PubSubClient client(espClient);
char msg[10];

// Bekannte Wifi Setup Routine
void setup_wifi()
{
  delay(10);
  Serial.print(F("Connecting to "));
  Serial.println(ssid);
  // Nur Station Mode
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  // Loop bis wir connected sind.
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(F("."));
  }
  randomSeed(micros());
  Serial.println(F(""));
  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200); // Serielle Konsole Initialisieren
  Wire.begin(D1, D2);   // SDA->D1, SCL ->D2
  am2320.begin();       // Sensor starten
  setup_wifi();         // Wifi an
  client.setServer(mqtt_server, 1883); // MQTT+TLS= Port 8883

  if (client.connect(mqtt_client, mqtt_user, mqtt_password)) {
    dtostrf(am2320.readTemperature(), 3, 2, msg); //Wert in String umwandeln.
    Serial.print(F("Temp: "));
    Serial.println(msg);
    client.publish(topic_temp, msg, true);
    client.loop();

    dtostrf(am2320.readHumidity(), 3, 2, msg);
    Serial.print(F("Hum: "));
    Serial.println(msg);
    client.publish(topic_hum, msg, true);
    client.loop();

    client.disconnect();
    delay(1);
  }
  Serial.println("bye bye!");
  WiFi.disconnect();   // Vom Wifi abmelden
  WiFi.mode(WIFI_OFF); // Wifi explizit ausschalten
  delay(10);
  ESP.deepSleep(55UL * 1000UL * 1000UL); // Update alle 55+5 Sekunden
}

void loop(){ }
