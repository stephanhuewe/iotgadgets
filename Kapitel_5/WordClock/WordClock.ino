// Die nachfolgenden Libraries sind unbedingt erforderlich und müssen über
// die Arduino-Bibliotheksverwaltung eingebunden werden
// Diese sind für die Ansteuerung der RTC und der LED erforderlich
#include <Time.h>
#include <DS3231.h>
#include "Adafruit_WS2801.h"
#include "SPI.h" 

// Erstellt eine 24-Bit Farbe aus den R,G,B Werten
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

// Wichtig ist, die Ground-Leitung des Stripes mit der Ground-Leitung
// des Arduino zu verbinden. Die +5V Leitung muss mit dem externen Netzteil 
// verbunden werden.

// Nun legen wir fest, aus wie vielen LEDs unser Stripe besteht. Dies ist für
// die Adressierung wichtig. In unserem Fall sind es 100 Stück.

Adafruit_WS2801 strip = Adafruit_WS2801(100);

// Als n?chstes initialisieren wir die Realtime Clock
DS3231  rtc(SDA, SCL);
Time  t;

// Die Setup-Methode wird zu Beginn aufgerufen 
// und hier werden alle Initialisierungsvorg?nge durchgef?hrt

void setup() {
  rtc.begin();

  // Sollten wir die Zeit und das Datum initial einmal setzen müssen, können
  // wir das mit den folgenden Befehlen tun.
  // Diesen lassen wir aktuell auskommentiert
  // rtc.setTime(16, 18, 0);   // Damit setzen wir die Zeit auf 16:18:00 Uhr
  // rtc.setDate(24, 12, 2018); // Datum wird auf den 24.12.2018 gesetzt

  // Nun initialisieren wir den LED-Stripe und setzten alle LEDs auf den
  // Status aus
  strip.begin();
  strip.show();

  // Zum Schluss initialisieren wir noch unseren Zufallszahlen-Generator
  // Diesen benötigen wir für die zufällige Wahl der Farbe, die die LEDs
  // anzeigen werden
  randomSeed(42); 

}

// Füllt alle LEDs mit der gesetzten Farbe
void colorWipe(uint32_t c, uint8_t wait) 
{
   int i;
   for (i = 0; i < strip.numPixels(); i++) 
   {
     strip.setPixelColor(i, c);
     strip.show();
     delay(wait);
   }
}
  
void show(int array[], int size)
{
   int i, j;
   int r,g,b;

   // Die dargestellte Farbe besteht aus zufälligen RGB-Werten
   r = random(0,255);
   g = random(0,255);
   b = random(0,255);

   // Hier werden die einzelnen LEDs mit der Farbe vorbelegt
   for (i = 0; i < size; i++) 
   {
       strip.setPixelColor(array[i] - 1, Color(r, g, b));
   }
   
   // und nachfolgend ausgesteuert  
   strip.show();   
   delay(100)  ;
}
  

