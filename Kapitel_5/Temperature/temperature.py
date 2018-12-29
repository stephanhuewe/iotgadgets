from Hologram.HologramCloud import HologramCloud
import Adafruit_DHT

credentials = {'devicekey': 'top_secret'}
hologram = HologramCloud(credentials, network='cellular')

result = hologram.network.connect()
if result == False:
    print ' Failed to connect to cell network'

sensor = Adafruit_DHT.DHT11
pin = 4
humidity, temperature = Adafruit_DHT.read_retry(sensor, pin)
humidity, temperature = Adafruit_DHT.read_retry(sensor, pin)

response_code = hologram.sendMessage("h:" + str(humidity) + "t:" + str(temperature))
print hologram.getResultString(response_code) # Prints 'Message sent successfully'.

hologram.network.disconnect()