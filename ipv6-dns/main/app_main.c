/**
 * IPv6 DNS example
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "lwip/ip_addr.h"
#include "lwip/ip6_addr.h"
#include "lwip/dns.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"

#include "nvs_flash.h"

#define EXAMPLE_WIFI_SSID       CONFIG_EXAMPLE_WIFI_SSID
#define EXAMPLE_WIFI_PASS       CONFIG_EXAMPLE_WIFI_PASSWORD

#ifdef CONFIG_EXAMPLE_IPV6_ADDR_PREF_GLOBAL
#define IPV6_CONNECTED_BIT  (IPV6LL_CONNECTED_BIT | IPV6GL_CONNECTED_BIT)
#else
#define IPV6_CONNECTED_BIT  (IPV6LL_CONNECTED_BIT)
#endif

static const int IPV4_CONNECTED_BIT = BIT(0);
static const int IPV6LL_CONNECTED_BIT = BIT(1);
static const int IPV6GL_CONNECTED_BIT = BIT(2);
static EventGroupHandle_t s_wifi_event_group;

static const char *TAG = "ipv6-example";

static void on_wifi_disconnect(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    xEventGroupClearBits(s_wifi_event_group, IPV4_CONNECTED_BIT | IPV6_CONNECTED_BIT);
    ESP_LOGI(TAG, "Wi-Fi disconnected, trying to reconnect...");
    ESP_ERROR_CHECK(esp_wifi_connect());
}

static void on_wifi_start(void *args, esp_event_base_t base, int32_t id, void* event_data)
{
    ESP_ERROR_CHECK(esp_wifi_connect());
}

static void on_wifi_connect(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    tcpip_adapter_create_ip6_linklocal(TCPIP_ADAPTER_IF_STA);
}

static void on_got_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ip4_addr_t s_ip_addr;
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    memcpy(&s_ip_addr, &event->ip_info.ip, sizeof(s_ip_addr));
    xEventGroupSetBits(s_wifi_event_group, IPV4_CONNECTED_BIT);
    ESP_LOGI(TAG, "Got IPv4: %s", ip4addr_ntoa(&s_ip_addr));
}

static void on_got_ipv6(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ip6_addr_t s_ipv6_addr;
    ip_event_got_ip6_t *event = (ip_event_got_ip6_t *)event_data;
    memcpy(&s_ipv6_addr, &event->ip6_info.ip, sizeof(s_ipv6_addr));

    ESP_LOGI(TAG, "Got IPv6: %s", ip6addr_ntoa(&s_ipv6_addr));

    if (ip6_addr_isglobal(&s_ipv6_addr)) {
        xEventGroupSetBits(s_wifi_event_group, IPV6GL_CONNECTED_BIT);
    } else if (ip6_addr_islinklocal(&s_ipv6_addr)) {
        xEventGroupSetBits(s_wifi_event_group, IPV6LL_CONNECTED_BIT);
    }
}

static void initialise_wifi(void)
{
    s_wifi_event_group = xEventGroupCreate();
    xEventGroupClearBits(s_wifi_event_group, IPV4_CONNECTED_BIT | IPV6_CONNECTED_BIT);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2)
    esp_netif_create_default_wifi_sta();
#endif

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &on_wifi_connect, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_START, on_wifi_start, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_GOT_IP6, &on_got_ipv6, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s PASSWD:%s", wifi_config.sta.ssid, wifi_config.sta.password);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static esp_err_t config_dns_server(void)
{
#ifdef CONFIG_EXAMPLE_IPV6_DNS_SERVER
    // IPv6 DNS Server definition
    const char *dns_server[] = {
        "240c::6666",            // China Future Internet Engineering Center
        "240e:5a::6666",         // JiangSu IPv6 DNS Server
        "2001:4860:4860::8888"   // google-public-dns-a.google.com
    };

    // IPv6 DNS Server Set
    for (int i = 0; i < DNS_MAX_SERVERS; ++i) {
        ip_addr_t dnsserver;
        if (ipaddr_aton(dns_server[i], &dnsserver) == 0) {
            ESP_LOGE(TAG, "IPv6 Address ERROR");
            return ESP_FAIL;
        }
        dns_setserver(i, &dnsserver);
    }

    // IPv6 DNS Server Get
    for (int i = 0; i < DNS_MAX_SERVERS; ++i) {
        const ip_addr_t *addr = dns_getserver(i);
        if (addr == NULL) {
            continue;
        }
        ESP_LOGI(TAG, "type:%d (0:IPv4 6:IPv6 o:any)", addr->type);

        switch (addr->type) {
        case IPADDR_TYPE_V4: {
            ip4_addr_t ip4 = addr->u_addr.ip4;
            ESP_LOGI(TAG, "Valid DNSv4: "IPSTR"", IP2STR(&ip4));
        }
        break;

        case IPADDR_TYPE_V6: {
            ip6_addr_t ip6 = addr->u_addr.ip6;
            ESP_LOGI(TAG, "Valid DNSv6: "IPV6STR"", IPV62STR(ip6));
        }
        break;

        default:
            ESP_LOGW(TAG, "undefined type");
            break;
        }
    }

#endif
    return ESP_OK;
}

static void resolve_domain_name_test(void)
{
    // IPv6 DNS Server definition
    const char *domain_name_list[] = {
        "www.ipv6.com",
        "ipv6.google.com",
        "ds.test-ipv6.com",
        "ipv6.test-ipv6.com",   // IPv6-only host
    };

    int domain_name_count = sizeof(domain_name_list) / sizeof(domain_name_list[0]);
    for (int i = 0; i < domain_name_count; ++i) {
        struct addrinfo *res = NULL;
        const struct addrinfo hints = {
            .ai_family = AF_INET6,
            .ai_socktype = SOCK_STREAM,
        };

        while (1) {
            int err = getaddrinfo(domain_name_list[i], NULL, &hints, &res);
            if (err != 0 || res == NULL) {
                ESP_LOGE(TAG, "DNS lookup failed(%s), err=%d res=%p", domain_name_list[i], err, res);
                break;
            }

            // Lookup OK
            char ip6_addr[128];
            struct addrinfo *curr;
            struct sockaddr_in6 *sockaddr_ipv6;

            for (curr = res; curr != NULL; curr = curr->ai_next) {
                sockaddr_ipv6 = (struct sockaddr_in6 *)(curr->ai_addr);
                inet_ntop(AF_INET6, &sockaddr_ipv6->sin6_addr, ip6_addr, sizeof(ip6_addr));
                ESP_LOGI(TAG, "DNS lookup succeeded, IP: %s(%s)", ip6_addr, domain_name_list[i]);
            }

            freeaddrinfo(res);
            break;
        }
    }

    return;
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    initialise_wifi();

    xEventGroupWaitBits(s_wifi_event_group, IPV4_CONNECTED_BIT | IPV6_CONNECTED_BIT, false, true, portMAX_DELAY);
    
    config_dns_server();

    resolve_domain_name_test();
}
