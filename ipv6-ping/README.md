# IPv6 Ping
A simple example shows how to use IPv6 Ping, based on ESP8266_RTOS_SDK and esp-idf SDK.

### IPv6 Ping tools
// TODO

### SDK Version
- ESP8266_RTOS_SDK: `v3.4-dev-334-gc972cccf`
- esp-idf: `v4.2-dev-2182-gdddcc2ede8`

### Configure the project
Configurate your SSID and Password at `menuconfig` OR `sdkconfig.defaults`

### Build and Flash
Support `GNU make` and `CMake`.

- ESP8266
```
export IDF_PATH=${your_esp8266_rtos_sdk_path}
make flash monitor
```

- ESP32
```
export IDF_PATH=${your_esp_idf_path}
$IDF_PATH/tool/idf.py set-target esp32
$IDF_PATH/tool/idf.py flash monitor
```

- ESP32S2
```
export IDF_PATH=${your_esp_idf_path}
$IDF_PATH/tool/idf.py set-target esp32s2
$IDF_PATH/tool/idf.py flash monitor
```

## Example Output
```
...
// TO TEST
```