void ShowTime(int hour, int minute)
{
  // Nachfolgend haben wir alle Worte in Form von LED-Positionen in Arrays
  // gespeichert, je nachdem wie ihr den LED-Strang verlegt habt, können
  // sich hier Abweichungen ergeben
  int es[] = {1, 2};
  int ist[] = {4, 5, 6};
  int fuenf[] = {8, 9, 10, 11};
  int zehn[] = {19, 20, 21, 22};
  int zwanzig[] = {12, 13, 14, 15, 16, 17, 18};
  int drei[] = {23, 24, 25, 26};
  int viertel[] = {27, 28, 29, 30, 31, 32, 33};
  int dreiviertel[] = {23, 24, 25, 26, 27, 28, 29, 30, 31, 23, 33};
  int nach[] = {37, 38, 39, 40};
  int vor[] = {34, 35, 36};
  int halb[] = {41, 42, 43, 44};
  int zwoelf[] = {45, 46, 47, 48,49};
  int zwei[] = {57, 58, 59, 60};
  int ein[] = {56, 57,58};
  int eins[] = {55, 56, 57,58};
  int sieben[] = {50, 51, 52, 53, 54,55};
  int drei2[] = {61, 62, 63, 64}; 
  int fuenf2[] = {65, 66, 67,68};
  int elf[] = {77, 78, 79};
  int neun[] = { 73, 74, 75,76};
  int vier[] = {69, 70, 71,72};
  int acht[] = {81, 82, 83,84};
  int zehn2[] = {85, 86, 87,88};
  int sechs[] = {96,97,98,99,100};
  int uhr[] = {91, 92, 93};
  
  // Zunächst schalten wir alles aus
  colorWipe(Color(0,0,0),0);
  
  // Der Text -- es ist -- wird immer angezeigt
  // Zur Anzeige eines Wortes gibt es die Funktion show
  // Diese Funktion erwartet das Wort und die Anzahl der LEDs,
  // die beleuchtet werden sollen
  show(es,2);
  show(ist,3);
  
  // Sofern wir uns innerhalb der ersten fünf Minuten befinden zeigen wir 
  // das Wort Uhr an. Werten es also als volle Stunde.
  if (minute >= 0 && minute < 5)
  {
    show(uhr,3); 
  }
  
  // Ab der Minute fünf bis zur zehnten zeigen wir fünf nach an
  if (minute >=5 && minute < 10)
  {
    show(fuenf,4);
    show(nach,4);
  }
  
  // Zwischen Minute 10 und bis 15 gilt zehn nach
  if (minute >= 10 && minute < 15)
  {
    show(zehn,4);
    show(nach,4);
  }
  
  // Zwischen Minute 15 und 20 gilt viertel nach
  if (minute >= 15 && minute < 20)
  {
    show(viertel,7);
    show(nach,4);
  }
  
  // Zwischen 20 und 25 stellen wir zwanzig nach dar
  if (minute >=20 && minute < 25)
  {
    show(zwanzig,7);
    show(nach,4);
  }
  
  // Fünf vor halb, wichtig ist, dass wir hier die Stunde weiterschieben
  if (minute >= 25 && minute < 30)
  {
    show(fuenf,4);
    show(vor,3);
    show(halb,4);
  
      hour++;
  }
  
  // Von 30 bis 35 gilt als halb - ebenfalls mit einer Stunde mehr
  if (minute >= 30 && minute < 35)
  {
    show(halb,4);
    hour++;
  }
  
  // Fünf nach halb
  if (minute >= 35 && minute < 40)
  {
    show(fuenf,4);
    show(nach,4);
    show(halb,4);
    hour++;
  }
  
  // Zwanzig vor
  if (minute >= 40 && minute < 45)
  {
    show(zwanzig,7);
    show(vor,3);
    hour++;
  }
  
  // Viertel vor
  if (minute >= 45 && minute < 50)
  {
    show(viertel,7);
    show(vor,3);
    hour++;
  }
  
  // Zehn vor
  if (minute >= 50 && minute < 55)
  {
    show(zehn,4);
    show(vor,3);
    hour++;
  }
  
  // F?nf vor
  if (minute >= 55 && minute < 59)
  {
    show(fuenf,4);
    show(vor,3);
    hour++;
  }
  
  // Nachfolgend wird die Anzeige der Stunden durchgeführt
  switch(hour)
  {
    case 0:
    case 12:
        show(zwoelf,5);
        break;

    case 1:
    case 13:
        if (minute <5)
        {
             show(ein,3);
        }
        else
        {
        show(eins,4);
        }
        break;

    case 2:
    case 14:
        show(zwei,4);
        break;

    case 3:
    case 15:
        show(drei2,4);
        break;

    case 4:
    case 16:
        show(vier,4);
        break;

    case 5:
    case 17:
        show(fuenf2,4);
        break;

    case 6:
    case 18:
        show(sechs,5);
        break;

    case 7:
    case 19:
        show(sieben,6);
        break;

    case 8:
    case 20:
        show(acht,4);
        break;

    case 9:
    case 21:
        show(neun,4);
        break;

    case 10:
    case 22:
        show(zehn2,4);
        break;

    case 11:
    case 23:
       show(elf,3);
       break;
  }
}


// Nun folgt die Hauptschleife, in der die gesamte Logik als Endlos-Schleife
// abgearbeitet wird
void loop() {

  // Zun?chst holen wir die aktuelle Uhrzeit ab und zerlegen diese in
  // Stunden und Minuten
  String myTime = rtc.getTimeStr();
  Serial.println("Uhrzeit : " + myTime);
  int hour = myTime.substring(0,2).toInt();
  int minute = myTime.substring(3,5).toInt();

  Serial.println(hour);
  Serial.println(minute);

  // Das ist unsere Anzeige-Funktion
  ShowTime(hour,minute);

  // 60 Sekunden warten, damit die n?chste Minutenanzeige durchgeführt wird
  delay (60000);
}
