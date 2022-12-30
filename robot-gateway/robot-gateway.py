import time
from pySerialTransfer import pySerialTransfer as SerialTransfer
from enum import Enum, IntEnum
import paho.mqtt.client as mqtt
import json
import msgpack
import os

SERIAL_PORT = os.environ.get('GATEWAY_ROBOT_SERIAL_PORT', '/dev/ttyUSB0')

MQTT_SERVER = 'kochcloud.local'
MQTT_USERNAME = 'robot-gateway'
MQTT_CLIENT = 'robot-gateway'
MQTT_ROOT_TOPIC = 'gateway-robot/devices/'
MQTT_PASSWORD = os.environ.get('MQTT_PASSWORD')

transfer = None
mqttc = None

def init_robot_message_handler():
    print("received init message")

def publish_robot_message_handler():
    rec_size = 0
    topic_length = transfer.rx_obj(obj_type='B', start_pos = rec_size)
    rec_size += SerialTransfer.STRUCT_FORMAT_LENGTHS['B']
    topic = transfer.rx_obj(obj_type=str, start_pos = rec_size, obj_byte_size = topic_length)
    rec_size += topic_length
    payload_length = transfer.rx_obj(obj_type='B', start_pos = rec_size)
    rec_size += SerialTransfer.STRUCT_FORMAT_LENGTHS['B']
    payload = transfer.rx_obj(obj_type=list, start_pos = rec_size, obj_byte_size = payload_length, list_format='B')
    rec_size += payload_length
    print('received publish message from robot for topic {}'.format(topic))
    messageBody = json.dumps(msgpack.unpackb(bytes(payload)))
    mqttc.publish(MQTT_ROOT_TOPIC + topic, messageBody)

def subscribe_robot_message_handler():
    rec_size = 0
    topic_length = transfer.rx_obj(obj_type='B', start_pos = rec_size)
    rec_size += SerialTransfer.STRUCT_FORMAT_LENGTHS['B']
    topic = transfer.rx_obj(obj_type=str, start_pos = rec_size, obj_byte_size = topic_length)

    print('received subscribe message for topic {} from robot'.format(topic))
    send_ack_to_robot()
    mqttc.subscribe(topic)

class RobotToGatewayMessageType(Enum):
    INIT = init_robot_message_handler
    PUBLISH = publish_robot_message_handler
    SUBSCRIBE = subscribe_robot_message_handler

class GatewayToRobotMessageType(IntEnum):
    ACK = 0
    RESET = 1
    MESSAGE = 2

def send_command_to_robot(message_type):
    transfer.tx_obj(0, 0, val_type_override='B') # dummy value because SerialTransfer packets must contain at least 1 byte payload
    transfer.send(1, message_type)

def send_ack_to_robot():
    send_command_to_robot(GatewayToRobotMessageType.ACK)
    print('sent ack message to robot')

def send_reset_to_robot():
    send_command_to_robot(GatewayToRobotMessageType.RESET)
    print('sent reset message to robot')
    # skip "garbage" bytes sent by ESP8266 after reset
    transfer.close()
    time.sleep(0.5)
    transfer.open()

def init_serial_transfer():
    global transfer
    print('connecting to serial port {}'.format(SERIAL_PORT))
    transfer = SerialTransfer.SerialTransfer(SERIAL_PORT, restrict_ports=False)
    
    transfer.set_callbacks([init_robot_message_handler, publish_robot_message_handler, subscribe_robot_message_handler])
    transfer.open()

def mqtt_connect_handler(client, userdata, flags, rc):
    print('connected to MQTT server with result code {}'.format(rc))
    send_reset_to_robot()

def mqtt_message_handler(client, userdata, msg):
    print('received message from MQTT server for topic {}'.format(msg.topic))

def init_mqtt():
    global mqttc
    mqttc = mqtt.Client(client_id='robot-gateway')
    mqttc.on_connect = mqtt_connect_handler
    mqttc.on_message = mqtt_message_handler
    mqttc.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
    mqttc.connect(MQTT_SERVER)

if __name__ == '__main__':
    try:
        init_serial_transfer()
        init_mqtt()
        
        while True:
            transfer.tick()
            mqttc.loop(timeout = 0.1)
    
    except KeyboardInterrupt:
        try:
            transfer.close()
        except:
            pass
    
    except:
        import traceback
        traceback.print_exc()
        
        try:
            transfer.close()
        except:
            pass