#!/bin/bash 
image=“/tmp/licht.png“ 
# Bild aufnehmen 
raspistill -o $image 
# Helligkeit berechnen lassen | Kommazahlen abschneiden 
helligkeit=$(convert $image -format %[mean] info: | cut -d ‚.‘ -f 1) 
if [ $helligkeit -gt 30000 ]; then 
        echo „Licht ist an“; 
else 
        echo „Licht ist aus“; 
fi