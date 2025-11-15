import json
import random
import threading
import time
from pySerialTransfer import pySerialTransfer as SerialTransfer
from enum import Enum, IntEnum
import paho.mqtt.client as mqtt
import os
from google.protobuf import json_format
from caseconverter import kebabcase, snakecase
from robobuf import gateway_to_robot_message_pb2, robot_to_gateway_message_pb2
from kochcloud_of_things_devices import KOCHCLOUD_OF_THINGS_DEVICES

SERIAL_PORT = os.environ.get('GATEWAY_ROBOT_SERIAL_PORT', '/dev/ttyUSB0')

MQTT_SERVER = os.environ.get('MQTT_SERVER', 'localhost')
MQTT_USERNAME = os.environ.get('MQTT_USERNAME', 'robot-gateway')
MQTT_CLIENT = os.environ.get('MQTT_CLIENT', 'robot-gateway')
MQTT_ROOT_TOPIC = os.environ.get('MQTT_ROOT_TOPIC', 'gateway-robot/devices/')
MQTT_PASSWORD = os.environ.get('MQTT_PASSWORD') # no default value
MQTT_INITIALIZED_TOPIC = "robot/initialized"
MQTT_DISCOVERY_PREFIX = os.environ.get('MQTT_DISCOVERY_PREFIX', 'homeassistant')
MQTT_HOMEASSISTANT_STATUS_TOPIC = os.environ.get('MQTT_HOMEASSISTANT_STATUS_TOPIC', 'homeassistant/status')
MQTT_HOMEASSISTANT_STATUS_ONLINE = os.environ.get('MQTT_HOMEASSISTANT_STATUS_ONLINE', 'online')

transfer = None
mqttc = None

# Home Assistant MQTT Discovery publishing
# 
def publish_homeassistant_discovery():
    """
    Publish Home Assistant MQTT Discovery messages for all devices and their components.
    """
    for device in KOCHCLOUD_OF_THINGS_DEVICES:
        topic = f"{MQTT_DISCOVERY_PREFIX}/device/{device.id}/config"
        payload = device.discovery_json(MQTT_ROOT_TOPIC)
        mqttc.publish(topic, json.dumps(payload))

def init_robot_message_handler():
    print("received init message")
    mqtt_subscribe()
    mqtt_publish_initialized()

def publish_robot_message_handler():
    """
    Serial callback which reads a message from the robot and publishes it to MQTT.
    """
    try:
        payload_length = transfer.rx_obj(obj_type='H', start_pos = 0)
        rec_size = SerialTransfer.STRUCT_FORMAT_LENGTHS['H']
        payload = transfer.rx_obj(obj_type=list, start_pos = rec_size, obj_byte_size = payload_length, list_format='B')
        message = robot_to_gateway_message_pb2.RobotToGatewayMessage()
        message.ParseFromString(bytes(payload))
        message_type = message.WhichOneof('message')
        topic = kebabcase(message_type).replace('-state', '/state')
        message_body = json_format.MessageToJson(getattr(message, message_type), always_print_fields_with_no_presence=True)
        print('received publish message from robot for topic {}'.format(topic))
        mqtt_publish(topic, message_body)
    except Exception as e:
        print('failed to unpack message from robot: {}'.format(e))

def mqtt_subscribe():
    mqtt_subscribe_robot()
    mqtt_subscribe_homeassistant_discovery()

def mqtt_subscribe_robot():
    """
    Subscribe to all MQTT topics corresponding to GatewayToRobotMessage message fields.
    Those correspond to commands which the robot can execute.
    """
    message = gateway_to_robot_message_pb2.GatewayToRobotMessage()
    fields = message.DESCRIPTOR.fields
    # a MQTT topic corresponds to one of the fields in the GatewayToRobotMessage.message oneof
    topics = [kebabcase(field.name) for field in fields if field.containing_oneof.name == 'message']
    print('subscribing to gateway-robot MQTT topics "{}"'.format(topics))
    for topic in topics:
        mqttc.subscribe(MQTT_ROOT_TOPIC + topic)

