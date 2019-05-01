// Kalibrierungs Programm für den HX711, basierend auf dem Beispiel von HX711_ADC
#include <HX711_ADC.h>
#include <Esp.h>

HX711_ADC Waage(D5, D6); //DOUT an Pin D5, SCLK an Pin D6
long t; //Variable zur Zeitmesssung

void setup() {
  Serial.begin(116200); // Serielle Schnittstelle initialisieren, 9600 Baud
  Serial.println("Bitte kurz abwarten, dass sich die Werte stabilisieren");
  Serial.println("Je hoeher der Kalibrierungsfaktor, desto Geringer wird das Gewicht");
  Serial.println("H = +10, h= +1, l= -1 , L = -10");
  
  Waage.begin(); // Waage einschalten
  wdt_disable(); // Watchdog Timer ausschalten.
  Waage.start(5*1000); // 5 sekunden warten, dann tarieren
  Waage.setCalFactor(2169.0); // Geschaetzten Kalibrierungsfaktor einstellen.
  Serial.println("Setup abgeschlossen");
  delay (1000);
}

void loop() {
  Waage.update(); // Zustand aktualisieren / Waage ablesen

  // Sind mehr als 250 ms seit dem letzten Auslesen vergangen geben wir den aktuellen Wert und Kalibrierungsfaktor aus
  if (millis() > t + 250) {
    Serial.print("Gewicht: ");
    Serial.print(Waage.getData());
    Serial.print(" Kalibierungsfaktor:  ");
    Serial.print(Waage.getCalFactor());
    Serial.print(" Tarierungs-Offset:  ");
    Serial.println(Waage.getTareOffset());
    t = millis();
  }

  if (Serial.available() > 0) { // Hat der Benutzer etwas eingegeben?
    float korrektur;
    char eingabe = Serial.read(); // Eingabe einlesen
    if (eingabe == 'l')      korrektur = -1.0;
    else if (eingabe == 'L') korrektur = -10.0;
    else if (eingabe == 'h') korrektur = 1.0;
    else if (eingabe == 'H') korrektur = 10.0;
    
    float neu = Waage.getCalFactor() + korrektur;
    Waage.setCalFactor(neu);
  }
}
