# Communicatie tussen Raspberry Pi en Arduino

Deze repository bevat de code voor een project waarbij een Raspberry Pi communiceert met een Arduino via MQTT. De Raspberry Pi heeft een MQTT broker draaien en de Arduino is een client die berichten stuurt naar de broker.

Die berichten worden uitgelezen via MQTT.js en worden realtimeweergegeven op een webpagina. 

Om ook via de browser MQTT te kunnen gebruiken, wordt de Mosquitto broker geconfigureerd om ook websockets te ondersteunen.


## Installatie

### Raspberry Pi

1. Installeer de Mosquitto broker:

```
sudo apt-get install mosquitto
```

2. Installeer de Mosquitto client:

```
sudo apt-get install mosquitto-clients
```

3. Configureer de Mosquitto broker om websockets te ondersteunen

Daarvoor moet je eerst manueel een bestand maken in de map `/etc/mosquitto/conf.d/` met de naam `websockets.conf` en de volgende inhoud:

```
listener 1883
protocol mqtt
allow_anonymous true

listener 9001
protocol websockets
allow_anonymous true
```

Door toevoeging van deze configuratie, zal de Mosquitto broker luisteren op poort 1883 (de standaard MQTT poort) en op poort 9001 voor websockets.


De Arduino zal op poort 1883 berichten sturen naar de broker en de webpagina zal op poort 9001 berichten ontvangen via websockets.

4. Start de broker opnieuw op

Na het aanpassen van de configuratie moet je de Mosquitto broker herstarten:


```
sudo systemctl restart mosquitto
```

5. Installeer dependencies

```
pip install -r requirements.txt
```

## Starten

```
python app.py
```

Kijk na op welk IP-adres de Raspberry Pi draait en surf naar dat adres op poort 5678. Bijvoorbeeld: `http://192.168.40.20:5678`.

Bekijk de pagina in de browser en zie hoe de berichten van de Arduino realtime worden weergegeven.