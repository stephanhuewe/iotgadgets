# Die nachfolgenden Libraries können bspw. über pip nachinstalliert werden
# Benötigt für den Zugriff auf den seriellen Port
import serial
# Regex benötigten wir zum parsen der Benutzernamen
import re
# Urllib2 hilft uns beim Aufruf der NUKI-Api
import urllib2

# Zunächst verbinden wir uns mit unserem seriellen Wiegand Device
ser = serial.Serial('/dev/ttyUSB0',9600)

# Einfaches Textfile, Aufbau siehe weiter unten
dbfile = 'database.txt'

# Diese Funktion öffnet unsere „Benutzerdatenbank“ und prüft, ob der
# Fingerabdruck darin enthalten ist
def CheckUserExists(user):
       with open(dbfile, 'r') as file:
            if re.search('^{0}$'.format(re.escape(user)), file.read(), flags=re.M):
                return True
            else:
              return False

# In einer Endlosschleife prüfen wir nun, ob etwas über den Fingerprinter
# ankommt
while True:
    # Wir lesen 7 Byte
    reading = ser.read(7)

    # Schneiden mögliche Leerzeichen weg
    reading = reading.strip()

    # Leeren den Einlesepuffer
    ser.flush()

    # Zur Sicherheit geben wir das Gelesene auf der Konsole aus
    print(reading)

    # Wenn der Benutzer gefunden wurde, öffnen wir die Türe
    if CheckUserExists(reading):

        # Mit diesem Aufruf stellen wir das Schloss auf „offen“, sofern
        # abgeschlossen war
        f = urllib2.urlopen('http://xxx:8080/lockAction?nukiId=4711&action=1&token=abc')

        # Ausgabe der Rückgabe
        print(f.read(100))

        g = urllib2.urlopen('http://xxx:8080/lockAction?nukiId=4711&action=3&token=abc')
        # Ausgabe der Rückgabe
        print(g.read(100))
