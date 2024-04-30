/**
 * @file pin_map.h
 * @author Shoma Gane <shomagan@gmail.com>
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 * @defgroup ../main/
 * @ingroup ../main/
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
/*
 * Copyright (c) 2018 Snema Service
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the sofi PLC.
 *
 * Author: Shoma Gane <shomagan@gmail.com>
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 */
#ifndef PIN_MAP_H
#define PIN_MAP_H 1
 
/*add includes below */
#include "type_def.h"

/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
/*add functions and variable declarations below */
#if CONFIG_IDF_TARGET_ESP32
#define CONFIG_LED_BLINK_GPIO (2)
#define CONFIG_RXD_GPIO (16)
#define CONFIG_TXD_GPIO (17)
// ssd1306 SDA - GPIO21
#define PIN_SDA 21u
// ssd1306 SCL - GPIO22
#define PIN_SCL 22u
// ssd1306 reset 
#define PIN_RESET 19u
//touch pind config
#define GPIO_TOUCH_0 0
#define GPIO_TOUCH_1 7
#define GPIO_TOUCH_2 8
#define GPIO_TOUCH_3 9
#define DRIVE_PIN 26
#define GPIO_WATER_COUNTER_INPUT     23
#define GPIO_WATER_COUNTER_INPUT_MASK  (1ULL<<GPIO_WATER_COUNTER_INPUT)

/*PWM part*/
#define GPIO_PWM0A_OUT 13   //Set GPIO 13 as PWM0A
#define GPIO_PWM0B_OUT 15   //Set GPIO 15 as PWM0A

#define GPIO_STEP0_OUT 13   //Set GPIO 13 as STEP0
#define GPIO_STEP1_OUT 15   //Set GPIO 15 as STEP1
#define GPIO_STEP2_OUT 14   //Set GPIO 14 as STEP2
#define GPIO_STEP3_OUT 12   //Set GPIO 12 as STEP3
/*DI_HANDLER_PART*/
#define DI_HANDLER_PIN12_INPUT   12 /*sr04 trigger*/
#define DI_HANDLER_PIN13_INPUT   13 /*sr04 echo signal*/
#define DI_HANDLER_PIN14_INPUT   14
#define DI_HANDLER_PIN27_INPUT   27
#define DI_HANDLER_PIN26_INPUT   26

#define DI_HANDLER_PIN23_INPUT   23
#define DI_HANDLER_PIN32_INPUT   32
#define DI_HANDLER_PIN15_INPUT   15
#define DI_HANDLER_PIN18_INPUT   18
#define DI_HANDLER_PIN4_INPUT_RESERV   4
#define DI_HANDLER_PIN5_INPUT_RESERV   5

#define GPIO_OUTPUT_STEP_MOTOR_EN     13
#define GPIO_OUTPUT_STEP_MOTOR_DIR0     15
#define GPIO_OUTPUT_STEP_MOTOR_STEP0     14
#define GPIO_OUTPUT_STEP_MOTOR_SLEEP     12
#define GPIO_OUTPUT_STEP_MOTOR_DIR1    15
#define GPIO_OUTPUT_STEP_MOTOR_STEP1     14

#define EXT_WAKEUP_PIN 25

#elif CONFIG_IDF_TARGET_ESP32C3

/*
GP2 is strapping pin  ┌───────┐
GP2   a0     d0    ───┤       ├───5V
GP3   a1     d1    ───┤       ├───gnd
GP4   a2     d2    ───┤       ├───3v3
GP5   a3     d3    ───┤       ├───d10 mosi GP10
GP6   sda    d4    ───┤       ├───d9  miso GP9 strapping pin
GP7   scl    d5    ───┤       ├───d8  sck  GP8 strapping pin
GP21  tx     d6    ───┤       ├───d7  rx   GP20
                      └───────┘
*/
#define CONFIG_LED_BLINK_GPIO (9)
#define GPIO_OUTPUT_STEP_MOTOR_EN     10
#define GPIO_OUTPUT_STEP_MOTOR_DIR0     20
#define GPIO_OUTPUT_STEP_MOTOR_STEP0     6
#define GPIO_OUTPUT_STEP_MOTOR_SLEEP     7
#define GPIO_OUTPUT_STEP_MOTOR_DIR1    20
#define GPIO_OUTPUT_STEP_MOTOR_STEP1     6
#define EXT_WAKEUP_PIN 3

#endif

/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //PIN_MAP_H
