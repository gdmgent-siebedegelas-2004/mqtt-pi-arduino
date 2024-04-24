from flask import Flask, render_template
import paho.mqtt.client as mqtt
 
app = Flask(__name__)

# MQTT settings
mqtt_server = "127.0.0.1"
mqtt_topic = "arduino/test"
 
# MQTT callback functions
def on_connect(client, userdata, flags, rc, properties):
    print("Connected with result code "+str(rc))
    client.subscribe(mqtt_topic)
 
def on_message(client, userdata, msg):
    print(msg.payload.decode())
    global last_message
    last_message = msg.payload.decode()
 
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, transport='websockets')
client.on_connect = on_connect
client.on_message = on_message
 
client.connect(mqtt_server, 9001, 60)
 
last_message = "No messages yet"
 
@app.route("/")
def index():
    return render_template("index.html", message=last_message)
 
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5678, debug=True)
    client.loop_forever()
