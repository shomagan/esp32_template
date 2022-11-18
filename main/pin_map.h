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
#define DRIVE_PIN 25
#define GPIO_WATER_COUNTER_INPUT     23
#define GPIO_WATER_COUNTER_INPUT_MASK  (1ULL<<GPIO_WATER_COUNTER_INPUT)



/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //PIN_MAP_H
