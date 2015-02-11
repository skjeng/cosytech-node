/**
 *
 * \file
 *
 * \brief WINC1500 configuration.
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef CONF_WIFI_M2M_H_INCLUDED
#define CONF_WIFI_M2M_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include "cmsis/samd21/include/pio/samd21g18a.h"

/** Module Pin Settings */
#define CONF_WIFI_M2M_RESET_PIN                 PIN_PB08 //
#define CONF_WIFI_M2M_CHIP_ENABLE_PIN           PIN_PB09
#define CONF_WIFI_M2M_WAKE_PIN                  PIN_PA04

#define CONF_WIFI_M2M_INT_PIN                   PIN_PA01A_EIC_EXTINT1; //EXT1_IRQ_PIN; no idea if this is correct at 
#define CONF_WIFI_M2M_INT_MUX                   MUX_PA01A_EIC_EXTINT1; //EXT1_IRQ_MUX;MUX_PB04A_EIC_EXTINT4
#define CONF_WIFI_M2M_INT_EIC                   (1) //IRQ_INPUT

/** INTERRUPT Detect Mode Settings */
#define CONF_WIFI_M2M_EDGE_INTERRUPT            (1)
/* #define CONF_WIFI_M2M_LEVEL_INTERRUPT        (1) */

/** BUS Type Settings */
#define CONF_WIFI_M2M_SPI                       (1)
/* #define CONF_WIFI_M2M_I2C                    (1) */

/** SPI Settings */
#define CONF_WIFI_M2M_SPI_MODULE                SERCOM0 // EXT1_SPI_MODULE
#define CONF_WIFI_M2M_SPI_SERCOM_MUX            SPI_SIGNAL_MUX_SETTING_E // EXT1_SPI_SERCOM_MUX_SETTING 
#define CONF_WIFI_M2M_SPI_PINMUX_PAD0           PINMUX_PA04D_SERCOM0_PAD0 //EXT1_SPI_SERCOM_PINMUX_PAD0 /* in */ //
#define CONF_WIFI_M2M_SPI_PINMUX_PAD1           PINMUX_UNUSED /* cs driven from software */
#define CONF_WIFI_M2M_SPI_PINMUX_PAD2           PINMUX_PA06D_SERCOM0_PAD2  /* out */ //EXT1_SPI_SERCOM_PINMUX_PAD2
#define CONF_WIFI_M2M_SPI_PINMUX_PAD3           PINMUX_PA07D_SERCOM0_PAD3  /* sck */ //EXT1_SPI_SERCOM_PINMUX_PAD3
#define CONF_WIFI_M2M_SPI_CS_PIN                PIN_PA05  // EXT1_PIN_SPI_SS_0 EXT1_PIN_15 // PIN_PA05
#define CONF_WIFI_M2M_SPI_BAUDRATE              (10000000UL)

/** Debug Options */
#define CONF_WIFI_M2M_DEBUG                     (0)
#define CONF_WIFI_M2M_PRINTF                    printf

#ifdef __cplusplus
}
#endif

#endif /* CONF_WIFI_M2M_H_INCLUDED */