def mqtt_subscribe_homeassistant_discovery():
    """
    Subscribe to Home Assistant MQTT Discovery status topic to detect when Home Assistant restarts.
    In that case, re-publish all discovery messages.
    """
    print('subscribing to Home Assistant status MQTT topic "{}"'.format(MQTT_HOMEASSISTANT_STATUS_TOPIC))
    mqttc.subscribe(MQTT_HOMEASSISTANT_STATUS_TOPIC)

class RobotToGatewayMessageType(Enum):
    INIT = init_robot_message_handler
    MQTT_PUBLISH = publish_robot_message_handler

class GatewayToRobotMessageType(IntEnum):
    ACK = 0
    RESET = 1
    MESSAGE = 2

def send_command_to_robot(message_type: GatewayToRobotMessageType):
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

def send_message_to_robot(topic, message_body):
    """
    Send a message received from MQTT to the robot via SerialTransfer.
    The message body must be a JSON string corresponding to the protobuf message for the given topic.
    """
    gateway_to_robot = gateway_to_robot_message_pb2.GatewayToRobotMessage()
    try:
        # a MQTT topic corresponds to one of the fields in the GatewayToRobotMessage.message oneof
        message_attr = snakecase(topic)
        message = getattr(gateway_to_robot, message_attr)
        # the MQTT message body JSON string is converted to the corresponding protobuf message
        json_format.Parse(message_body, message)
    except Exception as e:
        print('failed to pack message body protobuf for topic {}: {}'.format(topic, e))
        return

    # the serialized protobuf message is packed into a SerialTransfer packet and sent to the robot
    message_body_bytes = gateway_to_robot.SerializeToString()
    rec_size = transfer.tx_obj(len(message_body_bytes), 0, val_type_override='H')
    rec_size = transfer.tx_obj(message_body_bytes, rec_size, val_type_override='%ds' % len(message_body_bytes))
    success = transfer.send(rec_size, GatewayToRobotMessageType.MESSAGE)
    if not success:
        print('failed to transfer message to robot for topic {}'.format(topic))

def init_serial_transfer():
    global transfer
    print('connecting to serial port {}'.format(SERIAL_PORT))
    transfer = SerialTransfer.SerialTransfer(SERIAL_PORT, restrict_ports=False)

    transfer.set_callbacks([init_robot_message_handler, publish_robot_message_handler])
    success = transfer.open()
    if not success:
        raise Exception('failed to connect to serial port {}'.format(SERIAL_PORT))

def mqtt_publish_initialized():
    mqtt_publish(MQTT_INITIALIZED_TOPIC, '{}')

def mqtt_publish(topic, message_body):
    mi = mqttc.publish(MQTT_ROOT_TOPIC + topic, message_body)
    mi.wait_for_publish()

def mqtt_connect_handler(client, userdata, flags, rc):
    if rc != 0:
        raise Exception('failed to connect to MQTT server with result code {}'.format(rc))

    print('connected to MQTT server')
    publish_homeassistant_discovery()
    send_reset_to_robot()

def mqtt_message_handler(client, userdata, msg):
    if msg.topic == MQTT_HOMEASSISTANT_STATUS_TOPIC:
        if msg.payload.decode('utf-8') == MQTT_HOMEASSISTANT_STATUS_ONLINE:
            print('Home Assistant came online, re-publishing discovery messages')
            # it is best practice to add a small random delay before re-publishing
            # to avoid flooding the MQTT server if many devices do this simultaneously
            delay = random.uniform(0, 3)
            threading.Timer(delay, lambda: publish_homeassistant_discovery()).start()
        return
    if msg.topic.startswith(MQTT_ROOT_TOPIC):
        topic = msg.topic[len(MQTT_ROOT_TOPIC):]
        print('received message from MQTT server for topic {}'.format(topic))
        send_message_to_robot(topic, msg.payload)
        return
    print('received message for unknown topic {}'.format(msg.topic))

def init_mqtt():
    global mqttc
    mqttc = mqtt.Client(client_id=MQTT_CLIENT)
    mqttc.on_connect = mqtt_connect_handler
    mqttc.on_disconnect = lambda client, userdata, rc: print('disconnected from MQTT server with result code {}'.format(rc))
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