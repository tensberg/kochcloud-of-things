#!/bin/bash
socat -v PTY,link=./gateway-robot,waitslave,rawer TCP:kochcloud.local:20001
