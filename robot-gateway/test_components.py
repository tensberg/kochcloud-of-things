import unittest
import components

ROOT_TOPIC = "root/"

class DiscoveryJsonTestBase(unittest.TestCase):
    def setUp(self):
        self.device = components.Device(
            id="dev1",
            name="Test Device",
            state_topic="test/state",
            components=[]
        )

    def create_component(self, component_class, *args, **kwargs):
        comp = component_class(*args, **kwargs)
        comp.device = self.device
        return comp

    def check_discovery_json(self, comp, root_topic, expected):
        result = comp.discovery_json(root_topic)
        for key, value in expected.items():
            self.assertEqual(result[key], value)

class TestDeviceDiscoveryJson(DiscoveryJsonTestBase):
    def test_device_discovery_json_one_component(self):
        sensor = self.create_component(
            components.Sensor,
            id="temp",
            name="Temperature",
            device_class="temperature",
            unit_of_measurement="°C",
            value_path="temp.value",
            suggested_display_precision=1
        )
        device = components.Device(
            id="dev1",
            name="Test Device",
            state_topic="test/state",
            components=[sensor]
        )
        expected = {
            "dev": {
                "ids": "kochcloud-of-things-dev1",
                "name": "Test Device",
                "manufacturer": "Tensberg Industries",
            },
            "origin": {
                "name": "Robot Gateway"
            },
            "components": {
                "kochcloud-of-things-dev1_temp": sensor.discovery_json(ROOT_TOPIC) # use the actual value for the component, the component JSON is tested separately
            },
            "state_topic": "root/test/state"
        }
        result = device.discovery_json(ROOT_TOPIC)
        self.assertEqual(result, expected)

class TestSensorDiscoveryJson(DiscoveryJsonTestBase):
    def test_sensor(self):
        sensor = self.create_component(
            components.Sensor,
            id="temp",
            name="Temperature",
            device_class="temperature",
            unit_of_measurement="°C",
            value_path="temp.value",
            state_topic="test/component/state",
            suggested_display_precision=1
        )
        expected = {
            "platform": "sensor",
            "name": "Temperature",
            "value_template": "{{ value_json.temp.value }}",
            "unique_id": "kochcloud-of-things-dev1_temp",
            "state_topic": "root/test/component/state",
            "device_class": "temperature",
            "unit_of_measurement": "°C",
            "state_class": "measurement",
            "suggested_display_precision": 1
        }
        self.check_discovery_json(sensor, ROOT_TOPIC, expected)

class TestPowerSumDiscoveryJson(DiscoveryJsonTestBase):
    def test_power_sum(self):
        power_sum = self.create_component(components.PowerSum, "meter1", "Meter One")
        expected = {
            "platform": "sensor",
            "name": "Meter One Verbrauch",
            "value_template": "{{ value_json.meter1.power.sumWh }}",
            "unique_id": "kochcloud-of-things-dev1_meter1_power_sumwh",
            "device_class": "energy",
            "unit_of_measurement": "Wh",
            "state_class": "total",
            "suggested_display_precision": 1
        }
        self.check_discovery_json(power_sum, ROOT_TOPIC, expected)

class TestPowerCurrentDiscoveryJson(DiscoveryJsonTestBase):
    def test_power_current(self):
        power_current = self.create_component(components.PowerCurrent, "meter1", "Meter One")
        expected = {
            "platform": "sensor",
            "name": "Meter One Leistung",
            "value_template": "{{ value_json.meter1.power.currentW }}",
            "unique_id": "kochcloud-of-things-dev1_meter1_power_currentw",
            "device_class": "power",
            "unit_of_measurement": "W",
            "state_class": "measurement",
            "suggested_display_precision": 0
        }
        self.check_discovery_json(power_current, ROOT_TOPIC, expected)

class TestBinarySensorDiscoveryJson(DiscoveryJsonTestBase):
    def test_binary_sensor(self):
        binary_sensor = self.create_component(
            components.BinarySensor,
            id="door",
            name="Door",
            device_class="door",
            value_path="door.open"
        )
        expected = {
            "platform": "binary_sensor",
            "name": "Door",
            "value_template": "{{ \"ON\" if value_json.door.open else \"OFF\" }}",
            "unique_id": "kochcloud-of-things-dev1_door",
            "device_class": "door"
        }
        self.check_discovery_json(binary_sensor, ROOT_TOPIC, expected)

class TestSwitchDiscoveryJson(DiscoveryJsonTestBase):
    def test_switch(self):
        switch = self.create_component(
            components.Switch,
            id="led1",
            name="LED 1",
            value_property="user1",
            command_topic="test/devices/leds"
        )
        expected = {
            "platform": "switch",
            "name": "LED 1",
            "value_template": "{{ \"ON\" if value_json.user1 else \"OFF\" }}",
            "unique_id": "kochcloud-of-things-dev1_led1",
            "command_topic": "root/test/devices/leds",
            "command_template": "{{ {\"user1\": (value == \"ON\") } | tojson }}",
            "state_topic": "root/test/devices/leds/status"
        }
        self.check_discovery_json(switch, ROOT_TOPIC, expected)

class TestTextControlDiscoveryJson(DiscoveryJsonTestBase):
    def test_text_control(self):
        text_control = self.create_component(
            components.TextControl,
            id="text1",
            name="Text 1",
            value_property="text_value",
            command_topic="test/devices/text",
            state_topic="test/devices/text/state"
        )
        expected = {
            "platform": "text",
            "name": "Text 1",
            "unique_id": "kochcloud-of-things-dev1_text1",
            "command_topic": "root/test/devices/text",
            "command_template": "{{ {\"text_value\": value } | tojson }}",
            "state_topic": "root/test/devices/text/state",
            "value_template": "{{ value_json.text_value }}"
        }
        self.check_discovery_json(text_control, ROOT_TOPIC, expected)

class TestNumberControlDiscoveryJson(DiscoveryJsonTestBase):
    def test_number_control(self):
        number_control = self.create_component(
            components.NumberControl,
            id="num1",
            name="Number 1",
            value_property="num_value",
            value_min=0,
            value_max=100,
            command_topic="test/devices/number",
            state_topic="test/devices/number/state",
            step=5,
            mode="slider",
            unit_of_measurement="units"
        )
        expected = {
            "platform": "number",
            "name": "Number 1",
            "unique_id": "kochcloud-of-things-dev1_num1",
            "command_topic": "root/test/devices/number",
            "command_template": "{{ {\"num_value\": value } | tojson }}",
            "state_topic": "root/test/devices/number/state",
            "value_template": "{{ value_json.num_value }}",
            "min": 0,
            "max": 100,
            "step": 5,
            "mode": "slider",
            "unit_of_measurement": "units"
        }
        self.check_discovery_json(number_control, ROOT_TOPIC, expected)

class TestTriggerDiscoveryJson(DiscoveryJsonTestBase):
    def test_trigger(self):
        trigger = self.create_component(
            components.Trigger,
            id="trig1",
            name="Trigger 1",
            topic="test/devices/trigger",
            type="button_short_press",
            subtype="button1"
        )
        expected = {
            "platform": "device_automation",
            "name": "Trigger 1",
            "unique_id": "kochcloud-of-things-dev1_trig1",
            "automation_type": "trigger",
            "topic": "root/test/devices/trigger",
            "type": "button_short_press",
            "subtype": "button1"
        }
        self.check_discovery_json(trigger, ROOT_TOPIC, expected)
