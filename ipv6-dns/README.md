# IPv6 DNS
A simple example shows how to configurate IPv6 DNS server and resolve the domain name, based on ESP8266_RTOS_SDK and esp-idf SDK.

### Several Domain Name Resolve tools
- Linux System Shell Command
    ```
    host -t AAAA google.com
    ```
- [webwiz](https://network-tools.webwiz.net/reverse-dns.htm) to lookup PTR Record (IPv4 + IPv6)
- [boce](https://www.boce.com/ipv6) to check website IPv6 accessibility validator, language: CN
- [ipv6test](https://ipv6-test.com/validate.php) to check website IPv6 accessibility validator, language: EN
- [v6sc](https://v6sc.ipip.net) for China IPv6 website

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
I (10314) wifi:state: init -> auth (b0)
I (10314) wifi:state: auth -> assoc (0)
I (10344) wifi:state: assoc -> run (10)
I (10464) wifi:connected with HUAWEI-IPV6-IPV6, aid = 3, channel 1, BW20, bssid = 54:f2:94:b1:6c:18
I (10474) wifi:security: WPA2-PSK, phy: bgn, rssi: -25
I (10474) wifi:pm start, type: 1

I (10534) wifi:AP's beacon interval = 102400 us, DTIM period = 1
I (11444) esp_netif_handlers: sta ip: 192.168.3.7, mask: 255.255.255.0, gw: 192.168.3.1
I (11444) ipv6-example: Got IPv4: 192.168.3.7
I (12444) ipv6-example: Got IPv6: FE80::7EDF:A1FF:FE00:392C
I (15444) ipv6-example: Got IPv6: 240E:3A1:2074:430:7EDF:A1FF:FE00:392C
I (15444) ipv6-example: type:6 (0:IPv4 6:IPv6 o:any)
I (15444) ipv6-example: Valid DNSv6: 240c:0000:0000:0000:0000:0000:0000:6666
I (15454) ipv6-example: type:6 (0:IPv4 6:IPv6 o:any)
I (15454) ipv6-example: Valid DNSv6: 240e:005a:0000:0000:0000:0000:0000:6666
I (15464) ipv6-example: type:6 (0:IPv4 6:IPv6 o:any)
I (15464) ipv6-example: Valid DNSv6: 2001:4860:4860:0000:0000:0000:0000:8888
I (15774) ipv6-example: DNS lookup succeeded, IP: 2604:7C00:18:E::2(www.ipv6.com)
I (15824) ipv6-example: DNS lookup succeeded, IP: 2001::C710:9EB4(ipv6.google.com)
I (16894) ipv6-example: DNS lookup succeeded, IP: 2001:470:1:18::115(ds.test-ipv6.com)
I (18234) ipv6-example: DNS lookup succeeded, IP: 2001:470:1:18::115(ipv6.test-ipv6.com)
```
