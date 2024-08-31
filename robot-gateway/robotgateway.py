import time
from pySerialTransfer import pySerialTransfer as SerialTransfer
from enum import Enum, IntEnum
import paho.mqtt.client as mqtt
import os
from google.protobuf import json_format
from caseconverter import kebabcase, snakecase
from robobuf import gateway_to_robot_message_pb2, robot_to_gateway_message_pb2

SERIAL_PORT = os.environ.get('GATEWAY_ROBOT_SERIAL_PORT', '/dev/ttyUSB0')

MQTT_SERVER = os.environ.get('MQTT_SERVER', 'localhost')
MQTT_USERNAME = os.environ.get('MQTT_USERNAME', 'robot-gateway')
MQTT_CLIENT = os.environ.get('MQTT_CLIENT', 'robot-gateway')
MQTT_ROOT_TOPIC = os.environ.get('MQTT_ROOT_TOPIC', 'gateway-robot/devices/')
MQTT_PASSWORD = os.environ.get('MQTT_PASSWORD') # no default value
MQTT_INITIALIZED_TOPIC = "robot/initialized"

transfer = None
mqttc = None

def init_robot_message_handler():
    print("received init message")
    mqtt_subscribe()
    mqtt_publish_initialized()

def publish_robot_message_handler():
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
    message = gateway_to_robot_message_pb2.GatewayToRobotMessage()
    fields = message.DESCRIPTOR.fields
    # a MQTT topic corresponds to one of the fields in the GatewayToRobotMessage.message oneof
    topics = [kebabcase(field.name) for field in fields if field.containing_oneof.name == 'message']
    print('subscribing to MQTT topics "{}"'.format(topics))
    for topic in topics:
        mqttc.subscribe(MQTT_ROOT_TOPIC + topic)

class RobotToGatewayMessageType(Enum):
    INIT = init_robot_message_handler
    MQTT_PUBLISH = publish_robot_message_handler

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

def send_message_to_robot(topic, message_body):
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
    send_reset_to_robot()

def mqtt_message_handler(client, userdata, msg):
    topic = msg.topic[len(MQTT_ROOT_TOPIC):]
    print('received message from MQTT server for topic {}'.format(topic))
    send_message_to_robot(topic, msg.payload)

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