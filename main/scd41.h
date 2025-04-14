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

#ifdef __cplusplus
extern "C" {
#endif

enum scd41_task_event {
    SCD41_TASK_STOP = (1 << 0), /*!< Stop the SCD41 task */
};

extern task_handle_t scd41_task_handle;

void scd41_task(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* SCD41_H */
