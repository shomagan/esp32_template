/**
 * @file scd41.h
 * @brief Header file for SCD41 sensor task and driver.
 */

#ifndef SCD41_H
#define SCD41_H 1

#include "type_def.h"
#include "main_config.h"
#include "regs.h"
#include "common.h"

#define SCD41_ARRAY_SIZE 48

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief struct for SCD41 sensor component
 * name variables use for generating names in the description file and then in get value by name.
 * Therefore, use max size len name is 16 characters.
 * Comment style:
 *   "" - description,
 *   &ro  - read only,
 *   &def -> have const variable with struct like def_name,
 *   &save- will have saved in bkram,
 * @ingroup regs
 */
/** #generator_use_description {"space_name" :"scd41_reg_t",  "address_space" :11, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :4800}*/
typedef union {
    struct MCU_PACK {
        // start regs struct
        u16 scd41_co2_level;         //!< "CO2 level in ppm" &ro
        float scd41_temperature;     //!< "Temperature in Celsius" &ro
        float scd41_humidity;        //!< "Humidity in percentage" &ro
        u16 scd41_settings;    //!< "Settings for SCD41 sensor" &save
        u16 scd41_measur_interval;   //!< "minutes between a measurement" &save &def &min &max
        u16 scd41_index_in_array;   //!< "Index in array" &ro &save
        u16 scd41_co2_array[SCD41_ARRAY_SIZE];         //!< "CO2 level in ppm" &ro &save
    } vars;
    u32 bytes[32]; // for full bksram copy
} scd41_reg_t; // #generator_use_description {"message":"end_struct"}
extern scd41_reg_t * const scd41_reg;

/* Default/min/max values for scd41_measur_interval (&def &min &max) */
extern const u16 def_scd41_measur_interval;
extern const u16 def_min_scd41_measur_interval;
extern const u16 def_max_scd41_measur_interval;

enum scd41_task_event {
    SCD41_TASK_STOP = (1 << 0), /*!< Stop the SCD41 task */
};

extern task_handle_t scd41_task_handle;

void scd41_task(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* SCD41_H */
