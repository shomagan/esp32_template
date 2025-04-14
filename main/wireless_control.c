/**
 * @file wireless_control.c
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main
 * @ingroup main
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef WIRELESS_CONTROL_C
#define WIRELESS_CONTROL_C 1
#include "wireless_control.h" 
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_wifi_types.h"
#include <string.h>
#include "credentials.h"
#define wireless_control_TASK_PERIOD (100u)
#define ESP_WIFI_CHANNEL   1u
#define MAX_STA_CONN       4u
#define TEMPORAL_DISACTIVATION_TIME 120/*sec*/ 
typedef enum {
    WIFI_STATE_NOT_ACTIVATED = 0,/*not yet initialIzed or not disabled by default*/
    WIFI_STATE_ACTIVATED = 1,/*activated STA ot AP*/
    WIFI_STATE_DISACTIVATED = 2, /*stoped*/
    WIFI_STATE_DISACTIVATED_TEMPORAL = 3,/*temporal stopped*/
}wireless_state_t;

task_handle_t wireless_control_handle_id = NULL;
static const char *TAG = "wireless_control";
esp_netif_t *ap_netif = NULL;
esp_netif_t *sta_netif = NULL;

static int wireless_control_init(void);
static int wireless_control_deinit();
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data);
static wireless_state_t wifi_init(void);
static void wifi_common_setup(void);
static void wifi_init_softap(void);
static bool wifi_init_sta(void);
static void wifi_init_soft_ap_sta(void);
static int wifi_full_stop(void);
static int wifi_state_handle(u64 task_counter,u32 start_reset_time,wireless_state_t * wifi_state);

/**
 * @brief wifi_event_handler
 * @param arg
 * @param event_base
 * @param event_id
 * @param event_data
 */
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data){
    (void)(arg);
    (void)(event_data);
    if (event_base==WIFI_EVENT){
        switch (event_id) {
        case WIFI_EVENT_AP_STACONNECTED:
            ESP_LOGI(TAG, "Wi-Fi AP got a station connected");
            regs_global->vars.ap_connections_number++;
            break;
        case WIFI_EVENT_AP_STADISCONNECTED:
            ESP_LOGI(TAG, "Wi-Fi AP got a station disconnected");
            if (regs_global->vars.ap_connections_number){
                regs_global->vars.ap_connections_number--;
            }
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "Wi-Fi sta got a station disconnected");
            regs_global->vars.sta_connect=0;
            break;
        default:
            break;
        }
    }else if(event_base==IP_EVENT){
        if (event_id==IP_EVENT_STA_GOT_IP){
            ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
            regs_global->vars.sta_connect = 1;
            memcpy(&regs_global->vars.sta_ip, &event->ip_info.ip.addr, sizeof(uint32_t));
            ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
        }
    }
}

