#!/bin/sh

MQTT_HOST="iot.eclipse.org"
TARGET_PLAYER="Xasin2"

mosquitto_sub -h $MQTT_HOST -q 0 -t Lasertag/Players/$TARGET_PLAYER/Console/Out &

mosquitto_pub -h $MQTT_HOST -q 0 -m "ping(3000, 3000, 100)" -t Lasertag/Players/$TARGET_PLAYER/Console/In
mosquitto_pub -h $MQTT_HOST -q 0 -l -t Lasertag/Players/$TARGET_PLAYER/Console/In
