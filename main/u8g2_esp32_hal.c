#include <stdio.h>
#include <string.h>

#include "sdkconfig.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "main_config.h"
#include "u8g2_esp32_hal.h"
#include "regs.h"
#include "pin_map.h"


static const char *TAG = "u8g2_hal";
static const unsigned int I2C_TIMEOUT_MS = 1000;
u8g2_t u8g2; // a structure which will contain all the data for one display
static spi_device_handle_t handle_spi;      // SPI handle.
static i2c_cmd_handle_t    handle_i2c;      // I2C handle.
static u8g2_esp32_hal_t    u8g2_esp32_hal;  // HAL state data.

#if DISPLAY
int init_display(void){
    u8g2_esp32_hal_t esp32_hal = U8G2_ESP32_HAL_DEFAULT;
#if NOKIA_5110
    int res=0;
    gpio_config_t io_conf = {0};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1<<NOKIA_PIN_RESET)| (1<<NOKIA_PIN_BL);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    gpio_set_level(NOKIA_PIN_RESET, 0);
	 int itt =1000;
	 while(itt--){	};
    gpio_set_level(NOKIA_PIN_RESET, 1);
    esp32_hal.mosi  = NOKIA_PIN_SDA;
    esp32_hal.clk  = NOKIA_PIN_SCK;
	 esp32_hal.reset = NOKIA_PIN_RESET;
	 esp32_hal.dc   = NOKIA_PIN_DC;
	 esp32_hal.cs   = NOKIA_PIN_CS;
    u8g2_esp32_hal_init(esp32_hal);
	 u8g2_Setup_pcd8544_84x48_f(
		  &u8g2,
        U8G2_R0,
        u8g2_esp32_spi_byte_cb,
        u8g2_esp32_gpio_and_delay_cb);  // init u8g2 structure
    u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
    main_printf(TAG, "init display passed");
    u8g2_SetPowerSave(&u8g2, 0); // wake up display
    main_printf(TAG, "set power save passed");
    u8g2_ClearBuffer(&u8g2);
    char address[64] = {0};
    sprintf(address,"mdb: %u, ip: %u.%u.%u.%u",regs_global->vars.mdb_addr,regs_global->vars.sta_ip[0],regs_global->vars.sta_ip[1],regs_global->vars.sta_ip[2],regs_global->vars.sta_ip[3]);
    u8g2_SetFont(&u8g2, u8g2_font_5x8_tf);
    u8g2_DrawStr(&u8g2, 0,7, address);
    u8g2_SendBuffer(&u8g2);
    return res;
#else
    int res=0;
    gpio_config_t io_conf = {0};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1<<PIN_RESET;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    gpio_set_level(PIN_RESET, 0);
    gpio_set_level(PIN_RESET, 1);
    esp32_hal.sda   = PIN_SDA;
    esp32_hal.scl  = PIN_SCL;
    u8g2_esp32_hal_init(esp32_hal);
    u8g2_Setup_ssd1306_i2c_128x32_univision_f(
        &u8g2,
        U8G2_R0,
        //u8x8_byte_sw_i2c,
        u8g2_esp32_i2c_byte_cb,
        u8g2_esp32_gpio_and_delay_cb);  // init u8g2 structure
    u8x8_SetI2CAddress(&u8g2.u8x8,0x78);
    regs_global->vars.i2c_display_address = 0x78>>1;
    u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
    main_printf(TAG, "init display passed");
    u8g2_SetPowerSave(&u8g2, 0); // wake up display
    main_printf(TAG, "set power save passed");
    u8g2_ClearBuffer(&u8g2);
    char address[64] = {0};
    sprintf(address,"mdb: %u, ip: %u.%u.%u.%u",regs_global->vars.mdb_addr,regs_global->vars.sta_ip[0],regs_global->vars.sta_ip[1],regs_global->vars.sta_ip[2],regs_global->vars.sta_ip[3]);
    u8g2_SetFont(&u8g2, u8g2_font_5x8_tf);
    u8g2_DrawStr(&u8g2, 0,7, address);
    u8g2_SendBuffer(&u8g2);
    return res;
#endif
}
#endif

/*
 * Initialze the ESP32 HAL.
 */
void u8g2_esp32_hal_init(u8g2_esp32_hal_t u8g2_esp32_hal_param) {
	u8g2_esp32_hal = u8g2_esp32_hal_param;
} // u8g2_esp32_hal_init

/*
 * HAL callback function as prescribed by the U8G2 library.  This callback is invoked
 * to handle SPI communications.
 */
