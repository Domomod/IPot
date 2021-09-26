from logging import debug
import logging
from flask import Flask, render_template, request
from flask.globals import session
from flask_socketio import *
import paho.mqtt.client as mqtt

import json
import random
from copy import deepcopy
from datetime import datetime

class NoPingPongFilter(logging.Filter):
  def filter(self, record):
    return not ('Received packet PONG' in record.getMessage() or
                'Sending packet PING' in record.getMessage())


app = Flask(__name__)
logging.getLogger('engineio.server').addFilter(NoPingPongFilter())
socketio = SocketIO(app, async_mode="eventlet", logger=False, engineio_logger=True)
socketio.init_app(app, cors_allowed_origins="*")
client = mqtt.Client() 
topic_intro = "intro"
topic_status = "server/status"

class Observer(object):
    def __init__(self):
        self.observers = []
        
        def register(self, fn):
            self.observers.append(fn)
            return fn

        def notify_observers(self, *args, **kwargs):
            for fn in self.observers:
                fn(*args, **kwargs)
observer = Observer()

@app.after_request
def add_header(r):
    r.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    r.headers["Pragma"] = "no-cache"
    r.headers["Expires"] = "0"
    r.headers['Cache-Control'] = 'public, max-age=0'
    return r

@app.route('/')
def index():
    return render_template('index.html')

def on_connect(client, userdata, flag, rc):
    if rc == 0:
        print("Connected success")
        client.subscribe(topic_intro)
    else:
        print(f"Connected fail with code {rc}")

known_sensors = dict()
default_sensor = {
    'temp': 0.0,
    'humidity': 0.0,
    'water': 0.0,
    'light': 0.0,
    'color': "#"
}

def on_recieve(client, userdata, msg):
    global sensor_data
    m_decode=str(msg.payload.decode("utf-8","ignore"))
    if(msg.topic == topic_intro):
        try:
            introduction_msg = json.loads(m_decode)
            sensor_mac = introduction_msg["clientMac"]
            client.subscribe(sensor_mac)

            default_sensor['color'] = "#"+''.join([random.choice('0123456789ABCDEF') for j in range(6)])
            known_sensors[sensor_mac] = deepcopy(default_sensor)
            print(f"Known sensors are: {', '.join(known_sensors)}")
        except Exception as e:
            print(f"[  EXCEPITON]: While handling mqtt introduction msg: {e}")
    else:
        try:
            color = known_sensors[msg.topic]['color']
            sensor_data = json.loads(m_decode)
            sensor_data['color'] = color
            sensor_data['light'] =  "Light" if sensor_data['light'] < 1.0 else "Dark"  
            known_sensors[msg.topic] = sensor_data
        except Exception as e:
            print(f"[  EXCEPITON]: While handling mqtt sensor data: {e}")


# SocketIO Events
@socketio.on('connect')
def connected():
    print(
    f"""-------------------------------\n
    [       INFO]Socket Connected {datetime.now().strftime('%H:%M:%S')}
    \n-------------------------------""")
    emit('my response', {'data': 'Hi client. This is server'})

# SocketIO Events
@socketio.on('message')
def handle_msg(data):
    print(f'Recieved message from {request.sid}: {data}')

# SocketIO Events
@socketio.on('sensor-request')
def handle_sensor_request(data):
    print(f'Recieved sensor-request from {request.sid}')
    emit('sensor-update', list(known_sensors.values()))

@socketio.on('disconnect')
def disconnected():
    print(
    f"""-------------------------------\n
    [ !IMPORTANT]Socketio Disconnected {datetime.now().strftime('%H:%M:%S')}
    \n-------------------------------""")



if __name__ == '__main__':
    client.on_connect = on_connect 
    client.on_message = on_recieve
    client.connect("localhost", 1883, 60) 
    client.loop_start()
    client.publish(topic_status, "Server up")

    #log_output=None disables irrelevant POST and GET logs
    socketio.run(app, host="your-ip-address", port=5000, debug = True, log_output=None) 
    
    #socketio.run(app, host="your-ip-address", port=5000, debug = True, keyfile='./creditentials/host.key', certfile='./creditentials/host.cert')
