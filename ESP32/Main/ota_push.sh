idf.py build && \
 scp build/LZRTag.bin root@xaseiresh.hopto.org:/var/esp_ota/lzrtag.bin && \
 mosquitto_pub -h xaseiresh.hopto.org -t /esp32/lzrtag/ota -m $(date +%s) -r && \
 mosquitto_sub -h xaseiresh.hopto.org -v -N -t "/esp32/lzrtag/+/logs"
