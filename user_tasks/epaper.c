/**
 * @file epaper.c
 * @author Shoma Gane <shomagan@gmail.com>
 *
 * @defgroup main
 * @ingroup main
 * @version 0.1
 * @brief  TODO!!! write brief in
 */
#ifndef EPAPER_C
#define EPAPER_C 1


#include "epaper.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "link_functions.h"


task_handle_t epaper_handle_id = NULL;
static const char *TAG = "epaper";
#define EPAPER_TASK_PERIOD (100u)
#if EPAPER
/*we act studion 2.9" epaper module 128x296 */
static int epaper_init(void);
static int epaper_deinit();

static int epaper_init(){
   int result = 0;
   regs_global->vars.current_state[0] |= CS0_TASK_ACTIVE_EPAPER;
   return result;
}
static int epaper_deinit(){
   int result = 0;
   regs_global->vars.current_state[0] &= ~((u32)CS0_TASK_ACTIVE_EPAPER);
   return result;
}
void epaper_task(void *arg){
   (void)arg;
   uint32_t signal_value;
   epaper_init();
   u64 task_counter = 0u;
   while(1){
      if(link_functions.os_thread_signal_wait(STOP_CHILD_PROCCES, &signal_value, EPAPER_TASK_PERIOD)==pdTRUE){
         /*by signal*/
         if (signal_value & STOP_CHILD_PROCCES){
            epaper_deinit();
            link_functions.os_thread_terminate(link_functions.os_thread_get_id());
         }
      }
      task_counter++;
   }
}

uint8_t epd_init(void) {
  if (_hibernating)
    epd_reset();

  if (epd_wait_busy()) {
    return 1;
  }
  epd_write_reg(0x12); // SWRESET

  epd_delay(100);

  if (epd_wait_busy())
    return 1;

  epd_write_reg(0x01); // Driver output control
  epd_write_data(0x27);
  epd_write_data(0x01);
  epd_write_data(0x01);

  epd_write_reg(0x11); // data entry mode
  epd_write_data(0x01);

  epd_write_reg(0x44); // set Ram-X address start/end position
  epd_write_data(0x00);
  epd_write_data(0x0F); // 0x0F-->(15+1)*8=128

  epd_write_reg(0x45);  // set Ram-Y address start/end position
  epd_write_data(0x27); // 0x127-->(295+1)=296
  epd_write_data(0x01);
  epd_write_data(0x00);
  epd_write_data(0x00);

  epd_write_reg(0x3C); // BorderWavefrom
  epd_write_data(0x05);

  epd_write_reg(0x21); //  Display update control
  epd_write_data(0x00);
  epd_write_data(0x80);

  epd_write_reg(0x18); // Read built-in temperature sensor
  epd_write_data(0x80);

  epd_setpos(0, 0);

  if (epd_power_on())
    return 1;

  return 0;
}

#endif //EPAPER
#endif /*EPAPER_C*/