static int wireless_control_init(){
   int result = 0;
   regs_global->vars.current_state[0] |= CS0_TASK_ACTIVE_WIRELESS_CONTROL;
   return result;
}  
static int wireless_control_deinit(){
   int result = 0;
   regs_global->vars.current_state[0] &= ~((u32)CS0_TASK_ACTIVE_WIRELESS_CONTROL);
   return result;
}
void wireless_control_task(void *arg){
   (void)(*arg);
   uint32_t signal_value;
   wireless_control_init();
   u64 task_counter = 0u;
   u32 start_reset_time = 0u;/*capture time of reset command*/
   wireless_state_t wifi_state = WIFI_STATE_NOT_ACTIVATED;
   wifi_ap_record_t ap_info;
   
   while(1){
      wifi_state_handle(task_counter, start_reset_time, &wifi_state);
      signal_value = 0;
      if(task_notify_wait(STOP_CHILD_PROCCES|WIRELESS_TASK_STOP_WIFI, &signal_value, wireless_control_TASK_PERIOD)==pdTRUE){
         /*by signal*/
         if (signal_value & WIRELESS_TASK_STOP_WIFI){
            if (WIFI_STATE_ACTIVATED == wifi_state){
               wifi_full_stop();
            }
            wifi_state = WIFI_STATE_DISACTIVATED;
            main_printf(TAG, "wifi was stopped");
         }
         if (signal_value & WIRELESS_TASK_START_WIFI){
            if (wifi_state != WIFI_STATE_ACTIVATED){
               wifi_state = WIFI_STATE_NOT_ACTIVATED;/*give it a chance to start*/
               main_printf(TAG, "start wifi if need it");
            }
         }
         if (signal_value & WIRELESS_TASK_RESET_WIFI_FOR_120_SEC){
            main_printf(TAG, "wifi WIRELESS_TASK_RESET_WIFI_FOR_120_SEC");
            if (WIFI_STATE_ACTIVATED == wifi_state){
               wifi_full_stop();
            }
            regs_copy_safe(&start_reset_time,&regs_global->vars.live_time,sizeof(regs_global->vars.live_time));
            wifi_state = WIFI_STATE_DISACTIVATED_TEMPORAL;
         }
         if (signal_value & STOP_CHILD_PROCCES){
            if (WIFI_STATE_ACTIVATED == wifi_state){
               wifi_full_stop();
            }
            wireless_control_deinit();
            task_delete(task_get_id());
         }
      }
      esp_wifi_sta_get_ap_info(&ap_info);
      regs_global->vars.rssi_ap[0] = ap_info.rssi;/*atomic*/
      regs_global->vars.primary_channel_ap[0] = ap_info.primary;//atomic
      task_counter++;
   }
}
static int wifi_state_handle(u64 task_counter,u32 start_reset_time,wireless_state_t * wifi_state){
   int result = 0;
   switch(*wifi_state){
      case WIFI_STATE_NOT_ACTIVATED:/*not yet initialIzed*/
         *wifi_state = wifi_init();/*trying to init */
      break;
      case WIFI_STATE_ACTIVATED:/*activated STA ot AP*/
         if(((task_counter)%(10000u/wireless_control_TASK_PERIOD))==0u){    // every 10 sec
            if((regs_global->vars.wifi_setting == WIFI_CLIENT) ||
               (regs_global->vars.wifi_setting == WIFI_ESP32_CHANGED_ONLY_CLIENT)||
               (regs_global->vars.wifi_setting == WIFI_AP_STA)||
               (regs_global->vars.wifi_setting == WIFI_ESP32_CHANGED_ONLY_AP_STA)){
               semaphore_take(regs_access_mutex, portMAX_DELAY);{
               if(regs_global->vars.sta_connect==0){
                  main_printf(TAG, "try connect to sta");
                  if (ESP_OK != esp_wifi_connect()){
                     main_error_message(TAG, "connection to sta failed");
                  }
               }
               }semaphore_release(regs_access_mutex);
            }
         }
         break;
      case WIFI_STATE_DISACTIVATED:/*stoped*/
      break;
      case WIFI_STATE_DISACTIVATED_TEMPORAL:/*temporal stopped*/
      {
         u32 current_live_time = 0u;
         regs_copy_safe(&current_live_time,&regs_global->vars.live_time,sizeof(regs_global->vars.live_time));
         if ((current_live_time < start_reset_time)||
               ((current_live_time - start_reset_time)>TEMPORAL_DISACTIVATION_TIME)){
            *wifi_state = WIFI_STATE_NOT_ACTIVATED;/*give it a chance to start*/
         }
      }
      break;
      default:
      result = -1;
      break;
   }
   return result;
}
/*
* @brief wifi_init - init wifi
* @return
*/
static wireless_state_t wifi_init(void){
    wireless_state_t result = WIFI_STATE_NOT_ACTIVATED;
    wifi_common_setup();
    main_printf(TAG, "wifi setting %d",regs_global->vars.wifi_setting);
    main_printf(TAG, "ap_name:%s ap_password:%s sta_name:%s sta_password:%s channel:%d",
             regs_global->vars.wifi_name, regs_global->vars.wifi_password,
             regs_global->vars.wifi_router_name, regs_global->vars.wifi_router_password,ESP_WIFI_CHANNEL);
#if MAIN_CONFIG_WIFI_AP
    regs_global->vars.wifi_setting = WIFI_ACCESS_POINT;
#elif MAIN_CONFIG_WIFI_NODE
    regs_global->vars.wifi_setting = WIFI_CLIENT;
#endif
    if (regs_global->vars.wifi_setting == WIFI_ACCESS_POINT ||
        regs_global->vars.wifi_setting == WIFI_ESP32_CHANGED_ONLY_ACCESS_POINT){
        main_printf(TAG, "ESP_WIFI_MODE_AP");
        wifi_init_softap();
        result = WIFI_STATE_ACTIVATED;
    }else if(regs_global->vars.wifi_setting == WIFI_CLIENT ||
             regs_global->vars.wifi_setting == WIFI_ESP32_CHANGED_ONLY_CLIENT){
        main_printf(TAG, "ESP_WIFI_MODE_STA");
        wifi_init_sta();
        result = WIFI_STATE_ACTIVATED;
    }else if(regs_global->vars.wifi_setting == WIFI_AP_STA||
             regs_global->vars.wifi_setting == WIFI_ESP32_CHANGED_ONLY_AP_STA){
        main_printf(TAG, "ESP_WIFI_MODE_AP_STA");
        wifi_init_soft_ap_sta();
        result = WIFI_STATE_ACTIVATED;
    }else{
        result = WIFI_STATE_NOT_ACTIVATED;
    }
    return result;
}
/**
 * @brief wifi_common_setup - common wifi init
 */
