# IPv6 address
A simple example shows how to get Linklocal IPv6 address and Global IPv6 address, based on ESP8266_RTOS_SDK and esp-idf SDK.

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
I (274) boot: Loaded app from partition at offset 0x10000
I (274) boot: Disabling RNG early entropy source...
I (274) cache: Instruction cache    : size 8KB, 4Ways, cache line size 32Byte
I (282) cpu_start: Pro cpu up.
I (285) cpu_start: Application information:
I (290) cpu_start: Project name:     ipv6-addr
I (295) cpu_start: App version:      bef38bf-dirty
I (301) cpu_start: Compile time:     Sep 22 2020 10:07:50
I (307) cpu_start: ELF file SHA256:  36d810710153c94c...
I (313) cpu_start: ESP-IDF:          v4.2-dev-2182-gdddcc2ede8-dirty
I (320) cpu_start: Single core mode
I (324) heap_init: Initializing. RAM available for dynamic allocation:
I (331) heap_init: At 3FFCF260 len 0002CDA0 (179 KiB): DRAM
I (338) heap_init: At 3FFFC000 len 00003A10 (14 KiB): DRAM
I (344) cpu_start: Pro cpu start user code
I (406) spi_flash: detected chip: generic
I (407) spi_flash: flash io: dio
W (407) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (417) cpu_start: Starting scheduler on PRO CPU.
I (463) wifi:wifi driver task: 3ffd7640, prio:23, stack:6656, core=0
I (463) system_api: Base MAC address is not set
I (463) system_api: read default base MAC address from EFUSE
I (473) wifi:wifi firmware version: d3be909
I (473) wifi:wifi certification version: v7.0
I (473) wifi:config NVS flash: enabled
I (473) wifi:config nano formating: disabled
I (483) wifi:Init data frame dynamic rx buffer num: 32
I (483) wifi:Init management frame dynamic rx buffer num: 32
I (493) wifi:Init management short buffer num: 32
I (493) wifi:Init dynamic tx buffer num: 32
I (503) wifi:Init static rx buffer size: 1600
I (503) wifi:Init static rx buffer num: 10
I (503) wifi:Init dynamic rx buffer num: 32
I (513) wifi_init: rx ba win: 6
I (513) wifi_init: tcpip mbox: 32
I (523) wifi_init: udp mbox: 6
I (523) wifi_init: tcp mbox: 6
I (523) wifi_init: tcp tx win: 5744
I (533) wifi_init: tcp rx win: 5744
I (533) wifi_init: tcp mss: 1440
I (543) wifi_init: WiFi IRAM OP enabled
I (543) wifi_init: WiFi RX IRAM OP enabled
I (553) ipv6-example: Setting WiFi configuration SSID HUAWEI-IPV6-IPV6 PASSWD:1234567890
I (853) phy: phy_version: 603, 72dfd77, Jul  7 2020, 19:57:05, 0, 2
I (853) wifi:enable tsf
I (853) wifi:mode : sta (7c:df:a1:00:39:2c)
I (903) wifi:state: init -> init (ce00)
I (973) wifi:new:<1,0>, old:<1,0>, ap:<255,255>, sta:<1,0>, prof:1
I (1663) wifi:state: init -> auth (b0)
I (1673) wifi:state: auth -> assoc (0)
I (1673) wifi:state: assoc -> run (10)
I (1743) wifi:connected with HUAWEI-IPV6-IPV6, aid = 3, channel 1, BW20, bssid = 54:f2:94:b1:6c:18
I (1743) wifi:security: WPA2-PSK, phy: bgn, rssi: -25
I (1753) wifi:pm start, type: 1

I (1823) wifi:AP's beacon interval = 102400 us, DTIM period = 1
I (2943) esp_netif_handlers: sta ip: 192.168.3.7, mask: 255.255.255.0, gw: 192.168.3.1
I (2943) ipv6-example: Got IPv4: 192.168.3.7
I (3443) ipv6-example: Got IPv6: FE80::7EDF:A1FF:FE00:392C
I (7443) ipv6-example: Got IPv6: 240E:3A1:2074:430:7EDF:A1FF:FE00:392C
I (7443) ipv6-example: IPv6 Address Example Test Pass!
```
