from flask import Flask, render_template
from flask_socketio import SocketIO
import paho.mqtt.client as mqtt

app = Flask(__name__)
socketio = SocketIO(app)

# MQTT settings
mqtt_server = "127.0.0.1"
mqtt_topic = "arduino/test"

# Correct way to create an MQTT client for VERSION2
mqtt_client = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2)

def on_connect(client, userdata, flags, rc, properties):
    print("Connected with result code " + str(rc))
    client.subscribe(mqtt_topic)

def on_message(client, userdata, msg):
    print(msg.payload.decode())
    socketio.emit('mqtt_message', {'data': msg.payload.decode()})

mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message

mqtt_client.connect(mqtt_server, 1883, 60)
mqtt_client.loop_start()

@app.route("/")
def index():
    return render_template('index.html')

if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=5678, debug=True)
