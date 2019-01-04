# Über den System-Befehl setzen wir ein System-Kommando ab
# In diesem Fall schreiben wir eine 1 auf den Pin 23 / GPIO 11 bzw. 14 unter
# WiringPi
# 
# Wir setzen den Pin11 und 14 in den Ausgangsmodus
# An der 14 hängt unsere Lichterkette
# An der 11 unser Motor
system ("gpio mode 14 out");
system ("gpio mode 11 out");

# Wir schalten beide zur Sicherheit aus
# 1 Bedeutet in unserem Fall aus, da das Relais auf aus schaltet
system ("gpio write 14 1"); 
system ("gpio write 14 0"); 

# in einer Schleife
do{
	# Ermitteln wir zunächst den Tag
	$tag=(localtime(time))[3];
	# Sollte der Tag „einstellig“ sein, so ergänzen wir vorne eine 0
	$tag = $tag < 10 ? $tag = "0".$tag : $tag; 
	
	# Kontrollausgabe
  print $tag;
	print "Warte auf Taster";
	
	# In einer Schleife warten wir nun darauf
	# Dass der Taster, der an Pin 10 liegt, gedrückt wird
	do{
			open($t,"gpio read 10 |");
			$i=<$t>;sleep 1;
		}while($i=~m/1/);
 	
	# Nun schalten wir das Licht an
	system ("gpio write 14 0");
	# Kontrollausgabe
	print "Aktueller Tag: $tag\n";
	# Start des MP3 des Tages, im Hintergrund
	# Dies wird durch das & geregelt, damit läuft das Hauptprogramm weiter
	system ("nohup mpg123 ./musik/$tag* &");
	# Kontrollausgabe
	print "Musik gertartet\n";
	# Nun starten wir den Motor und lassen ihn 10 Sekunden laufen
	system ("gpio write 11 0");
	sleep 10;
	# Und schalten ihn wieder ab
	system ("gpio write 11 1");
  # Am Ende schalten wir das Licht wieder aus
	system ("gpio write 14 1");
# Nur bis zum 24. Tag = Heilig Abend
}while($tag<24);