uint8_t u8g2_esp32_spi_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
	main_debug(TAG, "spi_byte_cb: Received a msg: %d, arg_int: %d, arg_ptr: %p", msg, arg_int, arg_ptr);
	switch(msg) {
		case U8X8_MSG_BYTE_SET_DC:
			if (u8g2_esp32_hal.dc != U8G2_ESP32_HAL_UNDEFINED) {
				gpio_set_level(u8g2_esp32_hal.dc, arg_int);
			}
			break;
/* arg_int: expected cs level after processing this msg */
		case U8X8_MSG_CAD_START_TRANSFER: {
			if (u8g2_esp32_hal.cs != U8G2_ESP32_HAL_UNDEFINED) {
				gpio_set_level(u8g2_esp32_hal.cs, 0);
			}
			break;
		}
/* arg_int: expected cs level after processing this msg */
		case U8X8_MSG_CAD_END_TRANSFER:
			if (u8g2_esp32_hal.cs != U8G2_ESP32_HAL_UNDEFINED) {
				gpio_set_level(u8g2_esp32_hal.cs, 1);
			}
			break;
		case U8X8_MSG_BYTE_INIT: {
			if (u8g2_esp32_hal.clk == U8G2_ESP32_HAL_UNDEFINED ||
					u8g2_esp32_hal.mosi == U8G2_ESP32_HAL_UNDEFINED ||
					u8g2_esp32_hal.cs == U8G2_ESP32_HAL_UNDEFINED) {
				break;
			}

		  spi_bus_config_t bus_config;
                  memset(&bus_config, 0, sizeof(spi_bus_config_t));
		  bus_config.sclk_io_num   = u8g2_esp32_hal.clk; // CLK
		  bus_config.mosi_io_num   = u8g2_esp32_hal.mosi; // MOSI
		  bus_config.miso_io_num   = -1; // MISO
		  bus_config.quadwp_io_num = -1; // Not used
		  bus_config.quadhd_io_num = -1; // Not used
		  //ESP_LOGI(TAG, "... Initializing bus.");
#if CONFIG_IDF_TARGET_ESP32
		  ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &bus_config, 1));
#elif CONFIG_IDF_TARGET_ESP32C3
		  ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO));
#endif

		  spi_device_interface_config_t dev_config;
		  dev_config.address_bits     = 0;
		  dev_config.command_bits     = 0;
		  dev_config.dummy_bits       = 0;
		  dev_config.mode             = 0;
		  dev_config.duty_cycle_pos   = 0;
		  dev_config.cs_ena_posttrans = 0;
		  dev_config.cs_ena_pretrans  = 0;
		  dev_config.clock_speed_hz   = 100000;
		  dev_config.spics_io_num     = u8g2_esp32_hal.cs;
		  dev_config.flags            = 0;
		  dev_config.queue_size       = 200;
		  dev_config.pre_cb           = NULL;
		  dev_config.post_cb          = NULL;
		  //ESP_LOGI(TAG, "... Adding device bus.");
#if CONFIG_IDF_TARGET_ESP32
		  ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &dev_config, &handle_spi));
#elif CONFIG_IDF_TARGET_ESP32C3
		  ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &dev_config, &handle_spi));
#endif		  

		  break;
		}

		case U8X8_MSG_BYTE_SEND: {
			spi_transaction_t trans_desc;
			trans_desc.addr      = 0;
			trans_desc.cmd   	 = 0;
			trans_desc.flags     = 0;
			trans_desc.length    = 8 * arg_int; // Number of bits NOT number of bytes.
			trans_desc.rxlength  = 0;
			trans_desc.tx_buffer = arg_ptr;
			trans_desc.rx_buffer = NULL;

			//ESP_LOGI(TAG, "... Transmitting %d bytes.", arg_int);
			ESP_ERROR_CHECK(spi_device_transmit(handle_spi, &trans_desc));
			break;
		}
	}
	return 0;
} // u8g2_esp32_spi_byte_cb

/*
 * HAL callback function as prescribed by the U8G2 library.  This callback is invoked
 * to handle I2C communications.
 */