static void wifi_common_setup(void){
    static bool initialized = false;
    if (initialized) {
        return;
    }
    ap_netif = esp_netif_create_default_wifi_ap();
    if(NULL == ap_netif){
        ESP_LOGE(TAG, "ESP_WIFI_MODE_AP");
    }
    sta_netif = esp_netif_create_default_wifi_sta();
    if(NULL == sta_netif){
        ESP_LOGE(TAG, "ESP_WIFI_MODE_STA");
    }
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_NULL));
}
/**
 * @brief wifi_init_softap - init softap
 */
static void wifi_init_soft_ap_sta(void){
    wifi_config_t ap_config;
    memset(&ap_config,0,sizeof(wifi_config_t));
    memset(ap_config.ap.ssid,0,sizeof(ap_config.ap.ssid));
    memset(ap_config.ap.password,0,sizeof(ap_config.ap.password));
    memcpy(ap_config.ap.ssid,regs_global->vars.wifi_name,WIFI_NAME_LEN);
    memcpy(ap_config.ap.password,regs_global->vars.wifi_password,WIFI_PASSWORD_LEN);
    ap_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
    ap_config.ap.ssid_len = WIFI_NAME_LEN;
    ap_config.ap.max_connection = MAX_STA_CONN;
    ap_config.ap.channel = ESP_WIFI_CHANNEL;
    wifi_config_t sta_config;
    memset(&sta_config,0,sizeof(wifi_config_t));
    memcpy(sta_config.sta.ssid,regs_global->vars.wifi_router_name,strlen((char*)&regs_global->vars.wifi_router_name));
    memcpy(sta_config.sta.password,regs_global->vars.wifi_router_password,strlen((char*)&regs_global->vars.wifi_router_password));
    
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_APSTA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_AP, &ap_config) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );
    return;
}
/**
 * @brief wifi_init_softap
 */
void wifi_init_softap(void){
    static uint8_t temp_ssid[32] = {0};
    memcpy(temp_ssid,regs_global->vars.wifi_name,WIFI_NAME_LEN);
    wifi_config_t wifi_config = {
        .ap = {
            .ssid_len = WIFI_NAME_LEN,
            .channel = ESP_WIFI_CHANNEL,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK
        },
    };
    memset(wifi_config.ap.ssid,0,sizeof(wifi_config.ap.ssid));
    memset(wifi_config.ap.password,0,sizeof(wifi_config.ap.password));
    memcpy(wifi_config.ap.ssid,regs_global->vars.wifi_name,WIFI_NAME_LEN);
    memcpy(wifi_config.ap.password,regs_global->vars.wifi_password,WIFI_PASSWORD_LEN);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             regs_global->vars.wifi_name, regs_global->vars.wifi_password, ESP_WIFI_CHANNEL);
}
static bool wifi_init_sta(void){
    wifi_config_t sta_config;
    memset(&sta_config, 0,sizeof(wifi_config_t));
    strcpy((char *)sta_config.sta.ssid, (char*)regs_global->vars.wifi_router_name);
    strcpy((char *)sta_config.sta.password, (char*)regs_global->vars.wifi_router_password);
    main_printf(TAG, "ap_name:%s ap_password:%s ",
             sta_config.sta.ssid, sta_config.sta.password);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK( esp_wifi_connect() );
    return 0;
}
static int wifi_full_stop(void){
   int result = 0;
   esp_err_t err = esp_wifi_stop();
   if (err != ESP_ERR_WIFI_NOT_INIT) {
      ESP_ERROR_CHECK(err);
      ESP_ERROR_CHECK(esp_wifi_deinit());
      if(ap_netif!=NULL){
         ESP_ERROR_CHECK(esp_wifi_clear_default_wifi_driver_and_handlers(ap_netif));
         esp_netif_destroy(ap_netif);
      }
      if(sta_netif!=NULL){
         ESP_ERROR_CHECK(esp_wifi_clear_default_wifi_driver_and_handlers(sta_netif));
         esp_netif_destroy(sta_netif);
      }
   }else{
      result = -1;
   }
   return result;
} 

#endif /*WIRELESS_CONTROL_C*/
