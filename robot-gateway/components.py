"""
Classes for devices and their contained components as used for Home Assistant MQTT Discovery.
See https://www.home-assistant.io/integrations/mqtt/#device-discovery-payload
"""

class Device:
    def __init__(self, id, name, state_topic, components):
        self._id = id
        self.name = name
        self.state_topic = state_topic
        # Flatten components if any are lists
        flat_components = []
        for comp in components:
            if isinstance(comp, list):
                flat_components.extend(comp)
            else:
                flat_components.append(comp)
        self.components = flat_components
        for component in self.components:
            component.device = self
    
    @property
    def id(self):
        return "kochcloud-of-things-" + self._id

    def discovery_json(self, root_topic):
        """
        Generate the MQTT Discovery JSON payload for the device and its contained components.

        Args:
            root_topic (str): The root MQTT topic for discovery messages. Prepended to component state topics.
        """
        discovery_info = {
            "dev": {
                "ids": self.id,
                "name": self.name,
                "manufacturer": "Tensberg Industries",
            },
            "origin": {
                "name": "Robot Gateway"
            },
            "components": {component.unique_id(): component.discovery_json(root_topic) for component in self.components},
            "state_topic": root_topic + self.state_topic
        }
        return discovery_info

class Component:
    device: Device

    def __init__(self, id, name, platform, device_class, value_path, state_topic=None, diagnostic=False):
        self.id = id
        self.name = name
        self.platform = platform
        self.device_class = device_class
        self.value_path = value_path
        self.state_topic = state_topic
        self.diagnostic = diagnostic

    def discovery_json(self, root_topic):
        json_data = {
            "platform": self.platform,
            "name": self.name,
            "value_template": "{{ value_json." + self.value_path + " }}",
            "unique_id": self.unique_id(),
        }
    
        if self.state_topic:
            json_data["state_topic"] = root_topic + self.state_topic
        if self.device_class:
            json_data["device_class"] = self.device_class
        if self.diagnostic:
            json_data["entity_category"] = "diagnostic"

        return json_data
    
    def unique_id(self):
        return self.device.id + "_" + self.id
    
class Sensor(Component):
    def __init__(self, id, name, device_class, unit_of_measurement, value_path, suggested_display_precision, state_class="measurement", state_topic=None, diagnostic=False):
        super().__init__(id, name, "sensor", device_class, value_path, state_topic, diagnostic=diagnostic)
        self.unit_of_measurement = unit_of_measurement
        self.suggested_display_precision = suggested_display_precision
        self.state_class = state_class

    def discovery_json(self, root_topic):
        json_data = super().discovery_json(root_topic)
        json_data["unit_of_measurement"] = self.unit_of_measurement
        json_data["state_class"] = self.state_class
        json_data["suggested_display_precision"] = self.suggested_display_precision
        return json_data

class PowerSum(Sensor):
    def __init__(self, meter, meter_name):
        super().__init__(meter + "_power_sumwh", meter_name + " Verbrauch", "energy", "Wh", meter + ".power.sumWh", 1, state_class="total")

class PowerCurrent(Sensor):
    def __init__(self, meter, meter_name):
        super().__init__(meter + "_power_currentw", meter_name + " Leistung", "power", "W", meter + ".power.currentW", 0)

class BinarySensor(Component):
    def __init__(self, id, name, device_class, value_path, state_topic=None, diagnostic=False):
        super().__init__(id, name, "binary_sensor", device_class, value_path, state_topic, diagnostic)

    def discovery_json(self, root_topic):
        json_data = super().discovery_json(root_topic)
        json_data["value_template"] = "{{ \"ON\" if value_json." + self.value_path + " else \"OFF\" }}"
        return json_data
    
# static factory functions

def create_power_sensors(meter, meter_name):
    return [
        PowerSum(meter, meter_name),
        PowerCurrent(meter, meter_name)
    ]

def create_status_sensors():
    """
    Creates sensors for the standard device status information.
    """
    return [
        Sensor(
            id="status_iteration",
            name="Status - Iteration",
            device_class=None,
            unit_of_measurement="",
            value_path="status.iteration",
            suggested_display_precision=0,
            diagnostic=True
        ),
        Sensor(
            id="status_sleep_time_s",
            name="Status - Sleep Time",
            device_class="duration",
            unit_of_measurement="s",
            value_path="status.sleepTimeS",
            suggested_display_precision=0,
            state_class="total",
            diagnostic=True
        ),
        Sensor(
            id="status_active_time_s",
            name="Status - Active Time",
            device_class="duration",
            unit_of_measurement="s",
            value_path="status.activeTimeS",
            suggested_display_precision=0,
            state_class="total",
            diagnostic=True
        ),
        Sensor(
            id="status_send_failed_count",
            name="Status - Send Failed Count",
            device_class=None,
            unit_of_measurement="",
            value_path="status.sendFailedCount",
            suggested_display_precision=0,
            state_class="total",
            diagnostic=True
        )
    ]
