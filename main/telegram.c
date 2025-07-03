/**
 * @file telegram.c
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main
 * @ingroup main
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef TELEGRAM_C
#define TELEGRAM_C 1
 

#include "telegram.h"
#include "esp_log.h"
#include "esp_check.h"
#include "regs_description.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "credentials.h"

#include "esp_tls.h"
/*
   #headers = {'Content-Type': 'application/json'}
   params = {'chat_id': credentials.tel_home_id, 'text': message}
   response = requests.post(f"https://api.telegram.org/bot{credentials.tel_bot_api}/sendMessage", data=params)
   return response
*/

#define SERVER_URL_MAX_SZ 256
#define MAX_URLS 1

extern const uint8_t telegram_pem_start[] asm("_binary_telegram_pem_start");
extern const uint8_t telegram_pem_end[]   asm("_binary_telegram_pem_end");

static const char *web_url = TELEGRAM_FULL_URL;


task_handle_t telegram_handle_id = NULL;
static const char *TAG = "telegram";
#define TELEGRAM_TASK_PERIOD (1000u)
#if TELEGRAM
static int telegram_init(void);
static int telegram_deinit();

static int telegram_init(){
   int result = 0;
   regs_global->vars.current_state[0] |= CS0_TASK_ACTIVE_TELEGRAM;
   return result;
}  
static int telegram_deinit(){
   int result = 0;
   regs_global->vars.current_state[0] &= ~((u32)CS0_TASK_ACTIVE_TELEGRAM);
   return result;
}

esp_tls_cfg_t cfg = {0};

void telegram_task(void *arg){
   (void)(*arg);
   uint32_t signal_value;
   u64 task_counter = 0u;
   int conn_count = 0;
   cfg.cacert_buf = (const unsigned char *) telegram_pem_start;
   cfg.cacert_bytes = telegram_pem_end - telegram_pem_start;
   tls_keep_alive_cfg_t keep_alive_cfg = {0};
   keep_alive_cfg.keep_alive_enable = true;
   keep_alive_cfg.keep_alive_idle = 5;
   keep_alive_cfg.keep_alive_interval = 5;
   keep_alive_cfg.keep_alive_count = 3;
   cfg.keep_alive_cfg = &keep_alive_cfg;
   cfg.timeout_ms = 9000; // Set timeout for TLS connection
   telegram_init();
   char http_request[768];
   int header_len = 0;
   header_len += sprintf(&http_request[header_len],
      "POST /bot%s/sendMessage HTTP/1.1\r\n"
      "Host: api.telegram.org\r\n"
      "Content-Type: application/x-www-form-urlencoded\r\n", tel_bot_api);


   while(1){
      if(task_notify_wait(0xffffffff, &signal_value, TELEGRAM_TASK_PERIOD)==pdTRUE){
         /*by signal*/
         if (signal_value & TELEGRAM_SEND_REG_BY_ID){
            u8 attempts = 10u;
            while(attempts){
               uint16_t reg_id = (signal_value >> 16u) & 0xffff;
               uint16_t reg_num = (signal_value >> 8u) & 0xff;
               char content[256] = {0};
               int content_len = 0;
               int len = 0;
               ESP_LOGI(TAG, "Connecting to %d URLs", MAX_URLS);
               memset(&cfg, 0, sizeof(cfg)); // Reset configuration
               attempts--;
               keep_alive_cfg.keep_alive_enable = true;
               keep_alive_cfg.keep_alive_idle = 5;
               keep_alive_cfg.keep_alive_interval = 5;
               keep_alive_cfg.keep_alive_count = 3;
               cfg.keep_alive_cfg = &keep_alive_cfg;
               cfg.timeout_ms = 9000; // Set timeout for TLS connection
               cfg.cacert_buf = (const unsigned char *) telegram_pem_start;
               cfg.cacert_bytes = telegram_pem_end - telegram_pem_start;
               struct esp_tls *tls = esp_tls_conn_http_new(web_url, &cfg);
               task_delay_ms(25);
               if (tls != NULL) {
                  main_printf(TAG, "Connection established to %s", web_url);
                  conn_count++;
                  content_len += sprintf(&content[content_len],"chat_id=%s&text=", tel_home_id);
                  content_len += regs_description_get_regs_string_value(reg_id, reg_num, &content[content_len], sizeof(content) - content_len);
                  len = header_len;
                  len += sprintf(&http_request[len],
                     "Content-Length: %d\r\n\r\n"
                     "%s", content_len, content);
                  if (esp_tls_conn_write(tls, http_request, strlen(http_request)) > 0) {
                     main_printf(TAG, "Request sent successfully %s", http_request);
                     attempts = 0; // Exit loop on successful send
                  } else {
                     task_delay_ms(10000u);
                     main_printf(TAG, "Failed to send request");
                  }
               } else {
                  task_delay_ms(10000u);
                  main_printf(TAG, "Could not connect to %s", web_url);
               }
               esp_tls_conn_delete(tls);
               main_printf(TAG,"TELEGRAM_SEND_REG_BY_ID %u", reg_id);
            }
         }
         if (signal_value & STOP_CHILD_PROCCES){
            telegram_deinit();
            task_delete(task_get_id());
         }

      }
      task_counter++;
   }

   //esp_http_client_cleanup(client);
}
#endif //TELEGRAM
#endif /*TELEGRAM_C*/
