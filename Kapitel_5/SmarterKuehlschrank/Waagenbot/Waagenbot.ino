#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Wire.h>
#include <HX711_ADC.h>
// Kalibrierungsfaktor und Tara Offset
#define calfactor 2169.0
#define tareoffset 136952560
// Wifi SSID und Password angeben
#define ssid "WLAN"
#define password "superSecret!"
// Token von BotFather
#define BOTtoken "682807999:AAG5xi3Y6HkJZ_9XXXEny5RnHv1QKUbJ1mM"
// Nur diese ID erhaelt den aktuellen Milchstand.
#define allowed_user_id "12345678"

// Bekannte Wifi Setup Routine
void setup_wifi()
{
  delay(10);
  Serial.println();
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
  char gewichtwert[20];
  int anzahlNeueNachrichten = 0;
  float gewicht = 0.0;
  String msg;
  WiFiClientSecure secclient;
  // Initialize Telegram BOT
  UniversalTelegramBot bot(BOTtoken, secclient);
  bot._debug = true;              // Debug Meldungen einschalten
  Serial.begin(115200);           // Serielle Konsole
   // Waage Initiailisieren: DOUT an Pin D6, SCLK an Pin D5, ggf anpassen.
  HX711_ADC Waage(D6, D5);
  Waage.begin();                  // Waage einschalten            
  Waage.start(1 * 1000);          // 2 sekunden warten, dann tarieren
  Waage.setCalFactor(calfactor);  // Ermittelten Kalibrierungsfaktor einstellen.
  Waage.setTareOffset(tareoffset);// Ermittelten Tare-offset einstellen.
  Waage.update(); // Zustand aktualisieren / Waage ablesen
  gewicht = Waage.getData();
  setup_wifi();                   // Wifi Einschalten
  Serial.println(millis());
  Serial.println(F("Setup abgeschlossen"));


  Serial.println(millis());
  // Nach neuen Nachrichten schauen
  anzahlNeueNachrichten = bot.getUpdates(bot.last_message_received + 1);
  while (anzahlNeueNachrichten) {
    Serial.print(F("Neue Nachrichten:"));
    Serial.println(anzahlNeueNachrichten);
    for (int i = 0; i < anzahlNeueNachrichten; i++) {
      // Nur Nachrichten von bekannten benutzern mit dem Inhalt "/gotmilk"
      if ((bot.messages[i].from_id == allowed_user_id) &&
          (bot.messages[i].text == "/gotmilk")) {
        dtostrf(gewicht,7,3,gewichtwert); // Zahl in Textumwandeln
        if (gewicht > 900) {
          msg = "Milch fast voll. Gewicht:";
        } else if (gewicht > 400) {
          msg = "Milch halb voll. Gewicht:";
        } else if (gewicht > 100) {
          msg = "Milch fast *leer*. Gewicht:";
        } else {
          msg = "Milch *leer* oder nicht im Kühlschrank!. Gewicht: ";
        }
        delay(10);
        bot.sendMessage(bot.messages[i].chat_id, msg + gewichtwert, "Markdown");
      } else if (bot.messages[i].text == "/start") {
           Serial.print(F("Got /start Message from "));
           Serial.print(bot.messages[i].from_name);
           Serial.print(F(" ("));
           Serial.print(bot.messages[i].from_id);
           Serial.print(F(" )"));
           bot.sendMessage(bot.messages[i].chat_id, 
              "Willkommen User mit ID: " + bot.messages[i].from_id, "Markdown");
      } else {
        Serial.print(F("Ignoring Message from Unknown user: "));
        Serial.println(bot.messages[i].from_id);
        bot.sendMessage(bot.messages[i].chat_id, 
              F("Sorry I only understand */gotmilk* from certain users"), "Markdown");
      }
    }
    anzahlNeueNachrichten = bot.getUpdates(bot.last_message_received + 1);
  }
  Serial.println(millis());
  Serial.println(F("Disconnecting"));
  WiFi.disconnect();       // Vom Wifi abmelden
  WiFi.mode(WIFI_OFF);     // Wifi explizit ausschalten
  WiFi.forceSleepBegin(0); // Wifi schlafen legen
  Serial.println(F("No Messages, going to sleep"));
  //Alles abschliessen lassen
  delay(10);
  Serial.println(millis());  
  ESP.deepSleep(10UL * 1000UL * 1000UL,
  RF_DISABLED); // Update alle 10 Sekunden
}
// Loop wird nie erreicht
void loop() { }
