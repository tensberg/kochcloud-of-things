from components import *

KOCHCLOUD_OF_THINGS_DEVICES = [
    Device(
        id="gateway-robot",
        name="Gateway Robot",
        components=[
            Switch(
                id="led_global_state",
                name="LED Global State",
                value_property="global",
                command_topic="leds",
            ),
            Switch(
                id="led_user1",
                name="LED 1",
                value_property="user1",
                command_topic="leds",
            ),
            Switch(
                id="led_user2",
                name="LED 2",
                value_property="user2",
                command_topic="leds",
            ),
            Switch(
                id="led_user3",
                name="LED 3",
                value_property="user3",
                command_topic="leds",
            ),
            NumberControl(
                id="display_brightness",
                name="Display Brightness",
                value_property="brightness",
                value_min=0,
                value_max=255,
                command_topic="display",
                state_topic="display/state",
                step=1,
                mode="slider"
            ),
            TextControl(
                id="display_image",
                name="Display Image",
                value_property="image",
                command_topic="display",
                state_topic="display/state"
            ),
            Trigger(
                id="initialized",
                name="Gateway Robot Initialized",
                topic="robot/initialized",
                type="Initialized",
                subtype="Gateway Robot",
            )
        ]
    ),
    Device(
        id="stromzaehler",
        name="Stromzähler",
        state_topic="stromzaehler/state",
        components=[
            create_status_sensors(),
            create_power_sensors("haushalt", "Haushalt"),
            create_power_sensors("waerme", "Wärmepumpe"),
            BinarySensor(
                id="config_continuous_mode",
                name="Config - Continuous Mode",
                device_class=None,
                value_path="config.continuousMode"
            )
        ]
    ),
    Device(
        id="zisternensensor",
        name="Zisternensensor",
        state_topic="zisternensensor/state",
        components=[
            create_status_sensors(),
            Sensor(
                id="temperature",
                name="Temperatur",
                device_class="temperature",
                unit_of_measurement="°C",
                value_path="data.temperature",
                suggested_display_precision=1,
            ),
            Sensor(
                id="humidity",
                name="Luftfeuchtigkeit",
                device_class="humidity",
                unit_of_measurement="%",
                value_path="data.humidity",
                suggested_display_precision=0,
            ),
            Sensor(
                id="pressure",
                name="Luftdruck (absolut)",
                device_class="pressure",
                unit_of_measurement="hPa",
                value_path="data.pressure",
                suggested_display_precision=0,
            )
        ]
    )
]
