## Remote Testing

The robot-gateway project setup allows remote testing by forwarding the serial connection between the kochcloud Raspberry Pi and the gateway-robot ESP8266 via ser2net.
The ser2net setup is configured in the kochcloud-ansible robotgateway.yml playbook and always available.

In order to use it for remote testing:
* Stop the robot-gateway service on the kochcloud Raspberry Pi: `sudo systemctl stop robot-gateway`.
* Create a local serial device that connects to the remote ser2net service by starting `./dev/remote-gateway-robot.sh`.
* Start the robot-gateway application using the VS Code launch configuration *robot-gateway (staging)" or "robot-gateway (prod)".
  The launch configurations use the robotgateway-*.env files, which are already set up to use the local serial device created by the remote-gateway-robot.sh script.
* After finishing the tests, stop the robot-gateway application in VS Code and terminate the remote-gateway-robot.sh script.
* Finally, restart the robot-gateway service on the kochcloud Raspberry Pi: `sudo systemctl start robot-gateway`.