uint8_t u8g2_esp32_i2c_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
	ESP_LOGD(TAG, "i2c_cb: Received a msg: %d, arg_int: %d, arg_ptr: %p", msg, arg_int, arg_ptr);

	switch(msg) {
		case U8X8_MSG_BYTE_SET_DC: {
			if (u8g2_esp32_hal.dc != U8G2_ESP32_HAL_UNDEFINED) {
				gpio_set_level(u8g2_esp32_hal.dc, arg_int);
			}
			break;
		}

		case U8X8_MSG_BYTE_INIT: {
			if (u8g2_esp32_hal.sda == U8G2_ESP32_HAL_UNDEFINED ||
					u8g2_esp32_hal.scl == U8G2_ESP32_HAL_UNDEFINED) {
				break;
			}

            i2c_config_t conf = {0};
		    conf.mode = I2C_MODE_MASTER;
			ESP_LOGI(TAG, "sda_io_num %d", u8g2_esp32_hal.sda);
		    conf.sda_io_num = u8g2_esp32_hal.sda;
		    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
			ESP_LOGI(TAG, "scl_io_num %d", u8g2_esp32_hal.scl);
		    conf.scl_io_num = u8g2_esp32_hal.scl;
		    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
			ESP_LOGI(TAG, "clk_speed %d", I2C_MASTER_FREQ_HZ);
		    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
			ESP_LOGI(TAG, "i2c_param_config %d", conf.mode);
            ESP_LOGI(TAG, "i2c_param_config flags %d", conf.clk_flags);
            ESP_LOGI(TAG, "i2c_param_config speed %d", conf.master.clk_speed);
		    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
			ESP_LOGI(TAG, "i2c_driver_install %d", I2C_MASTER_NUM);
		    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0));
			break;
		}

		case U8X8_MSG_BYTE_SEND: {
			uint8_t* data_ptr = (uint8_t*)arg_ptr;
			ESP_LOG_BUFFER_HEXDUMP(TAG, data_ptr, arg_int, ESP_LOG_VERBOSE);

			while( arg_int > 0 ) {
			   ESP_ERROR_CHECK(i2c_master_write_byte(handle_i2c, *data_ptr, ACK_CHECK_EN));
			   data_ptr++;
			   arg_int--;
			}
			break;
		}

		case U8X8_MSG_BYTE_START_TRANSFER: {
			uint8_t i2c_address = u8x8_GetI2CAddress(u8x8);
			handle_i2c = i2c_cmd_link_create();
			ESP_LOGD(TAG, "Start I2C transfer to %02X.", i2c_address>>1);
			ESP_ERROR_CHECK(i2c_master_start(handle_i2c));
			ESP_ERROR_CHECK(i2c_master_write_byte(handle_i2c, i2c_address | I2C_MASTER_WRITE, ACK_CHECK_EN));
			break;
		}

		case U8X8_MSG_BYTE_END_TRANSFER: {
			ESP_LOGD(TAG, "End I2C transfer.");
			ESP_ERROR_CHECK(i2c_master_stop(handle_i2c));
			ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_MASTER_NUM, handle_i2c, I2C_TIMEOUT_MS / portTICK_RATE_MS));
			i2c_cmd_link_delete(handle_i2c);
			break;
		}
	}
	return 0;
} // u8g2_esp32_i2c_byte_cb

/*
 * HAL callback function as prescribed by the U8G2 library.  This callback is invoked
 * to handle callbacks for GPIO and delay functions.
 */
uint8_t u8g2_esp32_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
	ESP_LOGD(TAG, "gpio_and_delay_cb: Received a msg: %d, arg_int: %d, arg_ptr: %p", msg, arg_int, arg_ptr);

	switch(msg) {
	// Initialize the GPIO and DELAY HAL functions.  If the pins for DC and RESET have been
	// specified then we define those pins as GPIO outputs.
		case U8X8_MSG_GPIO_AND_DELAY_INIT: {
			uint64_t bitmask = 0;
			if (u8g2_esp32_hal.dc != U8G2_ESP32_HAL_UNDEFINED) {
				bitmask = bitmask | (1ull<<u8g2_esp32_hal.dc);
			}
			if (u8g2_esp32_hal.reset != U8G2_ESP32_HAL_UNDEFINED) {
				bitmask = bitmask | (1ull<<u8g2_esp32_hal.reset);
			}
			if (u8g2_esp32_hal.cs != U8G2_ESP32_HAL_UNDEFINED) {
				bitmask = bitmask | (1ull<<u8g2_esp32_hal.cs);
			}

            if (bitmask==0) {
            	break;
            }
			gpio_config_t gpioConfig;
			gpioConfig.pin_bit_mask = bitmask;
			gpioConfig.mode         = GPIO_MODE_OUTPUT;
			gpioConfig.pull_up_en   = GPIO_PULLUP_DISABLE;
			gpioConfig.pull_down_en = GPIO_PULLDOWN_ENABLE;
			gpioConfig.intr_type    = GPIO_INTR_DISABLE;
			gpio_config(&gpioConfig);
			break;
		}

	// Set the GPIO reset pin to the value passed in through arg_int.
		case U8X8_MSG_GPIO_RESET:
			if (u8g2_esp32_hal.reset != U8G2_ESP32_HAL_UNDEFINED) {
				gpio_set_level(u8g2_esp32_hal.reset, arg_int);
			}
			break;
	// Set the GPIO client select pin to the value passed in through arg_int.
		case U8X8_MSG_GPIO_CS:
			if (u8g2_esp32_hal.cs != U8G2_ESP32_HAL_UNDEFINED) {
				gpio_set_level(u8g2_esp32_hal.cs, arg_int);
			}
			break;
	// Set the Software I²C pin to the value passed in through arg_int.
		case U8X8_MSG_GPIO_I2C_CLOCK:
			if (u8g2_esp32_hal.scl != U8G2_ESP32_HAL_UNDEFINED) {
				gpio_set_level(u8g2_esp32_hal.scl, arg_int);
//				printf("%c",(arg_int==1?'C':'c'));
			}
			break;
	// Set the Software I²C pin to the value passed in through arg_int.
		case U8X8_MSG_GPIO_I2C_DATA:
			if (u8g2_esp32_hal.sda != U8G2_ESP32_HAL_UNDEFINED) {
				gpio_set_level(u8g2_esp32_hal.sda, arg_int);
//				printf("%c",(arg_int==1?'D':'d'));
			}
			break;

	// Delay for the number of milliseconds passed in through arg_int.
		case U8X8_MSG_DELAY_MILLI:
			vTaskDelay(arg_int/portTICK_PERIOD_MS);
			break;
	}
	return 0;
} // u8g2_esp32_gpio_and_delay_cb
