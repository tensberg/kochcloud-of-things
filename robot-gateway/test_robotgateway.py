import unittest
from unittest.mock import Mock
from robobuf import gateway_to_robot_message_pb2, robot_to_gateway_message_pb2
import robotgateway

class TestRobotGateway(unittest.TestCase):
    def test_send_message_to_robot(self):
        # given
        robotgateway.transfer = Mock()
        dummy_rec_size = 11
        robotgateway.transfer.tx_obj = Mock(return_value=dummy_rec_size)
        topic = 'show-text'
        json = b'{"text": "foo", "color": 123}'

        # when
        robotgateway.send_message_to_robot(topic, json)

        # then
        gateway_to_robot = gateway_to_robot_message_pb2.GatewayToRobotMessage()
        gateway_to_robot.show_text.text = 'foo'
        gateway_to_robot.show_text.color = 123
        expected_message_body_bytes = gateway_to_robot.SerializeToString()

        robotgateway.transfer.tx_obj.assert_any_call(len(expected_message_body_bytes), 0, val_type_override='H')
        robotgateway.transfer.tx_obj.assert_called_with(expected_message_body_bytes, dummy_rec_size, val_type_override='9s')
        robotgateway.transfer.send.assert_called_with(dummy_rec_size, robotgateway.GatewayToRobotMessageType.MESSAGE)

    def test_mqtt_subscribe(self):
        # given
        robotgateway.mqttc = Mock()
        robotgateway.mqttc.subscribe = Mock()

        # when
        robotgateway.mqtt_subscribe()

        # then
        # only test a subset of the topics so that the test does not break when new topics are added
        robotgateway.mqttc.subscribe.assert_any_call(robotgateway.MQTT_ROOT_TOPIC + 'show-text')
        robotgateway.mqttc.subscribe.assert_any_call(robotgateway.MQTT_ROOT_TOPIC + 'leds')

    def test_publish_robot_message_handler(self):
        # given
        messageString = b'\n\r\x08\x01\x12\x03foo\x18\x02 \x03(\x04'
        robotgateway.transfer = Mock()
        robotgateway.transfer.rx_obj = Mock()
        robotgateway.transfer.rx_obj.side_effect = [len(messageString), messageString]
        robotgateway.mqtt_publish = Mock()

        # when
        robotgateway.publish_robot_message_handler()

        # then
        robotgateway.mqtt_publish.assert_called_with('display/state', '{\n  "backgroundColor": 1,\n  "image": "foo",\n  "x": 2,\n  "y": 3,\n  "brightness": 4\n}')


    def test_publish_robot_message_handler_send_attributes_with_default_values(self):
        # given
        message = robot_to_gateway_message_pb2.RobotToGatewayMessage()
        message.display_state.background_color = 1
        messageString = message.SerializeToString()
        robotgateway.transfer = Mock()
        robotgateway.transfer.rx_obj = Mock()
        robotgateway.transfer.rx_obj.side_effect = [len(messageString), messageString]
        robotgateway.mqtt_publish = Mock()

        # when
        robotgateway.publish_robot_message_handler()

        # then
        robotgateway.mqtt_publish.assert_called_with('display/state', '{\n  "backgroundColor": 1,\n  "x": 0,\n  "y": 0,\n  "brightness": 0\n}')


if __name__ == '__main__':
    unittest.main()