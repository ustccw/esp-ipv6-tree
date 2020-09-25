/**
 * IPv6 Ping example
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
#include "esp_ping.h"
#include "ping/ping_sock.h"

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

static const int PING_DONE_BIT = BIT(0);

static EventGroupHandle_t s_wifi_event_group;
static EventGroupHandle_t s_ping_event_group;

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

static void cmd_ping_on_ping_success(esp_ping_handle_t hdl, void *args)
{
    uint8_t ttl;
    uint16_t seqno;
    uint32_t elapsed_time, recv_len;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TTL, &ttl, sizeof(ttl));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
    printf("%d bytes from %s icmp_seq=%d ttl=%d time=%d ms\n",
           recv_len, ip6addr_ntoa(&target_addr.u_addr.ip6), seqno, ttl, elapsed_time);
}

static void cmd_ping_on_ping_timeout(esp_ping_handle_t hdl, void *args)
{
    uint16_t seqno;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    printf("From %s icmp_seq=%d timeout\n", ip6addr_ntoa(&target_addr.u_addr.ip6), seqno);
}

static void cmd_ping_on_ping_end(esp_ping_handle_t hdl, void *args)
{
    ip_addr_t target_addr;
    uint32_t transmitted;
    uint32_t received;
    uint32_t total_time_ms;
    esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
    esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));
    uint32_t loss = (uint32_t)((1 - ((float)received) / transmitted) * 100);
    if (IP_IS_V4(&target_addr)) {
        printf("\n--- %s ping statistics ---\n", inet_ntoa(*ip_2_ip4(&target_addr)));
    } else {
        printf("\n--- %s ping statistics ---\n", inet6_ntoa(*ip_2_ip6(&target_addr)));
    }
    printf("%d packets transmitted, %d received, %d%% packet loss, time %dms\n",
           transmitted, received, loss, total_time_ms);
    // delete the ping sessions, so that we clean up all resources and can create a new ping session
    // we don't have to call delete function in the callback, instead we can call delete function from other tasks
    esp_ping_delete_session(hdl);
    xEventGroupSetBits(s_ping_event_group, PING_DONE_BIT);
}

static void ping_test(void)
{
    // IPv6 Ping List
    const char *ping_list[] = {
        "240c::6666",           // China Future Internet Engineering Center
        "240e:5a::6666",        // JiangSu IPv6 DNS Server
        "www.ipv6.com",
        "ds.test-ipv6.com",
    };

    s_ping_event_group = xEventGroupCreate();
    int ping_count = sizeof(ping_list) / sizeof(ping_list[0]);

    for (int i = 0; i < ping_count; ++i) {
        // DNS Lookup
        struct addrinfo *res = NULL;
        const struct addrinfo hints = {
            .ai_family = AF_INET6,
        };

        int err = getaddrinfo(ping_list[i], NULL, &hints, &res);
        if (err != 0 || res == NULL) {
            ESP_LOGE(TAG, "DNS lookup failed(%s), err=%d res=%p", ping_list[i], err, res);
            continue;
        }

        char ip6_addr[128];
        struct addrinfo *curr;
        struct sockaddr_in6 *sockaddr_ipv6;

        for (curr = res; curr != NULL; curr = curr->ai_next) {
            if (res->ai_family != AF_INET6) {
                return;
            }
            sockaddr_ipv6 = (struct sockaddr_in6 *)(curr->ai_addr);
            inet_ntop(AF_INET6, &sockaddr_ipv6->sin6_addr, ip6_addr, sizeof(ip6_addr));
            ESP_LOGI(TAG, "IP is %s (%s)", ip6_addr, ping_list[i]);
        }

        // PING
        xEventGroupClearBits(s_ping_event_group, PING_DONE_BIT);
        ip_addr_t target_addr;
        esp_ping_config_t config = ESP_PING_DEFAULT_CONFIG();
        struct in6_addr addr6 = ((struct sockaddr_in6 *) (res->ai_addr))->sin6_addr;
        inet6_addr_to_ip6addr(ip_2_ip6(&target_addr), &addr6);
        config.target_addr = target_addr;
        freeaddrinfo(res);

        /* set callback functions */
        esp_ping_callbacks_t cbs = {
            .on_ping_success = cmd_ping_on_ping_success,
            .on_ping_timeout = cmd_ping_on_ping_timeout,
            .on_ping_end = cmd_ping_on_ping_end,
            .cb_args = NULL
        };
        esp_ping_handle_t ping;
        esp_ping_new_session(&config, &cbs, &ping);
        esp_ping_start(ping);
        xEventGroupWaitBits(s_ping_event_group, PING_DONE_BIT, false, true, portMAX_DELAY);
    }

    return;
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    initialise_wifi();

    xEventGroupWaitBits(s_wifi_event_group, IPV4_CONNECTED_BIT | IPV6_CONNECTED_BIT, false, true, portMAX_DELAY);

    ping_test();
}
